/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "SubtitleProcessor.h"

// C includes

#include <stdarg.h>

// Qt includes

#include <QCoreApplication>
#include <QEventLoop>
#include <QDir>
#include <QFile>
#include <QMutex>
#include <QThread>

// Local includes

#include "Packet.h"
#include "QtAV_factory.h"
#include "PlainText.h"
#include "QtAV_internal.h"
#include "digikam_debug.h"

/*
#define ASS_CAPI_NS // do not unload() manually!
#define CAPI_LINK_ASS
*/

#include "ass_api.h"

namespace QtAV
{

void RenderASS(QImage* image, const SubImage& img, int dstX, int dstY);

class Q_DECL_HIDDEN SubtitleProcessorLibASS final : public SubtitleProcessor,
                                                    protected ass::api
{
public:

    SubtitleProcessorLibASS();
    ~SubtitleProcessorLibASS();

    void updateFontCache();
    SubtitleProcessorId id()      const                                                     override;
    QString name()                const                                                     override;
    QStringList supportedTypes()  const                                                     override;
    bool process(QIODevice* dev)                                                            override;

    // supportsFromFile must be true

    bool process(const QString& path)                                                       override;
    QList<SubtitleFrame> frames() const                                                     override;

    bool canRender()              const                                                     override
    {
        return true;
    }

    QString getText(qreal pts)    const                                                     override;
    QImage getImage(qreal pts, QRect* boundingRect = nullptr)                               override;
    SubImageSet getSubImages(qreal pts, QRect* boundingRect)                                override;
    bool processHeader(const QByteArray& codec, const QByteArray& data)                     override;
    SubtitleFrame processLine(const QByteArray& data, qreal pts = -1, qreal duration = 0)   override;
    void setFontFile(const QString& file)                                                   override;
    void setFontsDir(const QString& dir)                                                    override;
    void setFontFileForced(bool force)                                                      override;

protected:

    void onFrameSizeChanged(int width, int height)                                          override;

private:

    bool initRenderer();
    void updateFontCacheAsync();          // cppcheck-suppress unusedPrivateFunction
    SubImageSet getSubImages(qreal pts, QRect* boundingRect, QImage* qimg, bool copy);
    void processTrack(ASS_Track* track);

private:

    bool                 m_update_cache;
    bool                 force_font_file; ///< works only if font_file is set
    QString              font_file;
    QString              fonts_dir;
    QByteArray           m_codec;
    ASS_Library*         m_ass;
    ASS_Renderer*        m_renderer;
    ASS_Track*           m_track;
    QList<SubtitleFrame> m_frames;

    // cache the image for the last invocation. return this if image does not change

    QImage               m_image;
    SubImageSet          m_assimages;
    QRect                m_bound;
    mutable QMutex       m_mutex;
};

static const SubtitleProcessorId SubtitleProcessorId_LibASS = QLatin1String("qtav.subtitle.processor.libass");

namespace
{

static const char kName[] = "LibASS";

} // namespace

FACTORY_REGISTER(SubtitleProcessor, LibASS, kName)

// log level from ass_utils.h

#define MSGL_FATAL 0
#define MSGL_ERR   1
#define MSGL_WARN  2
#define MSGL_INFO  4
#define MSGL_V     6
#define MSGL_DBG2  7

static void ass_msg_cb(int level, const char* fmt, va_list va, void* data)
{
    Q_UNUSED(data)

    if (level > MSGL_INFO)
        return;

#ifdef Q_OS_WIN

    if (level == MSGL_WARN)
    {
       return; // crash at warnings from fontselect
    }

#endif

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << "[libass]: " << QString::vasprintf(fmt, va);

/*
    QString msg(QLatin1String("{libass} ") + QString().vasprintf(fmt, va));

    if      (level == MSGL_FATAL)
        qFatal("%s", msg.toUtf8().constData());
    else if (level <= 2)
        qCWarning(DIGIKAM_QTAV_LOG_WARN) << msg;
    else if (level <= MSGL_INFO)
        qCDebug(DIGIKAM_QTAV_LOG) << msg;
*/

}

SubtitleProcessorLibASS::SubtitleProcessorLibASS()
    : m_update_cache (true),
      force_font_file(true),
      m_ass          (nullptr),
      m_renderer     (nullptr),
      m_track        (nullptr)
{
    if (!ass::api::loaded())
        return;

    m_ass = ass_library_init();

    if (!m_ass)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("ass_library_init failed!");

        return;
    }

    ass_set_message_cb(m_ass, ass_msg_cb, nullptr);
}

SubtitleProcessorLibASS::~SubtitleProcessorLibASS()
{
    // ass dll is loaded if ass objects are available

    if (m_track)
    {
        ass_free_track(m_track);
        m_track = nullptr;
    }

    if (m_renderer)
    {
        QMutexLocker lock(&m_mutex);
        Q_UNUSED(lock);
        ass_renderer_done(m_renderer); // check async update cache!!
        m_renderer = nullptr;
    }

    if (m_ass)
    {
        ass_library_done(m_ass);
        m_ass = nullptr;
    }
}

SubtitleProcessorId SubtitleProcessorLibASS::id() const
{
    return SubtitleProcessorId_LibASS;
}

QString SubtitleProcessorLibASS::name() const
{
    return QLatin1String(kName); // SubtitleProcessorFactory::name(id());
}

QStringList SubtitleProcessorLibASS::supportedTypes() const
{
    // from LibASS/tests/fate/subtitles.mak
    // TODO: mp4

    static const QStringList sSuffixes = QStringList() << QLatin1String("ass")
                                                       << QLatin1String("ssa");

    return sSuffixes;
}

QList<SubtitleFrame> SubtitleProcessorLibASS::frames() const
{
    return m_frames;
}

bool SubtitleProcessorLibASS::process(QIODevice* dev)
{
    if (!ass::api::loaded())
        return false;

    QMutexLocker lock(&m_mutex);
    Q_UNUSED(lock);

    if (m_track)
    {
        ass_free_track(m_track);
        m_track = nullptr;
    }

    if (!dev->isOpen())
    {
        if (!dev->open(QIODevice::ReadOnly))
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN)
                << "open qiodevice error: "
                    << dev->errorString();

            return false;
        }
    }

    QByteArray data(dev->readAll());
    m_track = ass_read_memory(m_ass, (char*)data.constData(), data.size(), nullptr); //utf-8

    if (!m_track)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("ass_read_memory error, ass track init failed!");

        return false;
    }

    processTrack(m_track);

    return true;
}

bool SubtitleProcessorLibASS::process(const QString& path)
{
    if (!ass::api::loaded())
        return false;

    QMutexLocker lock(&m_mutex);
    Q_UNUSED(lock);

    if (m_track)
    {
        ass_free_track(m_track);
        m_track = nullptr;
    }

    m_track = ass_read_file(m_ass, (char*)path.toUtf8().constData(), nullptr);

    if (!m_track)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("ass_read_file error, ass track init failed!");

        return false;
    }

    processTrack(m_track);

    return true;
}

bool SubtitleProcessorLibASS::processHeader(const QByteArray& codec, const QByteArray& data)
{
    if (!ass::api::loaded())
        return false;

    QMutexLocker lock(&m_mutex);
    Q_UNUSED(lock);
    m_codec = codec;
    m_frames.clear();
    setFrameSize(-1, -1);

    if (m_track)
    {
        ass_free_track(m_track);
        m_track = nullptr;
    }

    m_track = ass_new_track(m_ass);

    if (!m_track)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("failed to create an ass track");

        return false;
    }

    ass_process_codec_private(m_track, (char*)data.constData(), data.size());

    return true;
}

SubtitleFrame SubtitleProcessorLibASS::processLine(const QByteArray& data, qreal pts, qreal duration)
{
    if (!ass::api::loaded())
        return SubtitleFrame();

    if (data.isEmpty() || (data.at(0) == 0))
        return SubtitleFrame();

    QMutexLocker lock(&m_mutex);
    Q_UNUSED(lock);

    if (!m_track)
        return SubtitleFrame();

    const int nb_tracks = m_track->n_events;

    // TODO: confirm. ass/ssa path from mpv

    if (m_codec == QByteArray("ass"))
    {
        ass_process_chunk(m_track, (char*)data.constData(), data.size(), pts*1000.0, duration*1000.0);
    }
    else
    {
        // ssa. mpv: flush_on_seek, broken ffmpeg ASS packet format

        ass_process_data(m_track, (char*)data.constData(), data.size());
    }

    if (nb_tracks == m_track->n_events)
        return SubtitleFrame();
/*
    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("events: %d", m_track->n_events);
*/
    for (int i = m_track->n_events - 1 ; i >= 0 ; --i)
    {
        const ASS_Event& ae = m_track->events[i];
/*
        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("ass_event[%d] %lld+%lld/%lld+%lld: %s",
                i, ae.Start, ae.Duration, (long long)(pts*1000.0),
                (long long)(duration*1000.0), ae.Text);

        packet.duration can be 0
*/
        if (ae.Start == (long long)(pts * 1000.0))
        {
            // && ae.Duration == (long long)(duration*1000.0)) {

            SubtitleFrame frame;
            frame.text  = PlainText::fromAss(ae.Text);
            frame.begin = qreal(ae.Start) / 1000.0;
            frame.end   = frame.begin + qreal(ae.Duration) / 1000.0;

            return frame;
        }
    }

    return SubtitleFrame();
}

QString SubtitleProcessorLibASS::getText(qreal pts) const
{
    QMutexLocker lock(&m_mutex);
    Q_UNUSED(lock);
    QString text;

    for (int i = 0 ; i < m_frames.size() ; ++i)
    {
        if ((m_frames[i].begin <= pts) && (m_frames[i].end >= pts))
        {
            text += m_frames[i].text + QLatin1String("\n");

            continue;
        }

        if (!text.isEmpty())
            break;
    }

    return text.trimmed();
}

void renderASS32(QImage* image, ASS_Image* img, int dstX, int dstY);

QImage SubtitleProcessorLibASS::getImage(qreal pts, QRect* boundingRect)
{
    // ass dll is loaded if ass library is available

    getSubImages(pts, boundingRect, &m_image, false);
    m_assimages.reset();

    return m_image;
}

SubImageSet SubtitleProcessorLibASS::getSubImages(qreal pts, QRect* boundingRect)
{
    m_assimages = getSubImages(pts, boundingRect, nullptr, true);

    return m_assimages;
}

SubImageSet SubtitleProcessorLibASS::getSubImages(qreal pts, QRect* boundingRect, QImage* qimg, bool copy)
{
    // ass dll is loaded if ass library is available

    {
        QMutexLocker lock(&m_mutex);
        Q_UNUSED(lock);

        if (!m_ass)
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("ass library not available");

            return SubImageSet();
        }

        if (!m_track)
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("ass track not available");

            return SubImageSet();
        }

        if (!m_renderer)
        {
            initRenderer();

            if (!m_renderer)
            {
                qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                    << QString::asprintf("ass renderer not available");

                return SubImageSet();
            }
        }
    }

    if (m_update_cache)
        updateFontCache();

    QMutexLocker lock(&m_mutex);
    Q_UNUSED(lock);

    if (!m_renderer) // reset in setFontXXX
        return SubImageSet();

    int detect_change = 0;
    ASS_Image* img    = ass_render_frame(m_renderer, m_track, (long long)(pts * 1000.0), &detect_change);

    if (!detect_change && !m_assimages.isValid())
    {
        if (boundingRect)
            *boundingRect = m_bound;

        return m_assimages;
    }

    m_image      = QImage();
    m_assimages.reset(frameWidth(), frameHeight(), SubImageSet::ASS);
    QRect rect(0, 0, 0, 0);
    ASS_Image* i = img;

    while (i)
    {
        const quint8 a = 255 - (i->color&0xff);
/*
        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("ass %d rect %d: %d,%d-%dx%d",
                a, n, i->dst_x, i->dst_y, i->w, i->h);
*/
        if ((i->w <= 0) || (i->h <= 0) || (a == 0))
        {
            i = i->next;

            continue;
        }

        SubImage s(i->dst_x, i->dst_y, i->w, i->h, i->stride);
        s.color = i->color;

        if (copy)
        {
            s.data.reserve(i->stride*i->h);
            s.data.resize(i->stride*i->h);
            memcpy(s.data.data(), i->bitmap, i->stride * (i->h - 1) + i->w);
        }
        else
        {
            s.data = QByteArray::fromRawData((const char*)i->bitmap, i->stride * (i->h - 1) + i->w);
        }

        m_assimages.images.append(s);
        rect |= QRect(i->dst_x, i->dst_y, i->w, i->h);
        i     = i->next;
    }

    m_bound = rect;

    if (boundingRect)
    {
        *boundingRect = m_bound;
    }

    if (!qimg)
        return m_assimages;

    *qimg = QImage(rect.size(), QImage::Format_ARGB32);
    qimg->fill(Qt::transparent);

    Q_FOREACH (const SubImage& subimg, m_assimages.images)
    {
        RenderASS(qimg, subimg, subimg.x - rect.x(), subimg.y - rect.y());
    }

    return m_assimages;
}

void SubtitleProcessorLibASS::onFrameSizeChanged(int width, int height)
{
    if ((width < 0) || (height < 0))
        return;

    if (!m_renderer)
    {
        initRenderer();
    }

    if (!m_renderer)        // cppcheck-suppress duplicateCondition
        return;

    ass_set_frame_size(m_renderer, width, height);
}

void SubtitleProcessorLibASS::setFontFile(const QString &file)
{
    if (font_file == file)
        return;

    font_file      = file;
    m_update_cache = true; // update renderer when getting the next image

    if (m_renderer)
    {
        QMutexLocker lock(&m_mutex);
        Q_UNUSED(lock);

        // resize frame to ensure renderer can be resized later

        setFrameSize(-1, -1);
        ass_renderer_done(m_renderer);
        m_renderer = nullptr;
    }
}

void SubtitleProcessorLibASS::setFontFileForced(bool force)
{
    if (force_font_file == force)
        return;

    force_font_file = force;

    // FIXME: sometimes crash

    m_update_cache  = true; // update renderer when getting the next image

    if (m_renderer)
    {
        QMutexLocker lock(&m_mutex);
        Q_UNUSED(lock);

        // resize frame to ensure renderer can be resized later

        setFrameSize(-1, -1);
        ass_renderer_done(m_renderer);
        m_renderer = nullptr;
    }
}

void SubtitleProcessorLibASS::setFontsDir(const QString& dir)
{
    if (fonts_dir == dir)
        return;

    fonts_dir      = dir;
    m_update_cache = true; // update renderer when getting the next image

    if (m_renderer)
    {
        QMutexLocker lock(&m_mutex);
        Q_UNUSED(lock);

        // resize frame to ensure renderer can be resized later

        setFrameSize(-1, -1);
        ass_renderer_done(m_renderer);
        m_renderer = nullptr;
    }
}

bool SubtitleProcessorLibASS::initRenderer()
{
/*
    ass_set_extract_fonts(m_ass, 1);
    ass_set_style_overrides(m_ass, 0);
*/
    m_renderer = ass_renderer_init(m_ass);

    if (!m_renderer)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("ass_renderer_init failed!");

        return false;
    }

#if LIBASS_VERSION >= 0x01000000

    ass_set_shaper(m_renderer, ASS_SHAPING_SIMPLE);

#endif

    return true;
}

// TODO: set font cache dir. default is working dir which may be not writable on some platforms

void SubtitleProcessorLibASS::updateFontCache()
{
    // ass dll is loaded if renderer is valid

    QMutexLocker lock(&m_mutex);
    Q_UNUSED(lock);

    if (!m_renderer)
        return;

    // fonts in assets and qrc may change. so check before appFontsDir

    static const QStringList kFontsDirs = QStringList()
            << qApp->applicationDirPath().append(QLatin1String("/fonts"))
            << qApp->applicationDirPath() // for winrt
            << QLatin1String("assets:/fonts")
            << QLatin1String(":/fonts")
            << Internal::Path::appFontsDir()

#ifndef Q_OS_WINRT

            << Internal::Path::fontsDir()

#endif

    ;

    // TODO: modify fontconfig cache dir in fonts.conf <dir></dir> then save to conf

    static QString conf(0, QChar()); // FC_CONFIG_FILE?

    if (conf.isEmpty() && !conf.isNull())
    {
        static const QString kFontCfg(QLatin1String("fonts.conf"));

        Q_FOREACH (const QString& fdir, kFontsDirs)
        {
            qCDebug(DIGIKAM_QTAV_LOG) << "looking up "
                                      << kFontCfg
                                      << " in: " << fdir;

            QFile cfg(QString::fromUtf8("%1/%2").arg(fdir).arg(kFontCfg));

            if (!cfg.exists())
                continue;

            conf = cfg.fileName();

            if (fdir.isEmpty()                                                 ||
                fdir.startsWith(QLatin1String("assets:"), Qt::CaseInsensitive) ||
                fdir.startsWith(QLatin1String(":"),       Qt::CaseInsensitive) ||
                fdir.startsWith(QLatin1String("qrc:"),    Qt::CaseInsensitive)
               )
            {
                conf = QString::fromUtf8("%1/%2").arg(Internal::Path::appFontsDir()).arg(kFontCfg);

                qCDebug(DIGIKAM_QTAV_LOG)
                    << "Fonts dir (for config) is not supported by libass. Copy fonts to app fonts dir: "
                    << fdir;

                if (!QDir(Internal::Path::appFontsDir()).exists())
                {
                    if (!QDir().mkpath(Internal::Path::appFontsDir()))
                    {
                        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                            << QString::asprintf("Failed to create fonts dir: %s",
                                Internal::Path::appFontsDir().toUtf8().constData());
                    }
                }

                QFile cfgout(conf);

                if (cfgout.exists() && cfgout.size() != cfg.size())
                {
                    qCDebug(DIGIKAM_QTAV_LOG) << "new " << kFontCfg
                                              << " with the same name. remove old: "
                                              << cfgout.fileName();
                    cfgout.remove();
                }

                if (!cfgout.exists() && !cfg.copy(conf))
                {
                    qCWarning(DIGIKAM_QTAV_LOG_WARN) << "Copy font config file ["
                                                     << cfg.fileName() <<  "] error: "
                                                     << cfg.errorString();

                    continue;
                }
            }

            break;
        }

        if (!QFile(conf).exists())
            conf.clear();

        qCDebug(DIGIKAM_QTAV_LOG) << "FontConfig: " << conf;
    }

    /*
     * Fonts dir look up:
     * - appdir/fonts has fonts
     * - assets:/fonts, qrc:/fonts: copy fonts to appFontsDir
     * - appFontsDir (appdir/fonts has no fonts)
     * - fontsDir if it has font files
     * If defaults.ttf exists in fonts dir(for 'assets:' and 'qrc:' appFontsDir is checked), use it as default font and disable font provider
     * (for example fontconfig) to speed up(skip) libass font look up.
     * Skip setting fonts dir
     */
    static QString sFont(0, QChar());       // if exists, fontconfig will be disabled and directly use this font
    static QString sFontsDir(0, QChar());

    if (sFontsDir.isEmpty() && !sFontsDir.isNull())
    {
        static const QString kDefaultFontName(QLatin1String("default.ttf"));
        static const QStringList ft_filters = QStringList() << QLatin1String("*.ttf")
                                                            << QLatin1String("*.otf")
                                                            << QLatin1String("*.ttc");

        QStringList fonts;

        Q_FOREACH (const QString& fdir, kFontsDirs)
        {
            qCDebug(DIGIKAM_QTAV_LOG) << "looking up fonts in: " << fdir;

            QDir d(fdir);

            if (!d.exists()) // avoid winrt crash (system fonts dir)
                continue;

            fonts = d.entryList(ft_filters, QDir::Files);

            if (fonts.isEmpty())
                continue;

            if (fonts.contains(kDefaultFontName))
            {
                QFile ff(QString::fromUtf8("%1/%2").arg(fdir).arg(kDefaultFontName));

                if (ff.exists() && (ff.size() == 0))
                {
                    qCDebug(DIGIKAM_QTAV_LOG).noquote()
                        << QString::asprintf("invalid default font");

                    fonts.clear();

                    continue;
                }
            }

            sFontsDir = fdir;

            break;
        }

        if (fonts.isEmpty())
        {
            sFontsDir.clear();
        }
        else
        {
            qCDebug(DIGIKAM_QTAV_LOG) << "fonts dir:"
                                      << sFontsDir
                                      << "font files:"
                                      << fonts;

            if (sFontsDir.isEmpty()                                                 ||
                sFontsDir.startsWith(QLatin1String("assets:"), Qt::CaseInsensitive) ||
                sFontsDir.startsWith(QLatin1String(":"),       Qt::CaseInsensitive) ||
                sFontsDir.startsWith(QLatin1String("qrc:"),    Qt::CaseInsensitive)
               )
            {
                const QString fontsdir_in(sFontsDir);
                sFontsDir = Internal::Path::appFontsDir();

                qCDebug(DIGIKAM_QTAV_LOG)
                    << "Fonts dir is not supported by libass. Copy fonts to app fonts dir if not exist: "
                    << sFontsDir;

                if (!QDir(Internal::Path::appFontsDir()).exists())
                {
                    if (!QDir().mkpath(Internal::Path::appFontsDir()))
                    {
                        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                            << QString::asprintf("Failed to create fonts dir: %s",
                                Internal::Path::appFontsDir().toUtf8().constData());
                    }
                }

                Q_FOREACH (const QString& f, fonts)
                {
                    QFile ff(QString::fromUtf8("%1/%2").arg(fontsdir_in).arg(f));
                    const QString kOut(QString::fromUtf8("%1/%2").arg(sFontsDir).arg(f));
                    QFile ffout(kOut);

                    if (ffout.exists() && (ffout.size() != ff.size()))
                    {
                        // TODO:

                        qCDebug(DIGIKAM_QTAV_LOG)
                            << "new font with the same name. remove old: "
                            << ffout.fileName();

                        ffout.remove();
                    }

                    if (!ffout.exists() && !ff.copy(kOut))
                        qCWarning(DIGIKAM_QTAV_LOG_WARN) << "Copy font file ["
                                                         << ff.fileName()
                                                         <<  "] error: "
                                                         << ff.errorString();
                }
            }

            if (fonts.contains(kDefaultFontName))
            {
                sFont = QString::fromUtf8("%1/%2").arg(sFontsDir).arg(kDefaultFontName);

                qCDebug(DIGIKAM_QTAV_LOG) << "default font file: "
                                          << sFont
                                          << "; fonts dir: "
                                          << sFontsDir;
            }
        }
    }

    static QByteArray family; //fallback to Arial?

    if (family.isEmpty())
    {
        family = qgetenv("QTAV_SUB_FONT_FAMILY_DEFAULT");

        // Setting default font to the Arial from default.ttf (used if FontConfig fails)

        if (family.isEmpty())
            family = QByteArray("Arial");
    }

    // prefer user settings

    const QString kFont     = (font_file.isEmpty() ? sFont     : Internal::Path::toLocal(font_file));
    const QString kFontsDir = (fonts_dir.isEmpty() ? sFontsDir : Internal::Path::toLocal(fonts_dir));

    qCDebug(DIGIKAM_QTAV_LOG) << "font file: "
                              << kFont
                              << "; fonts dir: "
                              << kFontsDir;

    // setup libass

#ifdef Q_OS_WINRT

    if (!kFontsDir.isEmpty())
        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("BUG: winrt libass set a valid fonts dir results in crash. skip fonts dir setup.");

#else

    // will call strdup, so safe to use temp array .toUtf8().constData()

    if (!force_font_file || (!font_file.isEmpty() && !QFile::exists(kFont)))
        ass_set_fonts_dir(m_ass, kFontsDir.isEmpty() ? nullptr : kFontsDir.toUtf8().constData()); // look up fonts in fonts dir can be slow. force font file to skip lookup

#endif

    /* ass_set_fonts:
     * fc/dfp=false(auto font provider): Prefer font provider
     * to find a font(FC needs fonts.conf) in font_dir, or provider's configuration.
     * If failed, try the given font fc / dfp = true(no font provider): only try the given font
     */

    // user can prefer font provider(force_font_file=false), or disable font provider to force the given font
    // if provider is enabled, libass can fallback to the given font if provider can not provide a font

    const QByteArray a_conf(conf.toUtf8());
    const char* kConf = conf.isEmpty() ? nullptr : a_conf.constData();

    if (kFont.isEmpty())
    {
        // TODO: always use font provider if no font file is set, i.e. ignore force_font_file

        qCDebug(DIGIKAM_QTAV_LOG).noquote() << QString::asprintf("No font file is set, use font provider");
        ass_set_fonts(m_renderer, nullptr, family.constData(), !force_font_file, kConf, 1);
    }
    else
    {
        qCDebug(DIGIKAM_QTAV_LOG).noquote() << QString::asprintf("Font file is set. force font file: %d", force_font_file);
        ass_set_fonts(m_renderer, kFont.toUtf8().constData(), family.constData(), !force_font_file, kConf, 1);
    }
/*
    ass_fonts_update(m_renderer); // update in ass_set_fonts(....,1)
*/
    m_update_cache = false; // TODO: set true if user set a new font or fonts dir
}

class Q_DECL_HIDDEN FontCacheUpdater : public QThread
{
    Q_OBJECT

public:

    explicit FontCacheUpdater(SubtitleProcessorLibASS* const p)
        : sp(p)
    {
    }

    void run()
    {
        if (!sp)
        {
            return;
        }

        sp->updateFontCache();
    }

private:

    SubtitleProcessorLibASS* sp = nullptr;

private:

    FontCacheUpdater(QObject*);
};

void SubtitleProcessorLibASS::updateFontCacheAsync()
{

    FontCacheUpdater updater(this);
    QEventLoop loop;
/*
    QObject::connect(&updater, SIGNAL(finished()),
                     &loop, SLOT(quit()));
*/
    updater.start();

    while (updater.isRunning())
    {
        loop.processEvents();
    }
/*
    loop.exec(); // what if updater is finished before exec()?
    updater.wait();
*/
}

void SubtitleProcessorLibASS::processTrack(ASS_Track* track)
{
    // language, track type

    m_frames.clear();

    for (int i = 0 ; i < track->n_events ; ++i)
    {
        SubtitleFrame frame;
        const ASS_Event& ae = track->events[i];
        frame.text          = PlainText::fromAss(ae.Text);
        frame.begin         = qreal(ae.Start) / 1000.0;
        frame.end           = frame.begin + qreal(ae.Duration) / 1000.0;
        m_frames.append(frame);
    }
}

} // namespace QtAV

#include "SubtitleProcessorLibASS.moc"
