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

#ifndef QTAV_ENCODE_FILTER_H
#define QTAV_ENCODE_FILTER_H

// Local includes

#include "Filter.h"
#include "Packet.h"
#include "AudioFrame.h"
#include "VideoFrame.h"

namespace QtAV
{

class AudioEncoder;
class AudioEncodeFilterPrivate;

class DIGIKAM_EXPORT AudioEncodeFilter : public AudioFilter
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(AudioEncodeFilter)

public:

    explicit AudioEncodeFilter(QObject* const parent = nullptr);

    /*!
     * \brief setAsync
     * Enable async encoding. Default is disabled.
     */
    void setAsync(bool value = true);
    bool isAsync()                  const;

    /*!
     * \brief createEncoder
     * Destroy old encoder and create a new one. Filter has the ownership.
     * Encoder will open when encoding first valid frame, and set width/height as frame's.
     * \param name registered encoder name, for example "FFmpeg"
     * \return null if failed
     */
    AudioEncoder* createEncoder(const QString& name = QLatin1String("FFmpeg"));

    /*!
     * \brief encoder
     * Use this to set encoder properties and options
     * \return Encoder instance or null if createEncoder failed
     */
    AudioEncoder* encoder()         const;

    // TODO: async property

    /*!
     * \brief startTime
     * start to encode after startTime()
     */
    qint64 startTime()              const;
    void setStartTime(qint64 value);

public Q_SLOTS:

    /*!
     * \brief finish
     * Tell the encoder no more frames to encode.
     * Signal finished() will be emitted when all frames are encoded
     */
    void finish();

Q_SIGNALS:

    void finished();

    /*!
     * \brief readyToEncode
     * Emitted when encoder is open.
     * All parameters are set and muxer can set codec properties now.
     * close the encoder() to reset and reopen.
     */
    void readyToEncode();
    void frameEncoded(const QtAV::Packet& packet);
    void startTimeChanged(qint64 value);

    // internal use only

    void requestToEncode(const QtAV::AudioFrame& frame);

protected Q_SLOTS:

    void encode(const QtAV::AudioFrame& frame = AudioFrame());

protected:

    virtual void process(Statistics* statistics, AudioFrame* frame = nullptr) override;
};

// --------------------------------------------------------------------

class VideoEncoder;
class VideoEncodeFilterPrivate;

class DIGIKAM_EXPORT VideoEncodeFilter : public VideoFilter
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(VideoEncodeFilter)

public:

    explicit VideoEncodeFilter(QObject* const parent = nullptr);

    /*!
     * \brief setAsync
     * Enable async encoding. Default is disabled.
     */
    void setAsync(bool value = true);
    bool isAsync() const;

    bool isSupported(VideoFilterContext::Type t) const override
    {
        return (t == VideoFilterContext::None);
    }

    /*!
     * \brief createEncoder
     * Destroy old encoder and create a new one. Filter has the ownership.
     * Encoder will open when encoding first valid frame, and set width/height as frame's.
     * \param name registered encoder name, for example "FFmpeg"
     * \return null if failed
     */
    VideoEncoder* createEncoder(const QString& name = QLatin1String("FFmpeg"));

    /*!
     * \brief encoder
     * Use this to set encoder properties and options
     * \return Encoder instance or null if createEncoder failed
     */
    VideoEncoder* encoder() const;

    // TODO: async property

    /*!
     * \brief startTime
     * start to encode after startTime()
     */
    qint64 startTime() const;
    void setStartTime(qint64 value);

public Q_SLOTS:

    /*!
     * \brief finish
     * Tell the encoder no more frames to encode.
     * Signal finished() will be emitted when all frames are encoded
     */
    void finish();

Q_SIGNALS:

    void finished();

    /*!
     * \brief readyToEncode
     * Emitted when encoder is open.
     * All parameters are set and muxer can set codec properties now.
     * close the encoder() to reset and reopen.
     */
    void readyToEncode();
    void frameEncoded(const QtAV::Packet& packet);
    void startTimeChanged(qint64 value);

    // internal use only

    void requestToEncode(const QtAV::VideoFrame& frame);

protected Q_SLOTS:

    void encode(const QtAV::VideoFrame& frame = VideoFrame());

protected:

    virtual void process(Statistics* statistics, VideoFrame* frame = nullptr) override;
};

} // namespace QtAV

#endif // QTAV_ENCODE_FILTER_H
