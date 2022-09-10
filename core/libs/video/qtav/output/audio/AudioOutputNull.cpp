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
