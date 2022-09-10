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

// C++ includes

#include <limits>

// Qt includes

#include <qmath.h>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QStringList>

// Local includes

#include "AudioOutput.h"
#include "digikam_debug.h"

using namespace QtAV;

const int kTableSize         = 200;
const int kFrames            = 512;
qint16 sin_table[kTableSize] = { 0 };

void help()
{
    qCDebug(DIGIKAM_TESTS_LOG)
        << QLatin1String("parameters: [-ao ")
        << AudioOutput::backendsAvailable().join(QLatin1String("|"))
        << QLatin1String("]");
}

int main(int argc, char** argv)
{
    help();

    // initialise sinusoidal wavetable

    for (int i = 0 ; i < kTableSize ; ++i)
    {
        sin_table[i] = (qint16)((double)std::numeric_limits<qint16>::max() *
                        sin(((double)i / (double)kTableSize) * M_PI * 2.0));
    }

    QCoreApplication app(argc, argv); // only used qapp to get parameter easily
    AudioOutput ao;
    int idx = app.arguments().indexOf(QLatin1String("-ao"));

    if (idx > 0)
    {
        ao.setBackends(QStringList() << app.arguments().at(idx+1));
    }

    if (ao.backend().isEmpty())
    {
        qCCritical(DIGIKAM_TESTS_LOG)
            << QString::asprintf("unknow backend");

        return -1;
    }

    AudioFormat af;
    af.setChannels(2);
    af.setSampleFormat(AudioFormat::SampleFormat_Signed16);
    af.setSampleRate(44100);

    if (!ao.isSupported(af))
    {
        qCDebug(DIGIKAM_TESTS_LOG)
            << "does not support format: " << af;

        return -1;
    }

    ao.setAudioFormat(af);
    QByteArray data(af.bytesPerFrame()*kFrames, 0); // bytesPerSample*channels*1024
    ao.setBufferSamples(kFrames);

    if (!ao.open())
    {
        qCCritical(DIGIKAM_TESTS_LOG)
            << QString::asprintf("open audio error");

        return -1;
    }

    int left  = 0;
    int right = 0;
    QElapsedTimer timer;
    timer.start();

    while (timer.elapsed() < 3000)
    {
        qint16* d = reinterpret_cast<qint16*>(data.data());

        for (int k = 0 ; k < kFrames ; ++k)
        {
            *d++  = sin_table[left];
            *d++  = sin_table[right];
            left  = (left  + 1) % kTableSize;
            right = (right + 3) % kTableSize;
        }

        ao.setVolume(2 * sin(2.0 * M_PI / 1000.0 * timer.elapsed()));
        ao.play(data);
    }

    ao.close();

    return 0;
}
