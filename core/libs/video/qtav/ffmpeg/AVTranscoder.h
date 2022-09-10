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

#ifndef QTAV_AV_TRANSCODER_H
#define QTAV_AV_TRANSCODER_H

// Local includes

#include "MediaIO.h"
#include "AudioEncoder.h"
#include "VideoEncoder.h"

namespace QtAV
{

class AVPlayerCore;

class DIGIKAM_EXPORT AVTranscoder : public QObject
{
    Q_OBJECT

public:

    explicit AVTranscoder(QObject* const parent = nullptr);
    ~AVTranscoder();

    // TODO: other source (more operations needed, e.g. seek)?

    void setMediaSource(AVPlayerCore* player);
    AVPlayerCore* sourcePlayer()    const;

    QString outputFile()            const;
    QIODevice* outputDevice()       const;
    MediaIO* outputMediaIO()        const;

    /*!
     * \brief setOutputMedia
     */
    void setOutputMedia(const QString& fileName);
    void setOutputMedia(QIODevice* dev);
    void setOutputMedia(MediaIO* io);

    /*!
     * \brief setOutputFormat
     * Force the output format. Useful for custom io
     */
    void setOutputFormat(const QString& fmt);
    QString outputFormatForced() const;

    void setOutputOptions(const QVariantHash& dict);
    QVariantHash outputOptions() const;

    /*!
     * \brief setAsync
     * Enable async encoding. Default is disabled.
     */
    void setAsync(bool value = true);
    bool isAsync()               const;

    /*!
     * \brief createEncoder
     * Destroy old encoder and create a new one in filter chain.
     * Filter has the ownership. You shall not manually open it.
     * Transcoder will set the missing parameters open it.
     * \param name registered encoder name, for example "FFmpeg"
     * \return false if failed
     */
    bool createVideoEncoder(const QString& name = QLatin1String("FFmpeg"));

    /*!
     * \brief encoder
     * Use this to set encoder properties and options.
     * If frameRate is not set, source frame rate will be set if it's valid,
     * otherwise VideoEncoder::defaultFrameRate() will be used internally
     * Do not call open()/close() manually
     * \return Encoder instance or null if createVideoEncoder failed
     */
    VideoEncoder* videoEncoder() const;

    /*!
     * \brief createEncoder
     * Destroy old encoder and create a new one in filter chain.
     * Filter has the ownership. You shall not manually open it.
     * Transcoder will set the missing parameters open it.
     * \param name registered encoder name, for example "FFmpeg"
     * \return false if failed
     */
    bool createAudioEncoder(const QString& name = QLatin1String("FFmpeg"));

    /*!
     * \brief encoder
     * Use this to set encoder properties and options.
     * Do not call open()/close() manually
     * \return Encoder instance or null if createAudioEncoder failed
     */
    AudioEncoder* audioEncoder() const;

    /*!
     * \brief isRunning
     * \return true if encoding started
     */
    bool isRunning()             const;
    bool isPaused()              const;
    qint64 encodedSize()         const;
    qreal startTimestamp()       const;
    qreal encodedDuration()      const;

    /*!
     * \brief startTime
     * Start to encode after startTime(). To decode from a given time, call sourcePlayer()->setPosition()
     */
    qint64 startTime()           const;
    void setStartTime(qint64 ms);

Q_SIGNALS:

    void videoFrameEncoded(qreal timestamp);
    void audioFrameEncoded(qreal timestamp);
    void started();
    void stopped();
    void paused(bool value);
    void startTimeChanged(qint64 ms);
    void asyncChanged();

public Q_SLOTS:

    void start();

    /*!
     * \brief stop
     * Call stop() to encode delayed frames remains in encoder and then stop encoding.
     * It's called internally when sourcePlayer() is stopped
     */
    void stop();

    /*!
     * \brief pause
     * pause the encoders
     * \param value
     */
    void pause(bool value);

private Q_SLOTS:

    void onSourceStarted();
    void prepareMuxer();
    void writeAudio(const QtAV::Packet& packet);
    void writeVideo(const QtAV::Packet& packet);
    void tryFinish();

private:

    void stopInternal();

    class Private;
    QScopedPointer<Private> d;
};

} // namespace QtAV

#endif // QTAV_AV_TRANSCODER_H
