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

#include "AVPlayerConfigMngr.h"

// Qt includes

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QMetaEnum>
#include <QStandardPaths>

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>

// Local includes

#include "digikam_debug.h"

namespace QtAV
{

class Q_DECL_HIDDEN AVPlayerConfigMngr::Data
{
public:

    Data()
    {
        is_loading = false;

        QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

        if (!QDir(appDataDir).exists())
        {
            if (!QDir().mkpath(appDataDir))
            {
                qCWarning(DIGIKAM_AVPLAYER_LOG) << "Failed to create appDataDir: " << appDataDir;
            }
        }
    }

    void save()
    {
        if (is_loading)
            return;

        KSharedConfig::Ptr config = KSharedConfig::openConfig();

        // ---

        KConfigGroup group1       = config->group(QLatin1String("MediaPlayer General"));
        group1.writeEntry(QLatin1String("last_file"),         last_file);
        group1.writeEntry(QLatin1String("timeout"),           timeout);
        group1.writeEntry(QLatin1String("abort_timeout"),     abort_timeout);
        group1.writeEntry(QLatin1String("force_fps"),         force_fps);

        // ---

        KConfigGroup group2       = config->group(QLatin1String("MediaPlayer Decoder Video"));
        group2.writeEntry(QLatin1String("priority"),          video_decoders);
        group2.writeEntry(QLatin1String("zeroCopy"),          zero_copy);

        // ---

        KConfigGroup group3       = config->group(QLatin1String("MediaPlayer Capture"));
        group3.writeEntry(QLatin1String("dir"),               capture_dir);
        group3.writeEntry(QLatin1String("format"),            capture_fmt);
        group3.writeEntry(QLatin1String("quality"),           capture_quality);

        // ---

        KConfigGroup group4       = config->group(QLatin1String("MediaPlayer Subtitle"));
        group4.writeEntry(QLatin1String("enabled"),           subtitle_enabled);
        group4.writeEntry(QLatin1String("autoLoad"),          subtitle_autoload);
        group4.writeEntry(QLatin1String("engines"),           subtitle_engines);
        group4.writeEntry(QLatin1String("delay"),             subtitle_delay);
        group4.writeEntry(QLatin1String("font"),              subtitle_font);
        group4.writeEntry(QLatin1String("color"),             subtitle_color);
        group4.writeEntry(QLatin1String("outline_color"),     subtitle_outline_color);
        group4.writeEntry(QLatin1String("outline"),           subtitle_outline);
        group4.writeEntry(QLatin1String("bottom margin"),     subtilte_bottom_margin);

        // ---

        KConfigGroup group5       = config->group(QLatin1String("MediaPlayer Ass"));
        group5.writeEntry(QLatin1String("font_file"),         ass_font_file);
        group5.writeEntry(QLatin1String("force_font_file"),   ass_force_font_file);
        group5.writeEntry(QLatin1String("fonts_dir"),         ass_fonts_dir);

        // ---

        KConfigGroup group6       = config->group(QLatin1String("MediaPlayer Preview"));
        group6.writeEntry(QLatin1String("enabled"),           preview_enabled);
        group6.writeEntry(QLatin1String("width"),             preview_w);
        group6.writeEntry(QLatin1String("height"),            preview_h);

        // ---

        KConfigGroup group7       = config->group(QLatin1String("MediaPlayer AVFormat"));
        group7.writeEntry(QLatin1String("enable"),            avformat_on);

        QString avioFlags = QLatin1String("0");

        if (direct)
            avioFlags = QLatin1String("direct");

        group7.writeEntry(QLatin1String("avioflags"),         avioFlags);
        group7.writeEntry(QLatin1String("probesize"),         probe_size);
        group7.writeEntry(QLatin1String("analyzeduration"),   analyze_duration);
        group7.writeEntry(QLatin1String("extra"),             avformat_extra);

        // ---

        KConfigGroup group8       = config->group(QLatin1String("MediaPlayer AVFilterVideo"));
        group8.writeEntry(QLatin1String("enable"),            avfilterVideo_on);
        group8.writeEntry(QLatin1String("options"),           avfilterVideo);

        // ---

        KConfigGroup group9       = config->group(QLatin1String("MediaPlayer AVFilterAudio"));
        group9.writeEntry(QLatin1String("enable"),            avfilterAudio_on);
        group9.writeEntry(QLatin1String("options"),           avfilterAudio);

        const char* glname        = AVPlayerConfigMngr::staticMetaObject.enumerator(
            AVPlayerConfigMngr::staticMetaObject.indexOfEnumerator("OpenGLType"))
            .valueToKey(opengl);

        // ---

        KConfigGroup group10      = config->group(QLatin1String("MediaPlayer OpenGL"));
        group10.writeEntry(QLatin1String("egl"),               egl);
        group10.writeEntry(QLatin1String("type"),              QString::fromLatin1(glname));
        group10.writeEntry(QLatin1String("angle_platform"),    angle_dx);

        // ---

        KConfigGroup group11      = config->group(QLatin1String("MediaPlayer Shader"));
        group11.writeEntry(QLatin1String("enable"),            user_shader);
        group11.writeEntry(QLatin1String("fbo"),               fbo);
        group11.writeEntry(QLatin1String("fragHeader"),        frag_header);
        group11.writeEntry(QLatin1String("fragSample"),        frag_sample);
        group11.writeEntry(QLatin1String("fragPostProcess"),   frag_pp);

        // ---

        KConfigGroup group12      = config->group(QLatin1String("MediaPlayer Buffer"));
        group12.writeEntry(QLatin1String("value"),             buffer_value);

        // ---

        config->sync();

        qCDebug(DIGIKAM_AVPLAYER_LOG) << "sync end";
    }

public:

    QString                         file;
    bool                            is_loading              = false;

    qreal                           force_fps               = 0.0;
    QStringList                     video_decoders;
    bool                            zero_copy               = false;

    QString                         last_file;

    QString                         capture_dir;
    QString                         capture_fmt;
    int                             capture_quality         = 0;

    bool                            avformat_on             = false;
    bool                            direct                  = false;
    unsigned int                    probe_size              = 0;
    int                             analyze_duration        = 0;
    QString                         avformat_extra;
    bool                            avfilterVideo_on        = false;
    QString                         avfilterVideo;
    bool                            avfilterAudio_on        = false;
    QString                         avfilterAudio;

    QStringList                     subtitle_engines;
    bool                            subtitle_autoload       = false;
    bool                            subtitle_enabled        = false;
    QFont                           subtitle_font;
    QColor                          subtitle_color;
    QColor                          subtitle_outline_color;
    bool                            subtitle_outline        = false;
    int                             subtilte_bottom_margin  = 0;
    qreal                           subtitle_delay          = 0.0;

    bool                            ass_force_font_file     = false;
    QString                         ass_font_file;
    QString                         ass_fonts_dir;

    bool                            preview_enabled         = false;
    int                             preview_w               = 0;
    int                             preview_h               = 0;

    bool                            egl                     = false;
    AVPlayerConfigMngr::OpenGLType  opengl                  = OpenGLType::Auto;
    QString                         angle_dx;
    bool                            abort_timeout           = false;
    qreal                           timeout                 = 0.0;
    int                             buffer_value            = 0;

    bool                            user_shader             = false;
    bool                            fbo                     = false;
    QString                         frag_header;
    QString                         frag_sample;
    QString                         frag_pp;
};

// -----------------------------------------------------------------------------------

void AVPlayerConfigMngr::reload()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    mpData->is_loading        = true;

    KConfigGroup group1       = config->group(QLatin1String("MediaPlayer General"));
    setLastFile(group1.readEntry(QLatin1String("last_file"),                   QString()));
    setTimeout(group1.readEntry(QLatin1String("timeout"),                      30.0));
    setAbortOnTimeout(group1.readEntry(QLatin1String("abort_timeout"),         true));
    setForceFrameRate(group1.readEntry(QLatin1String("force_fps"),             0.0));

    // ---

    KConfigGroup group2       = config->group(QLatin1String("MediaPlayer Decoder Video"));
    setDecoderPriorityNames(group2.readEntry(QLatin1String("priority"),        QStringList() << QLatin1String("FFmpeg")
                                                                                             << QLatin1String("HW FFmpeg")));  // HW is ignored);
    setZeroCopy(group2.readEntry(QLatin1String("zeroCopy"),                    true));

    // ---

    KConfigGroup group3       = config->group(QLatin1String("MediaPlayer Capture"));
    setCaptureDir(group3.readEntry(QLatin1String("dir"),                       QString()));

    if (captureDir().isEmpty())
    {
        setCaptureDir(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    }

    setCaptureFormat(group3.readEntry(QLatin1String("format"),                 QString::fromLatin1("png")));
    setCaptureQuality(group3.readEntry(QLatin1String("quality"),               100));

    // ---

    KConfigGroup group4       = config->group(QLatin1String("MediaPlayer Subtitle"));
    setSubtitleAutoLoad(group4.readEntry(QLatin1String("autoLoad"),            true));
    setSubtitleEnabled(group4.readEntry(QLatin1String("enabled"),              true));
    setSubtitleEngines(group4.readEntry(QLatin1String("engines"),              QStringList() << QLatin1String("FFmpeg") << QLatin1String("LibASS")));
    setSubtitleDelay(group4.readEntry(QLatin1String("delay"),                  0.0));
    QFont f;
    f.setPointSize(20);
    f.setBold(true);
    setSubtitleFont(group4.readEntry(QLatin1String("font"),                    f));
    setSubtitleColor(group4.readEntry(QLatin1String("color"),                  QColor("white")));
    setSubtitleOutlineColor(group4.readEntry(QLatin1String("outline_color"),   QColor("blue")));
    setSubtitleOutline(group4.readEntry(QLatin1String("outline"),              true));
    setSubtitleBottomMargin(group4.readEntry(QLatin1String("bottom margin"),   8));

    // ---

    KConfigGroup group5       = config->group(QLatin1String("MediaPlayer Ass"));
    setAssFontFile(group5.readEntry(QLatin1String("font_file"),                QString()));
    setAssFontFileForced(group5.readEntry(QLatin1String("force_font_file"),    false));
    setAssFontsDir(group5.readEntry(QLatin1String("fonts_dir"),                QString()));

    // ---

    KConfigGroup group6       = config->group(QLatin1String("MediaPlayer Preview"));
    setPreviewEnabled(group6.readEntry(QLatin1String("enabled"),               true));
    setPreviewWidth(group6.readEntry(QLatin1String("width"),                   160));
    setPreviewHeight(group6.readEntry(QLatin1String("height"),                 90));

    // ---

    KConfigGroup group7       = config->group(QLatin1String("MediaPlayer AVFormat"));
    setAvformatOptionsEnabled(group7.readEntry(QLatin1String("enable"),        false));
    const QString avioFlags   = group7.readEntry(QLatin1String("avioflags"),   QString::fromLatin1("0"));
    reduceBuffering(avioFlags == QLatin1String("direct"));
    probeSize(group7.readEntry(QLatin1String("probesize"),                     5000000));
    analyzeDuration(group7.readEntry(QLatin1String("analyzeduration"),         5000000));
    avformatExtra(group7.readEntry(QLatin1String("extra"),                     QString()));

    // ---

    KConfigGroup group8       = config->group(QLatin1String("MediaPlayer AVFilterVideo"));
    avfilterVideoEnable(group8.readEntry(QLatin1String("enable"),              true));
    avfilterVideoOptions(group8.readEntry(QLatin1String("options"),            QString()));

    // ---

    KConfigGroup group9       = config->group(QLatin1String("MediaPlayer AVFilterAudio"));
    avfilterAudioEnable(group9.readEntry(QLatin1String("enable"),              true));
    avfilterAudioOptions(group9.readEntry(QLatin1String("options"),            QString()));

    // ---

    KConfigGroup group10      = config->group(QLatin1String("MediaPlayer OpenGL"));
    setEGL(group10.readEntry(QLatin1String("egl"),                             false));
    const QString glname = group10.readEntry(QLatin1String("type"),            QString::fromLatin1("OpenGLES"));

    setOpenGLType((AVPlayerConfigMngr::OpenGLType)AVPlayerConfigMngr::staticMetaObject.enumerator(
        AVPlayerConfigMngr::staticMetaObject.indexOfEnumerator("OpenGLType"))
            .keysToValue(glname.toLatin1().constData()));

    // NOTE: d3d11 bad performance (gltexsubimage2d)

    setANGLEPlatform(group10.readEntry(QLatin1String("angle_platform"),        QString::fromLatin1("d3d9")));

    // ---

    KConfigGroup group11      = config->group(QLatin1String("MediaPlayer Shader"));
    setUserShaderEnabled(group11.readEntry(QLatin1String("enable"),            false));
    setIntermediateFBO(group11.readEntry(QLatin1String("fbo"),                 false));
    setFragHeader(group11.readEntry(QLatin1String("fragHeader"),               QString()));
    setFragSample(group11.readEntry(QLatin1String("fragSample"),               QString::fromLatin1("// horizontal mirror effect\n"
                                                                                                   "vec4 sample2d(sampler2D tex, vec2 pos, int p) {\n"
                                                                                                   "    return texture(tex, vec2(1.0-pos.x, pos.y));\n"
                                                                                                   "}")));
    setFragPostProcess(group11.readEntry(QLatin1String("fragPostProcess"),     QString::fromLatin1("//negate color effect\n"
                                                                                                   "gl_FragColor.rgb = vec3(1.0-gl_FragColor.r, 1.0-gl_FragColor.g, 1.0-gl_FragColor.b);")));

    // ---

    KConfigGroup group12      = config->group(QLatin1String("MediaPlayer Buffer"));
    setBufferValue(group11.readEntry(QLatin1String("value"),                   -1));

    mpData->is_loading        = false;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::instance()
{
    static AVPlayerConfigMngr cfg;

    return cfg;
}

AVPlayerConfigMngr::AVPlayerConfigMngr(QObject* const parent)
    : QObject(parent),
      mpData (new Data())
{
    // DO NOT call save() in dtor because it's a singleton and may be deleted later than qApp, QFont is not valid

    connect(qApp, SIGNAL(aboutToQuit()),
            this, SLOT(save()));            // FIXME: what if qapp not ready

    reload();
}

AVPlayerConfigMngr::~AVPlayerConfigMngr()
{
    delete mpData;
}

bool AVPlayerConfigMngr::reset()
{
    QFile cf(mpData->file);

    if (!cf.remove())
    {
        qCWarning(DIGIKAM_AVPLAYER_LOG) << "Failed to remove config file: " << cf.errorString();

        return false;
    }

    reload();
    save();

    return true;
}

qreal AVPlayerConfigMngr::forceFrameRate() const
{
    return mpData->force_fps;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setForceFrameRate(qreal value)
{
    if (mpData->force_fps == value)
        return *this;

    mpData->force_fps = value;

    Q_EMIT forceFrameRateChanged();
    Q_EMIT changed();

    return *this;
}

QStringList AVPlayerConfigMngr::decoderPriorityNames() const
{
    return mpData->video_decoders;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setDecoderPriorityNames(const QStringList& value)
{
    if (mpData->video_decoders == value)
    {
        qCDebug(DIGIKAM_AVPLAYER_LOG).noquote() << QString::asprintf("decoderPriority not changed");

        return *this;
    }

    mpData->video_decoders = value;

    Q_EMIT decoderPriorityNamesChanged();
    Q_EMIT changed();

    mpData->save();

    return *this;
}

bool AVPlayerConfigMngr::zeroCopy() const
{
    return mpData->zero_copy;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setZeroCopy(bool value)
{
    if (mpData->zero_copy == value)
        return *this;

    mpData->zero_copy = value;

    Q_EMIT zeroCopyChanged();
    Q_EMIT changed();

    mpData->save();

    return *this;
}

QString AVPlayerConfigMngr::captureDir() const
{
    return mpData->capture_dir;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setCaptureDir(const QString& dir)
{
    if (mpData->capture_dir == dir)
        return *this;

    mpData->capture_dir = dir;

    Q_EMIT captureDirChanged(dir);
    Q_EMIT changed();

    return *this;
}

QString AVPlayerConfigMngr::captureFormat() const
{
    return mpData->capture_fmt;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setCaptureFormat(const QString& format)
{
    if (mpData->capture_fmt == format)
        return *this;

    mpData->capture_fmt = format;

    Q_EMIT captureFormatChanged(format);
    Q_EMIT changed();

    return *this;
}

// only works for non-yuv capture

int AVPlayerConfigMngr::captureQuality() const
{
    return mpData->capture_quality;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setCaptureQuality(int quality)
{
    if (mpData->capture_quality == quality)
        return *this;

    mpData->capture_quality = quality;

    Q_EMIT captureQualityChanged(quality);
    Q_EMIT changed();

    return *this;
}

QStringList AVPlayerConfigMngr::subtitleEngines() const
{
    return mpData->subtitle_engines;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setSubtitleEngines(const QStringList& value)
{
    if (mpData->subtitle_engines == value)
        return *this;

    mpData->subtitle_engines = value;

    Q_EMIT subtitleEnginesChanged();
    Q_EMIT changed();

    return *this;
}

bool AVPlayerConfigMngr::subtitleAutoLoad() const
{
    return mpData->subtitle_autoload;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setSubtitleAutoLoad(bool value)
{
    if (mpData->subtitle_autoload == value)
        return *this;

    mpData->subtitle_autoload = value;

    Q_EMIT subtitleAutoLoadChanged();
    Q_EMIT changed();

    return *this;
}

bool AVPlayerConfigMngr::subtitleEnabled() const
{
    return mpData->subtitle_enabled;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setSubtitleEnabled(bool value)
{
    if (mpData->subtitle_enabled == value)
        return *this;

    mpData->subtitle_enabled = value;

    Q_EMIT subtitleEnabledChanged();
    Q_EMIT changed();

    return *this;
}

QFont AVPlayerConfigMngr::subtitleFont() const
{
    return mpData->subtitle_font;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setSubtitleFont(const QFont& value)
{
    if (mpData->subtitle_font == value)
        return *this;

    mpData->subtitle_font = value;

    Q_EMIT subtitleFontChanged();
    Q_EMIT changed();

    return *this;
}

bool AVPlayerConfigMngr::subtitleOutline() const
{
    return mpData->subtitle_outline;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setSubtitleOutline(bool value)
{
    if (mpData->subtitle_outline == value)
        return *this;

    mpData->subtitle_outline = value;

    Q_EMIT subtitleOutlineChanged();
    Q_EMIT changed();

    return *this;
}

QColor AVPlayerConfigMngr::subtitleColor() const
{
    return mpData->subtitle_color;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setSubtitleColor(const QColor& value)
{
    if (mpData->subtitle_color == value)
        return *this;

    mpData->subtitle_color = value;

    Q_EMIT subtitleColorChanged();
    Q_EMIT changed();

    return *this;
}

QColor AVPlayerConfigMngr::subtitleOutlineColor() const
{
    return mpData->subtitle_outline_color;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setSubtitleOutlineColor(const QColor& value)
{
    if (mpData->subtitle_outline_color == value)
        return *this;

    mpData->subtitle_outline_color = value;

    Q_EMIT subtitleOutlineColorChanged();
    Q_EMIT changed();

    return *this;
}

int AVPlayerConfigMngr::subtitleBottomMargin() const
{
    return mpData->subtilte_bottom_margin;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setSubtitleBottomMargin(int value)
{
    if (mpData->subtilte_bottom_margin == value)
        return *this;

    mpData->subtilte_bottom_margin = value;

    Q_EMIT subtitleBottomMarginChanged();
    Q_EMIT changed();

    return *this;
}

qreal AVPlayerConfigMngr::subtitleDelay() const
{
    return mpData->subtitle_delay;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setSubtitleDelay(qreal value)
{
    if (mpData->subtitle_delay == value)
        return *this;

    mpData->subtitle_delay = value;

    Q_EMIT subtitleDelayChanged();
    Q_EMIT changed();

    return *this;
}

QString AVPlayerConfigMngr::assFontFile() const
{
    return mpData->ass_font_file;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setAssFontFile(const QString& value)
{
    if (mpData->ass_font_file == value)
        return *this;

    mpData->ass_font_file = value;

    Q_EMIT assFontFileChanged();
    Q_EMIT changed();

    return *this;
}


QString AVPlayerConfigMngr::assFontsDir() const
{
    return mpData->ass_fonts_dir;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setAssFontsDir(const QString& value)
{
    if (mpData->ass_fonts_dir == value)
        return *this;

    mpData->ass_fonts_dir = value;

    Q_EMIT assFontsDirChanged();
    Q_EMIT changed();

    return *this;
}

bool AVPlayerConfigMngr::isAssFontFileForced() const
{
    return mpData->ass_force_font_file;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setAssFontFileForced(bool value)
{
    if (mpData->ass_force_font_file == value)
        return *this;

    mpData->ass_force_font_file = value;

    Q_EMIT assFontFileForcedChanged();
    Q_EMIT changed();

    return *this;
}

bool AVPlayerConfigMngr::previewEnabled() const
{
    return mpData->preview_enabled;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setPreviewEnabled(bool value)
{
    if (mpData->preview_enabled == value)
        return *this;

    mpData->preview_enabled = value;

    Q_EMIT previewEnabledChanged();
    Q_EMIT changed();

    return *this;
}

int AVPlayerConfigMngr::previewWidth() const
{
    return mpData->preview_w;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setPreviewWidth(int value)
{
    if (mpData->preview_w == value)
        return *this;

    mpData->preview_w = value;

    Q_EMIT previewWidthChanged();
    Q_EMIT changed();

    return *this;
}

int AVPlayerConfigMngr::previewHeight() const
{
    return mpData->preview_h;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setPreviewHeight(int value)
{
    if (mpData->preview_h == value)
        return *this;

    mpData->preview_h = value;

    Q_EMIT previewHeightChanged();
    Q_EMIT changed();

    return *this;
}

QVariantHash AVPlayerConfigMngr::avformatOptions() const
{
    QVariantHash vh;

    if (!mpData->avformat_extra.isEmpty())
    {
        QStringList s(mpData->avformat_extra.split(QString::fromLatin1(" ")));

        for (int i = 0 ; i < s.size() ; ++i)
        {
            int eq = s[i].indexOf(QLatin1String("="));

            if (eq < 0)
            {
                continue;
            }
            else
            {
                vh[s[i].mid(0, eq)] = s[i].mid(eq+1);
            }
        }
    }

    if (mpData->probe_size > 0)
    {
        vh[QString::fromLatin1("probesize")] = mpData->probe_size;
    }

    if (mpData->analyze_duration)
    {
        vh[QString::fromLatin1("analyzeduration")] = mpData->analyze_duration;
    }

    if (mpData->direct)
    {
        vh[QString::fromLatin1("avioflags")] = QString::fromLatin1("direct");
    };

    return vh;
}

bool AVPlayerConfigMngr::avformatOptionsEnabled() const
{
    return mpData->avformat_on;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setAvformatOptionsEnabled(bool value)
{
    if (mpData->avformat_on == value)
        return *this;

    mpData->avformat_on = value;

    Q_EMIT avformatOptionsEnabledChanged();
    Q_EMIT changed();

    return *this;
}

unsigned int AVPlayerConfigMngr::probeSize() const
{
    return mpData->probe_size;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::probeSize(unsigned int ps)
{
    mpData->probe_size = ps;

    return *this;
}

int AVPlayerConfigMngr::analyzeDuration() const
{
    return mpData->analyze_duration;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::analyzeDuration(int ad)
{
    mpData->analyze_duration = ad;

    return *this;
}

bool AVPlayerConfigMngr::reduceBuffering() const
{
    return mpData->direct;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::reduceBuffering(bool y)
{
    mpData->direct = y;

    return *this;
}

QString AVPlayerConfigMngr::avformatExtra() const
{
    return mpData->avformat_extra;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::avformatExtra(const QString& text)
{
    mpData->avformat_extra = text;

    return *this;
}

QString AVPlayerConfigMngr::avfilterVideoOptions() const
{
    return mpData->avfilterVideo;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::avfilterVideoOptions(const QString& options)
{
    if (mpData->avfilterVideo == options)
        return *this;

    mpData->avfilterVideo = options;

    Q_EMIT avfilterVideoChanged();
    Q_EMIT changed();

    return *this;
}

bool AVPlayerConfigMngr::avfilterVideoEnable() const
{
    return mpData->avfilterVideo_on;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::avfilterVideoEnable(bool e)
{
    if (mpData->avfilterVideo_on == e)
        return *this;

    mpData->avfilterVideo_on = e;

    Q_EMIT avfilterVideoChanged();
    Q_EMIT changed();

    return *this;
}

QString AVPlayerConfigMngr::avfilterAudioOptions() const
{
    return mpData->avfilterAudio;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::avfilterAudioOptions(const QString& options)
{
    if (mpData->avfilterAudio == options)
        return *this;

    mpData->avfilterAudio = options;

    Q_EMIT avfilterAudioChanged();
    Q_EMIT changed();

    return *this;
}

bool AVPlayerConfigMngr::avfilterAudioEnable() const
{
    return mpData->avfilterAudio_on;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::avfilterAudioEnable(bool e)
{
    if (mpData->avfilterAudio_on == e)
        return *this;

    mpData->avfilterAudio_on = e;

    Q_EMIT avfilterAudioChanged();
    Q_EMIT changed();

    return *this;
}

bool AVPlayerConfigMngr::isEGL() const
{
    return mpData->egl;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setEGL(bool value)
{
    if (mpData->egl == value)
        return *this;

    mpData->egl = value;

    Q_EMIT EGLChanged();
    Q_EMIT changed();

    return *this;
}

AVPlayerConfigMngr::OpenGLType AVPlayerConfigMngr::openGLType() const
{
    return mpData->opengl;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setOpenGLType(OpenGLType value)
{
    if (mpData->opengl == value)
        return *this;

    mpData->opengl = value;

    Q_EMIT openGLTypeChanged();
    Q_EMIT changed();

    return *this;
}

QString AVPlayerConfigMngr::getANGLEPlatform() const
{
    return mpData->angle_dx;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setANGLEPlatform(const QString& value)
{
    if (mpData->angle_dx == value)
        return *this;

    mpData->angle_dx = value;

    Q_EMIT ANGLEPlatformChanged();
    Q_EMIT changed();

    return *this;
}

bool AVPlayerConfigMngr::userShaderEnabled() const
{
    return mpData->user_shader;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setUserShaderEnabled(bool value)
{
    if (mpData->user_shader == value)
        return *this;

    mpData->user_shader = value;

    Q_EMIT userShaderEnabledChanged();
    Q_EMIT changed();

    return *this;
}

bool AVPlayerConfigMngr::intermediateFBO() const
{
    return mpData->fbo;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setIntermediateFBO(bool value)
{
    if (mpData->fbo == value)
        return *this;

    mpData->fbo = value;

    Q_EMIT intermediateFBOChanged();
    Q_EMIT changed();

    return *this;
}

QString AVPlayerConfigMngr::fragHeader() const
{
    return mpData->frag_header;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setFragHeader(const QString& text)
{
    if (mpData->frag_header == text)
        return *this;

    mpData->frag_header = text;

    Q_EMIT fragHeaderChanged();
    Q_EMIT changed();

    return *this;
}

QString AVPlayerConfigMngr::fragSample() const
{
    return mpData->frag_sample;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setFragSample(const QString& text)
{
    if (mpData->frag_sample == text)
        return *this;

    mpData->frag_sample = text;

    Q_EMIT fragSampleChanged();
    Q_EMIT changed();

    return *this;
}

QString AVPlayerConfigMngr::fragPostProcess() const
{
    return mpData->frag_pp;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setFragPostProcess(const QString& text)
{
    if (mpData->frag_pp == text)
        return *this;

    mpData->frag_pp = text;

    Q_EMIT fragPostProcessChanged();
    Q_EMIT changed();

    return *this;
}

int AVPlayerConfigMngr::bufferValue() const
{
    return mpData->buffer_value;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setBufferValue(int value)
{
    if (mpData->buffer_value == value)
        return *this;

    mpData->buffer_value = value;

    Q_EMIT bufferValueChanged();
    Q_EMIT changed();

    return *this;
}

qreal AVPlayerConfigMngr::timeout() const
{
    return mpData->timeout;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setTimeout(qreal value)
{
    if (mpData->timeout == value)
        return *this;

    mpData->timeout = value;

    Q_EMIT timeoutChanged();
    Q_EMIT changed();

    return *this;
}

bool AVPlayerConfigMngr::abortOnTimeout() const
{
    return mpData->abort_timeout;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setAbortOnTimeout(bool value)
{
    if (mpData->abort_timeout == value)
        return *this;

    mpData->abort_timeout = value;

    Q_EMIT abortOnTimeoutChanged();
    Q_EMIT changed();

    return *this;
}

QString AVPlayerConfigMngr::lastFile() const
{
    return mpData->last_file;
}

AVPlayerConfigMngr& AVPlayerConfigMngr::setLastFile(const QString &value)
{
    if (mpData->last_file == value)
        return *this;

    mpData->last_file = value;

    Q_EMIT lastFileChanged();
    Q_EMIT changed();

    return *this;
}

void AVPlayerConfigMngr::save()
{
    mpData->save();
}

} // namespace QtAV
