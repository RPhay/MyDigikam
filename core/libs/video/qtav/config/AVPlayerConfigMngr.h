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

#ifndef QTAV_WIDGETS_AVPLAYER_CONFIG_MNGR_H
#define QTAV_WIDGETS_AVPLAYER_CONFIG_MNGR_H

// Qt includes

#include <QObject>
#include <QStringList>
#include <QVariant>
#include <QColor>
#include <QFont>

// Local includes

#include "QtAVWidgets_Global.h"

namespace QtAV
{

// TODO: use hash to simplify api

/**
 * MVC model. Q_SIGNALS from AVPlayerConfigMngr notify ui update.
 * Signals from ui does not change AVPlayerConfigMngr unless ui changes applyed by XXXPage.apply()
 * Signals from ui will emit AVPlayerConfigMngr::xxxChanged() with
 * the value in ui. ui cancel the change also emit it with the value stores in AVPlayerConfigMngr.
 * apply() will change the value in AVPlayerConfigMngr
 */

class DIGIKAM_EXPORT AVPlayerConfigMngr : public QObject
{
    Q_OBJECT

    // last file opened by file dialog

    Q_PROPERTY(QString lastFile READ lastFile WRITE setLastFile NOTIFY lastFileChanged)
    Q_PROPERTY(qreal forceFrameRate READ forceFrameRate WRITE setForceFrameRate NOTIFY forceFrameRateChanged)
    Q_PROPERTY(QStringList decoderPriorityNames READ decoderPriorityNames WRITE setDecoderPriorityNames NOTIFY decoderPriorityNamesChanged)
    Q_PROPERTY(bool zeroCopy READ zeroCopy WRITE setZeroCopy NOTIFY zeroCopyChanged)
    Q_PROPERTY(QString captureDir READ captureDir WRITE setCaptureDir NOTIFY captureDirChanged)
    Q_PROPERTY(QString captureFormat READ captureFormat WRITE setCaptureFormat NOTIFY captureFormatChanged)
    Q_PROPERTY(int captureQuality READ captureQuality WRITE setCaptureQuality NOTIFY captureQualityChanged)
    Q_PROPERTY(QStringList subtitleEngines READ subtitleEngines WRITE setSubtitleEngines NOTIFY subtitleEnginesChanged)
    Q_PROPERTY(bool subtitleAutoLoad READ subtitleAutoLoad WRITE setSubtitleAutoLoad NOTIFY subtitleAutoLoadChanged)
    Q_PROPERTY(bool subtitleEnabled READ subtitleEnabled WRITE setSubtitleEnabled NOTIFY subtitleEnabledChanged)
    Q_PROPERTY(QFont subtitleFont READ subtitleFont WRITE setSubtitleFont NOTIFY subtitleFontChanged)
    Q_PROPERTY(QColor subtitleColor READ subtitleColor WRITE setSubtitleColor NOTIFY subtitleColorChanged)
    Q_PROPERTY(QColor subtitleOutlineColor READ subtitleOutlineColor WRITE setSubtitleOutlineColor NOTIFY subtitleOutlineColorChanged)
    Q_PROPERTY(bool subtitleOutline READ subtitleOutline WRITE setSubtitleOutline NOTIFY subtitleOutlineChanged)
    Q_PROPERTY(int subtitleBottomMargin READ subtitleBottomMargin WRITE setSubtitleBottomMargin NOTIFY subtitleBottomMarginChanged)
    Q_PROPERTY(qreal subtitleDelay READ subtitleDelay WRITE setSubtitleDelay NOTIFY subtitleDelayChanged)

    // font properties for libass engine

    Q_PROPERTY(QString assFontFile READ assFontFile WRITE setAssFontFile NOTIFY assFontFileChanged)
    Q_PROPERTY(QString assFontsDir READ assFontsDir WRITE setAssFontsDir NOTIFY assFontsDirChanged)
    Q_PROPERTY(bool assFontFileForced READ isAssFontFileForced WRITE setAssFontFileForced NOTIFY assFontFileForcedChanged)

    Q_PROPERTY(bool previewEnabled READ previewEnabled WRITE setPreviewEnabled NOTIFY previewEnabledChanged)
    Q_PROPERTY(int previewWidth READ previewWidth WRITE setPreviewWidth NOTIFY previewWidthChanged)
    Q_PROPERTY(int previewHeight READ previewHeight WRITE setPreviewHeight NOTIFY previewHeightChanged)
    Q_PROPERTY(bool EGL READ isEGL WRITE setEGL NOTIFY EGLChanged)
    Q_PROPERTY(OpenGLType openGLType READ openGLType WRITE setOpenGLType NOTIFY openGLTypeChanged)
    Q_PROPERTY(QString ANGLEPlatform READ getANGLEPlatform WRITE setANGLEPlatform NOTIFY ANGLEPlatformChanged)
    Q_PROPERTY(bool avformatOptionsEnabled READ avformatOptionsEnabled WRITE setAvformatOptionsEnabled NOTIFY avformatOptionsEnabledChanged)
    Q_PROPERTY(qreal timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged)
    Q_PROPERTY(int bufferValue READ bufferValue WRITE setBufferValue NOTIFY bufferValueChanged)

    Q_PROPERTY(bool userShaderEnabled READ userShaderEnabled WRITE setUserShaderEnabled NOTIFY userShaderEnabledChanged)
    Q_PROPERTY(bool intermediateFBO READ intermediateFBO WRITE setIntermediateFBO NOTIFY intermediateFBOChanged)
    Q_PROPERTY(QString fragHeader READ fragHeader WRITE setFragHeader NOTIFY fragHeaderChanged)
    Q_PROPERTY(QString fragSample READ fragSample WRITE setFragSample NOTIFY fragSampleChanged)
    Q_PROPERTY(QString fragPostProcess READ fragPostProcess WRITE setFragPostProcess NOTIFY fragPostProcessChanged)

public:

    enum OpenGLType
    {
        // currently only for windows

        Auto,
        Desktop,
        OpenGLES,
        Software
    };
    Q_ENUM(OpenGLType)

public:

    static AVPlayerConfigMngr& instance();

public:

    Q_INVOKABLE bool reset();

    void reload();

    QString lastFile()                                  const;
    AVPlayerConfigMngr& setLastFile(const QString& value);

    qreal forceFrameRate()                              const;
    AVPlayerConfigMngr& setForceFrameRate(qreal value);

    // in priority order. the same order as displayed in ui

    QStringList decoderPriorityNames()                  const;
    AVPlayerConfigMngr& setDecoderPriorityNames(const QStringList& names);

    bool zeroCopy()                                     const;
    AVPlayerConfigMngr& setZeroCopy(bool value);

    QString captureDir()                                const;
    AVPlayerConfigMngr& setCaptureDir(const QString& dir);

    /*!
     * \brief captureFormat
     *  can be "yuv" to capture yuv image without convertion. the suffix is the yuv format, e.g. "yuv420p", "nv12"
     *  or can be "jpg", "png"
     * \return
     */
    QString captureFormat()                             const;
    AVPlayerConfigMngr& setCaptureFormat(const QString& format);

    // only works for non-yuv capture. value: -1~100, -1: default

    int captureQuality()                                const;
    AVPlayerConfigMngr& setCaptureQuality(int quality);

    QStringList subtitleEngines()                       const;
    AVPlayerConfigMngr& setSubtitleEngines(const QStringList& value);

    bool subtitleAutoLoad()                             const;
    AVPlayerConfigMngr& setSubtitleAutoLoad(bool value);

    bool subtitleEnabled()                              const;
    AVPlayerConfigMngr& setSubtitleEnabled(bool value);

    QFont subtitleFont()                                const;
    AVPlayerConfigMngr& setSubtitleFont(const QFont& value);

    bool subtitleOutline()                              const;
    AVPlayerConfigMngr& setSubtitleOutline(bool value);

    QColor subtitleColor()                              const;
    AVPlayerConfigMngr& setSubtitleColor(const QColor& value);

    QColor subtitleOutlineColor()                       const;
    AVPlayerConfigMngr& setSubtitleOutlineColor(const QColor& value);

    int subtitleBottomMargin()                          const;
    AVPlayerConfigMngr& setSubtitleBottomMargin(int value);

    qreal subtitleDelay()                               const;
    AVPlayerConfigMngr& setSubtitleDelay(qreal value);

    QString assFontFile()                               const;
    AVPlayerConfigMngr& setAssFontFile(const QString& value);

    QString assFontsDir()                               const;
    AVPlayerConfigMngr& setAssFontsDir(const QString& value);

    bool isAssFontFileForced()                          const;
    AVPlayerConfigMngr& setAssFontFileForced(bool value);

    bool previewEnabled()                               const;
    AVPlayerConfigMngr& setPreviewEnabled(bool value);

    int previewWidth()                                  const;
    AVPlayerConfigMngr& setPreviewWidth(int value);

    int previewHeight()                                 const;
    AVPlayerConfigMngr& setPreviewHeight(int value);

    QVariantHash avformatOptions()                      const;
    bool avformatOptionsEnabled()                       const;
    AVPlayerConfigMngr& setAvformatOptionsEnabled(bool value);

    int analyzeDuration()                               const;
    AVPlayerConfigMngr& analyzeDuration(int ad);

    unsigned int probeSize()                            const;
    AVPlayerConfigMngr& probeSize(unsigned int ps);

    bool reduceBuffering()                              const;
    AVPlayerConfigMngr& reduceBuffering(bool y);

    QString avformatExtra()                             const;
    AVPlayerConfigMngr& avformatExtra(const QString& text);

    QString avfilterVideoOptions()                      const;
    AVPlayerConfigMngr& avfilterVideoOptions(const QString& options);

    bool avfilterVideoEnable()                          const;
    AVPlayerConfigMngr& avfilterVideoEnable(bool e);

    QString avfilterAudioOptions()                      const;
    AVPlayerConfigMngr& avfilterAudioOptions(const QString& options);

    bool avfilterAudioEnable()                          const;
    AVPlayerConfigMngr& avfilterAudioEnable(bool e);

    // currently only for xcb

    bool isEGL()                                        const;
    AVPlayerConfigMngr& setEGL(bool value);

    // can be "Desktop", "OpenGLES", "Software"

    OpenGLType openGLType()                             const;
    AVPlayerConfigMngr& setOpenGLType(OpenGLType value);

    QString getANGLEPlatform()                          const;
    AVPlayerConfigMngr& setANGLEPlatform(const QString& value);

    // ms >0. default 30000ms
    qreal timeout()                                     const;

    AVPlayerConfigMngr& setTimeout(qreal value);

    bool abortOnTimeout()                               const;
    AVPlayerConfigMngr& setAbortOnTimeout(bool value);

    // < 0: auto

    int bufferValue()                                   const;
    AVPlayerConfigMngr& setBufferValue(int value);

    Q_INVOKABLE QVariant operator()(const QString& key) const;
    Q_INVOKABLE AVPlayerConfigMngr& operator()(const QString& key, const QVariant& value);

    AVPlayerConfigMngr& setUserShaderEnabled(bool value);
    bool userShaderEnabled()                            const;

    AVPlayerConfigMngr& setIntermediateFBO(bool value);
    bool intermediateFBO()                              const;

    AVPlayerConfigMngr& setFragHeader(const QString& text);
    QString fragHeader()                                const;

    AVPlayerConfigMngr& setFragSample(const QString& text);
    QString fragSample()                                const;

    AVPlayerConfigMngr& setFragPostProcess(const QString& text);
    QString fragPostProcess()                           const;

public:

    // keyword 'Q_SIGNALS' maybe protected. we need call the Q_SIGNALS in other classes. Q_SIGNAL is empty

    Q_SIGNAL void changed();
    Q_SIGNAL void userShaderEnabledChanged();
    Q_SIGNAL void intermediateFBOChanged();
    Q_SIGNAL void fragHeaderChanged();
    Q_SIGNAL void fragSampleChanged();
    Q_SIGNAL void fragPostProcessChanged();
    Q_SIGNAL void lastFileChanged();
    Q_SIGNAL void forceFrameRateChanged();
    Q_SIGNAL void decodingThreadsChanged(int n);
    Q_SIGNAL void decoderPriorityNamesChanged();
    Q_SIGNAL void registeredDecodersChanged(const QVector<int>& r);
    Q_SIGNAL void zeroCopyChanged();
    Q_SIGNAL void captureDirChanged(const QString& dir);
    Q_SIGNAL void captureFormatChanged(const QString& fmt);
    Q_SIGNAL void captureQualityChanged(int quality);
    Q_SIGNAL void avfilterVideoChanged();
    Q_SIGNAL void avfilterAudioChanged();
    Q_SIGNAL void subtitleEnabledChanged();
    Q_SIGNAL void subtitleAutoLoadChanged();
    Q_SIGNAL void subtitleEnginesChanged();
    Q_SIGNAL void subtitleFontChanged();
    Q_SIGNAL void subtitleColorChanged();
    Q_SIGNAL void subtitleOutlineChanged();
    Q_SIGNAL void subtitleOutlineColorChanged();
    Q_SIGNAL void subtitleBottomMarginChanged();
    Q_SIGNAL void subtitleDelayChanged();
    Q_SIGNAL void assFontFileChanged();
    Q_SIGNAL void assFontsDirChanged();
    Q_SIGNAL void assFontFileForcedChanged();
    Q_SIGNAL void previewEnabledChanged();
    Q_SIGNAL void previewWidthChanged();
    Q_SIGNAL void previewHeightChanged();
    Q_SIGNAL void EGLChanged();
    Q_SIGNAL void openGLTypeChanged();
    Q_SIGNAL void ANGLEPlatformChanged();
    Q_SIGNAL void avformatOptionsEnabledChanged();
    Q_SIGNAL void bufferValueChanged();
    Q_SIGNAL void timeoutChanged();
    Q_SIGNAL void abortOnTimeoutChanged();

protected:

    explicit AVPlayerConfigMngr(QObject* const parent = nullptr);
    ~AVPlayerConfigMngr();

public Q_SLOTS:

    void save();

private:

    class Data;
    Data* mpData;
};

} // namespace QtAV

#endif // QTAV_WIDGETS_AVPLAYER_CONFIG_MNGR_H
