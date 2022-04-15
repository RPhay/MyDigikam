/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
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

#include "QtAV/private/AudioOutputBackend.h"
#include "QtAV/private/mkid.h"
#include "QtAV/private/factory.h"

namespace QtAV {
//TODO: block internally
static const char kName[] = "null";
class AudioOutputNull : public AudioOutputBackend
{
public:
    AudioOutputNull(QObject *parent = 0);
    QString name() const Q_DECL_OVERRIDE { return QLatin1String(kName);}
    bool open() Q_DECL_OVERRIDE { return true;}
    bool close() Q_DECL_OVERRIDE { return true;}
    // TODO: check channel layout. Null supports channels>2
    BufferControl bufferControl() const Q_DECL_OVERRIDE { return Blocking;}
    bool write(const QByteArray&) Q_DECL_OVERRIDE { return true;}
    bool play() Q_DECL_OVERRIDE { return true;}

};

typedef AudioOutputNull AudioOutputBackendNull;
static const AudioOutputBackendId AudioOutputBackendId_Null = mkid::id32base36_4<'n', 'u', 'l', 'l'>::value;
FACTORY_REGISTER(AudioOutputBackend, Null, kName)

AudioOutputNull::AudioOutputNull(QObject *parent)
    : AudioOutputBackend(AudioOutput::DeviceFeatures(), parent)
{}

} //namespace QtAV
