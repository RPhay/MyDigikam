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

// Qt includes

#include <QCoreApplication>
#include <QScopedPointer>

// Local includes

#include "QtAV.h"
#include "digikam_debug.h"

using namespace QtAV;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qCDebug(DIGIKAM_TESTS_LOG)
        << QLatin1String("usage: ")
        << a.applicationFilePath().split(QLatin1String("/")).last().append(QLatin1String(" url"));

    if (a.arguments().size() < 2)
        return 0;

    QScopedPointer<AudioOutput> ao(new AudioOutput());
    AVDemuxer demuxer;
    demuxer.setMedia(a.arguments().last());

    if (!demuxer.load())
    {
        qWarning() << "Failed to load file " << demuxer.fileName();

        return 1;
    }

    QScopedPointer<AudioDecoder> dec(AudioDecoder::create()); // delete by user
    dec->setCodecContext(demuxer.audioCodecContext());
/*
    dec->prepare();
*/
    if (!dec->open())
        qFatal("open decoder error");

    int astream = demuxer.audioStream();
    Packet pkt;

    while (!demuxer.atEnd())
    {
        if (!pkt.isValid())
        {
            // continue to decode previous undecoded data

            if (!demuxer.readPacket() || (demuxer.stream() != astream))
                continue;

            pkt = demuxer.packet();
        }

        if (!dec->decode(pkt))
        {
            pkt = Packet(); // set invalid to read from demuxer

            continue;
        }

        // decode the rest data in the next loop. read from demuxer if no data remains

        pkt.data = QByteArray::fromRawData(pkt.data.constData() + pkt.data.size() - dec->undecodedSize(),
                                           dec->undecodedSize());

        AudioFrame frame(dec->frame()); // why is faster to call frame() for hwdec? no frame() is very slow for VDA

        if (!frame)
            continue;
/*
        frame.setAudioResampler(dec->resampler()); // if not set, always create a resampler in AudioFrame.to()
*/
        AudioFormat af(frame.format());

        if (ao->isOpen())
        {
            af = ao->audioFormat();
        }
        else
        {
            ao->setAudioFormat(af);
            dec->resampler()->setOutAudioFormat(ao->audioFormat());

            // if decoded format is not supported by audio renderer, change decoder output format

            if (af != ao->audioFormat())
                dec->resampler()->prepare();

            // now af is supported by audio renderer. it's safe to open

            if (!ao->open())
                qFatal("Open audio output error");

#if 0 // always resample ONCE due to QtAV bug

            // the first format unsupported frame still need to be converted to a supported format

            if (!ao->isSupported(frame.format()))
                frame = frame.to(af);

#endif

            qCDebug(DIGIKAM_TESTS_LOG) << "Input: " << frame.format();
            qCDebug(DIGIKAM_TESTS_LOG) << "Output: " << af;
        }

        qCDebug(DIGIKAM_TESTS_LOG).noquote()
            << QString::asprintf("playing: %.3f...\r",
                frame.timestamp());

        // always resample ONCE. otherwise data are all 0x0. QtAV bug

        frame = frame.to(af);
        QByteArray data(frame.data()); // plane data. currently only packet sample formats are supported.

        while (!data.isEmpty())
        {
            ao->play(QByteArray::fromRawData(data.constData(), qMin(data.size(), ao->bufferSize())));
            data.remove(0, qMin(data.size(), ao->bufferSize()));
        }
    }

    // dec, ao will be closed in dtor. demuxer will call unload in dtor

    return 0;
}
