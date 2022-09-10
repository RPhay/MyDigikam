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

#ifndef QTAV_AUDIO_RESAMPLER_P_H
#define QTAV_AUDIO_RESAMPLER_P_H

#include "AudioResampler.h"

// Qt includes

#include <QByteArray>

// Local includes

#include "AudioFormat.h"
#include "AVCompat.h"

namespace QtAV
{

class AudioResampler;

class DIGIKAM_EXPORT AudioResamplerPrivate : public DPtrPrivate<AudioResampler>
{
public:

    AudioResamplerPrivate()
      : in_samples_per_channel (0),
        out_samples_per_channel(0),
        speed                  (1.0)
    {
        in_format.setSampleFormat(AudioFormat::SampleFormat_Unknown);
        out_format.setSampleFormat(AudioFormat::SampleFormat_Float);
    }

public:

    int         in_samples_per_channel;
    int         out_samples_per_channel;
    qreal       speed;
    AudioFormat in_format;
    AudioFormat out_format;
    QByteArray  data_out;

private:

    Q_DISABLE_COPY(AudioResamplerPrivate);
};

} // namespace QtAV

#endif // QTAV_AUDIO_RESAMPLER_P_H
