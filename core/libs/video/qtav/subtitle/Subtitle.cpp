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

#include "Subtitle.h"

// C++ includes

#include <algorithm>

// Qt includes

#include <QBuffer>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QList>
#include <QRunnable>
#include <QThreadPool>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#   include <QTextCodec>
#endif

#include <QTextStream>
#include <QMutexLocker>

// Local includes

#include "QtAV_Global.h"
#include "SubtitleProcessor.h"
#include "CharsetDetector.h"
#include "digikam_debug.h"

namespace QtAV
{

const int kMaxSubtitleSize = 10 * 1024 * 1024; // TODO: remove because we find the matched extenstions

class Q_DECL_HIDDEN Subtitle::Private
{

public:

    Private()
        : loaded            (false),
          fuzzy_match       (true),
          update_text       (true),
          update_image      (true),
          last_can_render   (false),
          processor         (nullptr),
          codec             ("AutoDetect"),
          dev               (nullptr),
          t                 (0),
          delay             (0),
          current_count     (0),
          force_font_file   (false)
    {
    }

    void reset()
    {
        QMutexLocker lock(&mutex);
        Q_UNUSED(lock);

        loaded        = false;
        processor     = nullptr;
        update_text   = true;
        update_image  = true;
        t             = 0;

        frame         = SubtitleFrame();
        frames.clear();

        itf           = frames.begin();
        current_count = 0;
    }

    // width/height == 0: do not create image
    // return true if both frame time and content(currently is text) changed

    bool prepareCurrentFrame();
    QStringList find();

    /*!
     * \brief readFromFile
     * read subtilte content from path
     * \param path
     * \return utf8 encoded content
     */
    QByteArray readFromFile(const QString& path);

    /*!
     * \brief processRawData
     * process utf8 encoded subtitle content. maybe a temp file is created if subtitle processor does not
     * support raw data
     * \param data utf8 subtitle content
     */
    bool processRawData(const QByteArray& data);
    bool processRawData(SubtitleProcessor* sp, const QByteArray& data);

public:

    bool                            loaded;
    bool                            fuzzy_match;
    bool                            update_text;
    bool                            update_image; // TODO: detect image change from engine
    bool                            last_can_render;
    SubtitleProcessor*              processor;
    QList<SubtitleProcessor*>       processors;
    QByteArray                      codec;
    QStringList                     engine_names;
    QList<SubtitleFrame>            frames;
    QUrl                            url;
    QByteArray                      raw_data;
    QString                         file_name;
    QStringList                     dirs;
    QStringList                     suffixes;
    QStringList                     supported_suffixes;
    QIODevice*                      dev;

    // last time image

    qreal                           t;
    qreal                           delay;
    SubtitleFrame                   frame;
    QString                         current_text;
    QImage                          current_image;
    SubImageSet                     current_ass;
    QList<SubtitleFrame>::iterator  itf;

    /* number of subtitle frames at current time.
     * <0 means itf is the last. >0 means itf is the 1st
     */
    int                             current_count;
    QMutex                          mutex;

    bool                            force_font_file;
    QString                         font_file;
    QString                         fonts_dir;
};

Subtitle::Subtitle(QObject* const parent)
    : QObject(parent),
      d      (new Private())
{
    // TODO: use factory.registedNames() and the order

    setEngines(QStringList() << QLatin1String("LibASS") << QLatin1String("FFmpeg"));
}

Subtitle::~Subtitle()
{
    if (d)
    {
        delete d;
        d = nullptr;
    }
}

bool Subtitle::isLoaded() const
{
    return d->loaded;
}

void Subtitle::setCodec(const QByteArray& value)
{
    if (d->codec == value)
        return;

    d->codec = value;

    Q_EMIT codecChanged();
}

QByteArray Subtitle::codec() const
{
    return d->codec;
}

void Subtitle::setEngines(const QStringList& value)
{
    if (d->engine_names == value)
        return;

    // can not reset processor here, not thread safe.

    d->supported_suffixes.clear();
    d->engine_names = value;

    if (d->engine_names.isEmpty())
    {
        Q_EMIT enginesChanged();
        Q_EMIT supportedSuffixesChanged();

        return;
    }

    QList<SubtitleProcessor*> sps;

    Q_FOREACH (const QString& e, d->engine_names)
    {
        qCDebug(DIGIKAM_QTAV_LOG) << "engine:" << e;

        QList<SubtitleProcessor*>::iterator it = d->processors.begin();

        while (it != d->processors.end())
        {
            if (!(*it))
            {
                it = d->processors.erase(it);

                continue;
            }

            if ((*it)->name() != e)
            {
                ++it;

                continue;
            }

            sps.append(*it);
            it = d->processors.erase(it);

            break;
        }

        if (it == d->processors.end())
        {
            SubtitleProcessor* const sp = SubtitleProcessor::create(e.toLatin1().constData());

            if (sp)
                sps.append(sp);
        }
    }

    // release the processors not wanted

    qDeleteAll(d->processors);
    d->processors = sps;

    if (sps.isEmpty())
    {
        Q_EMIT enginesChanged();
        Q_EMIT supportedSuffixesChanged();

        return;
    }

    Q_FOREACH (SubtitleProcessor* const sp, sps)
    {
        d->supported_suffixes.append(sp->supportedTypes());
    }

    d->supported_suffixes.removeDuplicates();

    // DO NOT set d->suffixes

    Q_EMIT enginesChanged();
    Q_EMIT supportedSuffixesChanged();

    // it's safe to reload
}

QStringList Subtitle::engines() const
{
    return d->engine_names;
}

QString Subtitle::engine() const
{
    if (!d->processor)
        return QString();

    return d->processor->name();
}

void Subtitle::setFuzzyMatch(bool value)
{
    if (d->fuzzy_match == value)
        return;

    d->fuzzy_match = value;

    Q_EMIT fuzzyMatchChanged();
}

bool Subtitle::fuzzyMatch() const
{
    return d->fuzzy_match;
}

void Subtitle::setRawData(const QByteArray& data)
{
    // compare the whole content is not a good idea

    if (d->raw_data.size() == data.size())
        return;

    d->raw_data = data;

    Q_EMIT rawDataChanged();

    d->url.clear();
    d->file_name.clear();
}

QByteArray Subtitle::rawData() const
{
    return d->raw_data;
}

extern QString getLocalPath(const QString& fullPath);

void Subtitle::setFileName(const QString &name)
{
    if (d->file_name == name)
        return;

    d->url.clear();
    d->raw_data.clear();
    d->file_name = name;

    if (d->file_name.startsWith(QLatin1String("file:")))
        d->file_name = getLocalPath(d->file_name);

    Q_EMIT fileNameChanged();
}

QString Subtitle::fileName() const
{
    return d->file_name;
}

void Subtitle::setDirs(const QStringList& value)
{
    if (d->dirs == value)
        return;

    d->dirs = value;

    Q_EMIT dirsChanged();
}

QStringList Subtitle::dirs() const
{
    return d->dirs;
}

QStringList Subtitle::supportedSuffixes() const
{
    return d->supported_suffixes;
}

void Subtitle::setSuffixes(const QStringList& value)
{
    if (d->suffixes == value)
        return;

    d->suffixes = value;

    Q_EMIT suffixesChanged();
}

QStringList Subtitle::suffixes() const
{
/*
    if (d->suffixes.isEmpty())
        return supportedSuffixes();
*/
    return d->suffixes;
}

void Subtitle::setTimestamp(qreal t)
{
    // TODO: detect image change?

    {
        QMutexLocker lock(&d->mutex);
        Q_UNUSED(lock);
        d->t = t;

        if (!isLoaded())
            return;

        if (!d->prepareCurrentFrame())
            return;

        d->update_text  = true;
        d->update_image = true;
    }

    Q_EMIT contentChanged();
}

qreal Subtitle::timestamp() const
{
    return d->t;
}

void Subtitle::setDelay(qreal value)
{
    if (d->delay == value)
        return;

    d->delay = value;

    Q_EMIT delayChanged();
}

qreal Subtitle::delay() const
{
    return d->delay;
}

QString Subtitle::fontFile() const
{
    return d->font_file;
}

void Subtitle::setFontFile(const QString& value)
{
    if (d->font_file == value)
        return;

    d->font_file = value;

    Q_EMIT fontFileChanged();

    if (d->processor)
    {
        d->processor->setFontFile(value);
    }
}

QString Subtitle::fontsDir() const
{
    return d->fonts_dir;
}

void Subtitle::setFontsDir(const QString& value)
{
    if (d->fonts_dir == value)
        return;

    d->fonts_dir = value;

    Q_EMIT fontsDirChanged();

    if (d->processor)
    {
        d->processor->setFontsDir(value);
    }
}

bool Subtitle::isFontFileForced() const
{
    return d->force_font_file;
}

void Subtitle::setFontFileForced(bool value)
{
    if (d->force_font_file == value)
        return;

    d->force_font_file = value;

    Q_EMIT fontFileForcedChanged();

    if (d->processor)
    {
        d->processor->setFontFileForced(value);
    }
}

void Subtitle::load()
{
    SubtitleProcessor* const old_processor = d->processor;
    d->reset();

    Q_EMIT contentChanged(); // notify user to update subtitle

    // lock is not needed because it's not loaded now

    if (!d->url.isEmpty())
    {
        // need qt network module network

        if (old_processor != d->processor)
            Q_EMIT engineChanged();

        return;
    }

    // raw data is set, file name and url are empty

    QByteArray u8 = d->raw_data;

    if (!u8.isEmpty())
    {
        d->loaded = d->processRawData(u8);

        if (d->loaded)
            Q_EMIT loaded();

        checkCapability();

        if (old_processor != d->processor)
            Q_EMIT engineChanged();

        return;
    }

    // read from a url

    QFile f(QUrl::fromPercentEncoding(d->url.toEncoded()));

    if (f.exists())
    {
        u8 = d->readFromFile(f.fileName());

        if (u8.isEmpty())
            return;

        d->loaded = d->processRawData(u8);

        if (d->loaded)
            Q_EMIT loaded(QUrl::fromPercentEncoding(d->url.toEncoded()));

        checkCapability();

        if (old_processor != d->processor)
            Q_EMIT engineChanged();

        return;
    }

    // read from a file

    QStringList paths = d->find();

    if (paths.isEmpty())
    {
        checkCapability();

        if (old_processor != d->processor)
            Q_EMIT engineChanged();

        return;
    }

    Q_FOREACH (const QString& path, paths)
    {
        if (path.isEmpty())
            continue;

        u8 = d->readFromFile(path);

        if (u8.isEmpty())
            continue;

        if (!d->processRawData(u8))
            continue;

        d->loaded = true;

        Q_EMIT loaded(path);

        break;
    }

    checkCapability();

    if (old_processor != d->processor)
        Q_EMIT engineChanged();

    if (d->processor)
    {
        d->processor->setFontFile(d->font_file);
        d->processor->setFontsDir(d->fonts_dir);
        d->processor->setFontFileForced(d->force_font_file);
    }
}

void Subtitle::checkCapability()
{
    if (d->last_can_render == canRender())
        return;

    d->last_can_render = canRender();

    Q_EMIT canRenderChanged();
}

void Subtitle::loadAsync()
{
    if (fileName().isEmpty())
        return;

    class Q_DECL_HIDDEN Loader : public QRunnable
    {
    public:

        explicit Loader(Subtitle* const sub)
            : m_sub(sub)
        {
        }

        void run()
        {
            if (m_sub)
                m_sub->load();
        }

    private:

        Subtitle* m_sub = nullptr;

    private:

        Q_DISABLE_COPY(Loader);
    };

    QThreadPool::globalInstance()->start(new Loader(this));
}

bool Subtitle::canRender() const
{
    return (d->processor && d->processor->canRender());
}

QString Subtitle::getText() const
{
    QMutexLocker lock(&d->mutex);
    Q_UNUSED(lock);

    if (!isLoaded())
        return QString();

    if (!d->current_count)
        return QString();

    if (!d->update_text)
        return d->current_text;

    d->update_text                 = false;
    d->current_text.clear();
    const int count                   = qAbs(d->current_count);
    QList<SubtitleFrame>::iterator it = (d->current_count > 0) ? d->itf
                                                                  : d->itf + (d->current_count+1);

    for (int i = 0 ; i < count ; ++i)
    {
        d->current_text.append(it->text).append(QLatin1Char('\n'));
        ++it;
    }

    d->current_text = d->current_text.trimmed();

    return d->current_text;
}

QImage Subtitle::getImage(int width, int height, QRect* boundingRect)
{
    QMutexLocker lock(&d->mutex);
    Q_UNUSED(lock);

    if (!isLoaded())
        return QImage();

    if ((width == 0) || (height == 0))
        return QImage();

#if 0

    if (!d->current_count) // seems ok to use this code
        return QImage();

    // always render the image to support animations

    if (!d->update_image && (width == d->current_image.width()) && (height == d->current_image.height()))
        return d->current_image;

#endif

    d->update_image = false;

    if (!canRender())
        return QImage();

    d->processor->setFrameSize(width, height);

    // TODO: store bounding rect here and not in processor

    d->current_image = d->processor->getImage(d->t - d->delay, boundingRect);

    return d->current_image;
}

SubImageSet Subtitle::getSubImages(int width, int height, QRect* boundingRect)
{
    QMutexLocker lock(&d->mutex);
    Q_UNUSED(lock);

    if (!isLoaded())
        return SubImageSet();

    if ((width == 0) || (height == 0))
        return SubImageSet();

    d->update_image = false;

    if (!canRender())
        return SubImageSet();

    d->processor->setFrameSize(width, height);

    // TODO: store bounding rect here and not in processor

    d->current_ass = d->processor->getSubImages(d->t - d->delay, boundingRect);

    return d->current_ass;
}

bool Subtitle::processHeader(const QByteArray& codec, const QByteArray &data)
{
    qCDebug(DIGIKAM_QTAV_LOG) << "codec :" << codec;
    qCDebug(DIGIKAM_QTAV_LOG) << "header:" << data;

    SubtitleProcessor* const old_processor = d->processor;
    d->reset(); // reset for the new subtitle stream (internal)

    if (d->processors.isEmpty())
        return false;

    Q_FOREACH (SubtitleProcessor* const sp, d->processors)
    {
        if (sp->supportedTypes().contains(QLatin1String(codec)))
        {
            d->processor = sp;

            qCDebug(DIGIKAM_QTAV_LOG)
                << "current subtitle processor: " << sp->name();

            break;
        }
    }

    if (old_processor != d->processor)
        Q_EMIT engineChanged();

    if (!d->processor)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("No subtitle processor supports the codec '%s'",
                codec.constData());

        return false;
    }

    if (!d->processor->processHeader(codec, data))
        return false;

    d->loaded = true;

    d->processor->setFontFile(d->font_file);
    d->processor->setFontsDir(d->fonts_dir);
    d->processor->setFontFileForced(d->force_font_file);

    return true;
}

bool Subtitle::processLine(const QByteArray& data, qreal pts, qreal duration)
{
    if (!d->processor)
        return false;

    SubtitleFrame f = d->processor->processLine(data, pts, duration);

    if (!f.isValid())
        return false; // TODO: if seek to previous position, an invalid frame is returned.

    if (d->frames.isEmpty() || (d->frames.last() < f))
    {
        d->frames.append(f);
        d->itf = d->frames.begin();

        return true;
    }

    // usually add to the end. TODO: test

    QList<SubtitleFrame>::iterator it = d->frames.end();

    if (it != d->frames.begin())
        --it;

    while ((it != d->frames.begin()) && (f < (*it)))
    {
        --it;
    }

    if (it != d->frames.begin()) // found in middle, insert before next
        ++it;

    d->frames.insert(it, f);
    d->itf = it;

    return true;
}

// DO NOT set frame's image to reduce memory usage
// assume frame.text is already set
// check previous text if now no subtitle

bool Subtitle::Private::prepareCurrentFrame()
{
    if (frames.isEmpty())
        return false;

    QList<SubtitleFrame>::iterator it = itf;
    int found                         = 0;
    const int old_current_count       = current_count;
    const qreal t                     = this->t - delay;

    if (t < it->begin)
    {
        while (it != frames.begin())
        {
            --it;

            if (t > it->end)
            {
                if (found > 0)
                    break;

                // no subtitle at that time. check previous text

                if (old_current_count)
                {
                    current_count = 0;

                    return true;
                }

                return false;
            }

            if (t >= (*it).begin)
            {
                if (found == 0)
                    itf = it;

                found++;
            }
        }

        current_count = -found;

        if (found > 0)
        {
            frame = *it;

            return true;
        }

        // no subtitle at that time. it == begin(). check previous text

        if (old_current_count)
            return true;

        return false;
    }

    bool it_changed = false;

    while (it != frames.end())
    {
        if (t > it->end)
        {
            ++it;
            it_changed = true;

            continue;
        }

        if (t < it->begin)
        {
            if (found > 0)
                break;

            // no subtitle at that time. check previous text

            if (old_current_count)
            {
                current_count = 0;

                return true;
            }

            return false;
        }

        if (found == 0)
            itf = it;

        ++found;
        ++it;
    }

    current_count = found;

    if (found > 0)
        return (it_changed || (current_count != old_current_count));

    // no subtitle at that time, it == end(). check previous text

    if (old_current_count)
        return true;

    return false;
}

QStringList Subtitle::Private::find()
{
    if (file_name.isEmpty())
        return QStringList();

    // !fuzzyMatch: return the file

    if (!fuzzy_match)
    {
        return (QStringList() << file_name);
    }

    // found files will be sorted by extensions in sfx order

    QStringList sfx(suffixes);

    if (sfx.isEmpty())
        sfx = supported_suffixes;

    if (sfx.isEmpty())
        return QStringList() << file_name;

    QFileInfo fi(file_name);
    QString name      = fi.fileName();
    QString base_name = fi.completeBaseName(); // a.mp4=>a, video suffix has only 1 dot
    QStringList filters, filters_base;

    Q_FOREACH (const QString& suf, sfx)
    {
        filters.append(QString::fromUtf8("%1*.%2").arg(name).arg(suf));

        if (name != base_name)
            filters_base.append(QString::fromUtf8("%1*.%2").arg(base_name).arg(suf));
    }

    QStringList search_dirs(dirs);
    search_dirs.prepend(fi.absolutePath());
    QFileInfoList list;

    Q_FOREACH (const QString& d, search_dirs)
    {
        QDir dir(d);
/*
        qCDebug(DIGIKAM_QTAV_LOG) << "dir: " << dir;
*/
        QFileInfoList fis = dir.entryInfoList(filters, QDir::Files, QDir::Unsorted);

        if (fis.isEmpty())
        {
            if (filters_base.isEmpty())
                continue;

            fis = dir.entryInfoList(filters_base, QDir::Files, QDir::Unsorted);
        }

        if (fis.isEmpty())
            continue;

        list.append(fis);
    }

    if (list.isEmpty())
        return QStringList();

    // TODO: sort. get entryList from nameFilters 1 by 1 is slower?

    QStringList sorted;

    // sfx is not empty, sort to the given order (sfx's order)

    Q_FOREACH (const QString& suf, sfx)
    {
        if (list.isEmpty())
            break;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)

        auto rx = QRegularExpression::fromWildcard(QString::fromLatin1("*.%1").arg(suf));

#else

        QRegExp rx(QLatin1String("*.") + suf);
        rx.setPatternSyntax(QRegExp::Wildcard);

#endif

        QFileInfoList::iterator it = list.begin();

        while (it != list.end())
        {
            if (!it->fileName().startsWith(name) && !it->fileName().startsWith(base_name))
            {
                // why it happens?

                it = list.erase(it);

                continue;
            }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)

            if (!rx.match(it->fileName()).hasMatch())
            {

#else

            if (!rx.exactMatch(it->fileName()))
            {

#endif
                ++it;

                continue;
            }

            sorted.append(it->absoluteFilePath());
            it = list.erase(it);
        }
    }

    // the given name is at the highest priority.

    if (QFile(file_name).exists())
    {
        sorted.removeAll(file_name);
        sorted.prepend(file_name);
    }

    qCDebug(DIGIKAM_QTAV_LOG) << "subtitles found: " << sorted;

    return sorted;
}

QByteArray Subtitle::Private::readFromFile(const QString &path)
{
    qCDebug(DIGIKAM_QTAV_LOG) << "read subtitle from: " << path;
    QFile f(path);

    if (f.size() > kMaxSubtitleSize)
        return QByteArray();

    if (!f.open(QIODevice::ReadOnly))
    {
        qCDebug(DIGIKAM_QTAV_LOG) << "Failed to open subtitle ["
                                  << path << "]: " << f.errorString();

        return QByteArray();
    }

    QTextStream ts(&f);
    ts.setAutoDetectUnicode(true);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)

    if (!codec.isEmpty())
    {
        if      (codec.toLower() == "system")
        {
            ts.setCodec(QTextCodec::codecForLocale());
        }
        else if (codec.toLower() == "autodetect")
        {
            CharsetDetector det;

            if (det.isAvailable())
            {
                QByteArray charset = det.detect(f.readAll());

                qCDebug(DIGIKAM_QTAV_LOG).noquote()
                    << QString::asprintf("charset: %s", charset.constData());

                f.seek(0);

                if (!charset.isEmpty())
                    ts.setCodec(QTextCodec::codecForName(charset));
            }
        }
        else
        {
            ts.setCodec(QTextCodec::codecForName(codec));
        }
    }

#endif

    return ts.readAll().toUtf8();
}

bool Subtitle::Private::processRawData(const QByteArray &data)
{
    processor = nullptr;
    frames.clear();

    if (data.size() > kMaxSubtitleSize)
        return false;

    Q_FOREACH (SubtitleProcessor* const sp, processors)
    {
        if (processRawData(sp, data))
        {
            processor = sp;

            break;
        }
    }

    if (!processor)
        return false;

    QList<SubtitleFrame> fs(processor->frames());

    if (fs.isEmpty())
        return false;

    std::sort(fs.begin(), fs.end());

    Q_FOREACH (const SubtitleFrame& f, fs)
    {
       frames.push_back(f);
    }

    itf   = frames.begin();
    frame = *itf;

    return true;
}

bool Subtitle::Private::processRawData(SubtitleProcessor* sp, const QByteArray& data)
{
    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("processing subtitle from raw data...");

    QByteArray u8(data);
    QBuffer buf(&u8);

    if (buf.open(QIODevice::ReadOnly))
    {
        const bool ok = sp->process(&buf);

        if (buf.isOpen())
            buf.close();

        if (ok)
            return true;
    }
    else
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN)
            << "open subtitle qbuffer error: " << buf.errorString();
    }

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("processing subtitle from a tmp utf8 file...");

    QString name = QUrl::fromPercentEncoding(url.toEncoded()).section(ushort('/'), -1);

    if (name.isEmpty())
        name = QFileInfo(file_name).fileName(); //d->name.section('/', -1); // if no separator ?

    if (name.isEmpty())
        name = QLatin1String("QtAV_u8_sub_cache");

    name.append(QString::fromUtf8("_%1").arg((quintptr)this));
    QFile w(QDir::temp().absoluteFilePath(name));

    if (w.open(QIODevice::WriteOnly))
    {
        w.write(data);
        w.close();
    }
    else
    {
        if (!w.exists())
            return false;
    }

    return sp->process(w.fileName());
}

SubtitleAPIProxy::SubtitleAPIProxy(QObject* const obj)
    : m_obj(obj),
      m_s  (nullptr)
{
}

void SubtitleAPIProxy::setSubtitle(Subtitle* const sub)
{
    m_s = sub;

    QObject::connect(m_s, SIGNAL(canRenderChanged()),
                     m_obj, SIGNAL(canRenderChanged()));

    QObject::connect(m_s, SIGNAL(contentChanged()),
                     m_obj, SIGNAL(contentChanged()));

    QObject::connect(m_s, SIGNAL(loaded(QString)),
                     m_obj, SIGNAL(loaded(QString)));

    QObject::connect(m_s, SIGNAL(codecChanged()),
                     m_obj, SIGNAL(codecChanged()));

    QObject::connect(m_s, SIGNAL(enginesChanged()),
                     m_obj, SIGNAL(enginesChanged()));

    QObject::connect(m_s, SIGNAL(engineChanged()),
                     m_obj, SIGNAL(engineChanged()));
/*
    QObject::connect(m_s, SIGNAL(fileNameChanged()),
                     m_obj, SIGNAL(fileNameChanged()));
*/
    QObject::connect(m_s, SIGNAL(dirsChanged()),
                     m_obj, SIGNAL(dirsChanged()));

    QObject::connect(m_s, SIGNAL(fuzzyMatchChanged()),
                     m_obj, SIGNAL(fuzzyMatchChanged()));

    QObject::connect(m_s, SIGNAL(suffixesChanged()),
                     m_obj, SIGNAL(suffixesChanged()));

    QObject::connect(m_s, SIGNAL(supportedSuffixesChanged()),
                     m_obj, SIGNAL(supportedSuffixesChanged()));

    QObject::connect(m_s, SIGNAL(delayChanged()),
                     m_obj, SIGNAL(delayChanged()));

    QObject::connect(m_s, SIGNAL(fontFileChanged()),
                     m_obj, SIGNAL(fontFileChanged()));

    QObject::connect(m_s, SIGNAL(fontsDirChanged()),
                     m_obj, SIGNAL(fontsDirChanged()));

    QObject::connect(m_s, SIGNAL(fontFileForcedChanged()),
                     m_obj, SIGNAL(fontFileForcedChanged()));
}

void SubtitleAPIProxy::setCodec(const QByteArray& value)
{
    if (!m_s)
        return;

    m_s->setCodec(value);
}

QByteArray SubtitleAPIProxy::codec() const
{
    if (!m_s)
        return QByteArray();

    return m_s->codec();
}

bool SubtitleAPIProxy::isLoaded() const
{
    return (m_s && m_s->isLoaded());
}

void SubtitleAPIProxy::setEngines(const QStringList& value)
{
    if (!m_s)
        return;

    m_s->setEngines(value);
}

QStringList SubtitleAPIProxy::engines() const
{
    if (!m_s)
        return QStringList();

    return m_s->engines();
}
QString SubtitleAPIProxy::engine() const
{
    if (!m_s)
        return QString();

    return m_s->engine();
}

void SubtitleAPIProxy::setFuzzyMatch(bool value)
{
    if (!m_s)
        return;

    m_s->setFuzzyMatch(value);
}

bool SubtitleAPIProxy::fuzzyMatch() const
{
    return (m_s && m_s->fuzzyMatch());
}

#if 0

void SubtitleAPIProxy::setFileName(const QString& name)
{
    if (!m_s)
        return;

    m_s->setFileName(name);
}

QString SubtitleAPIProxy::fileName() const
{
    if (!m_s)
        return QString();

    return m_s->fileName();
}

#endif

void SubtitleAPIProxy::setDirs(const QStringList& value)
{
    if (!m_s)
        return;

    m_s->setDirs(value);
}

QStringList SubtitleAPIProxy::dirs() const
{
    if (!m_s)
        return QStringList();

    return m_s->dirs();
}

QStringList SubtitleAPIProxy::supportedSuffixes() const
{
    if (!m_s)
        return QStringList();

    return m_s->supportedSuffixes();
}

void SubtitleAPIProxy::setSuffixes(const QStringList& value)
{
    if (!m_s)
        return;

    m_s->setSuffixes(value);
}

QStringList SubtitleAPIProxy::suffixes() const
{
    if (!m_s)
        return QStringList();

    return m_s->suffixes();
}

bool SubtitleAPIProxy::canRender() const
{
    return (m_s && m_s->canRender());
}

void SubtitleAPIProxy::setDelay(qreal value)
{
    if (!m_s)
        return;

    m_s->setDelay(value);
}

qreal SubtitleAPIProxy::delay() const
{
    return (m_s ? m_s->delay() : 0);
}

QString SubtitleAPIProxy::fontFile() const
{
    return (m_s ? m_s->fontFile() : QString());
}

void SubtitleAPIProxy::setFontFile(const QString &value)
{
    if (!m_s)
        return;

    m_s->setFontFile(value);
}

QString SubtitleAPIProxy::fontsDir() const
{
    return (m_s ? m_s->fontsDir() : QString());
}

void SubtitleAPIProxy::setFontsDir(const QString& value)
{
    if (!m_s)
        return;

    m_s->setFontsDir(value);
}

bool SubtitleAPIProxy::isFontFileForced() const
{
    return (m_s && m_s->isFontFileForced());
}

void SubtitleAPIProxy::setFontFileForced(bool value)
{
    if (!m_s)
        return;

    m_s->setFontFileForced(value);
}

} // namespace QtAV
