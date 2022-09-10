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

#ifndef QTAV_AUDIO_FRAME_H
#define QTAV_AUDIO_FRAME_H

// Local includes

#include "Frame.h"
#include "AudioFormat.h"

namespace QtAV
{

class AudioResampler;
class AudioFramePrivate;

class DIGIKAM_EXPORT AudioFrame : public Frame             // clazy:exclude=copyable-polymorphic
{
    Q_DECLARE_PRIVATE(AudioFrame)

public:

    // data must be complete

    /*!
     * \brief AudioFrame
     * construct an audio frame from a given buffer and format
     */
    explicit AudioFrame(const AudioFormat& format = AudioFormat(), const QByteArray& data = QByteArray());
    AudioFrame(const AudioFrame& other);
    virtual ~AudioFrame();

    AudioFrame& operator =(const AudioFrame& other);

    bool isValid()                        const;

    operator bool()                       const
    {
        return isValid();
    }

    /*!
     * \brief data
     * Audio data. clone is called if frame is not constructed with a QByteArray.
     * \return
     */
    QByteArray data();
    virtual int channelCount()            const override;

    /*!
     * Deep copy. If you want to copy data from somewhere, knowing the format, width and height,
     * then you can use clone().
     */
    AudioFrame clone()                    const;
    AudioFrame mid(int pos, int len = -1) const;
    void prepend(AudioFrame& other);
    AudioFormat format()                  const;
    void setSamplesPerChannel(int samples);

    // may change after resampling

    int samplesPerChannel()               const;
    AudioFrame to(const AudioFormat& fmt) const;

    // AudioResamplerId

    void setAudioResampler(AudioResampler* conv); // TODO: remove

    /*!
        Returns the number of microseconds represented by \a bytes in this format.
        Returns 0 if this format is not valid.
        Note that some rounding may occur if \a bytes is not an exact multiple
        of the number of bytes per frame.
    */
    qint64 duration()                     const;
};

} // namespace QtAV

Q_DECLARE_METATYPE(QtAV::AudioFrame)

#endif // QTAV_AUDIO_FRAME_H
