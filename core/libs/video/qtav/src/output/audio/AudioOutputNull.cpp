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

#include "AudioOutputBackend.h"

// Local includes

#include "QtAV_mkid.h"
#include "QtAV_factory.h"

namespace QtAV
{

// TODO: block internally

static const char kName[] = "null";

class Q_DECL_HIDDEN AudioOutputNull : public AudioOutputBackend
{
    Q_OBJECT

public:

    AudioOutputNull(QObject* const parent = nullptr);

    QString name() const                override
    {
        return QLatin1String(kName);
    }

    bool open()                         override
    {
        return true;
    }

    bool close()                        override
    {
        return true;
    }

    // TODO: check channel layout. Null supports channels>2

    BufferControl bufferControl() const override
    {
        return Blocking;
    }

    bool write(const QByteArray&)       override
    {
        return true;
    }

    bool play()                         override
    {
        return true;
    }
};

typedef AudioOutputNull AudioOutputBackendNull;

static const AudioOutputBackendId AudioOutputBackendId_Null = mkid::id32base36_4<'n', 'u', 'l', 'l'>::value;

FACTORY_REGISTER(AudioOutputBackend, Null, kName)

AudioOutputNull::AudioOutputNull(QObject* const parent)
    : AudioOutputBackend(AudioOutput::DeviceFeatures(), parent)
{
}

} // namespace QtAV

#include "AudioOutputNull.moc"
