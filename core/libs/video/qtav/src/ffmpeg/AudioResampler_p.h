/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
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

class QTAV_PRIVATE_EXPORT AudioResamplerPrivate : public DPtrPrivate<AudioResampler>
{
public:

    AudioResamplerPrivate()
      : in_samples_per_channel(0),
        out_samples_per_channel(0),
        speed(1.0)
    {
        in_format.setSampleFormat(AudioFormat::SampleFormat_Unknown);
        out_format.setSampleFormat(AudioFormat::SampleFormat_Float);
    }

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
