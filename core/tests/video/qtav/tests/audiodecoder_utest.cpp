/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-09-18
 * Description : unit test of qtav audio decoder
 *
 * SPDX-FileCopyrightText: 2022 by Steve Robbins <steve at sumost dot ca>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

// Qt includes

#include <QCoreApplication>
#include <QDateTime>
#include <QQueue>
#include <QStringList>
#include <QTest>

// Local includes

#include <sstream>
#include "AVDemuxer.h"
#include "AudioDecoder.h"
#include "Packet.h"
#include "digikam_debug.h"
#include "qtavtestdatadir.h"

using namespace QtAV;

class AudioDecodeTest : public QObject
{
    Q_OBJECT

private:

    QtAVTestDataDir m_testDataDir;
    QString testFile1() { return m_testDataDir.basemediav1_mp4(); }

private Q_SLOTS:

    void testDirValid()
    {
        QVERIFY(m_testDataDir.isValid());
    }

    void canConstruct()
    {
        auto decoder = AudioDecoder::create();
        QVERIFY(decoder != nullptr);

        QCOMPARE(decoder->name(), QString::fromUtf8("FFmpeg"));
        QCOMPARE(decoder->id(), AudioDecoderId_FFmpeg);
    }

    void readStream()
    {
        AVDemuxer demux;
        QVERIFY(demux.setMedia(testFile1()));
        QVERIFY(demux.load());

        auto decoder = AudioDecoder::create();
        QVERIFY(decoder != nullptr);

        decoder->setCodecContext(demux.audioCodecContext());
        QVERIFY(decoder->open());

        int audioStreamNum = demux.audioStream();
        int audioFrameCount = 0;
        qreal last_timestamp = -1;

        Packet pkt;

        while(!demux.atEnd())
        {
            if (!pkt.isValid())
            {
                if (!demux.readPacket()) continue;

                if (demux.stream() != audioStreamNum) continue;

                pkt = demux.packet();
            }

            if (!decoder->decode(pkt))
            {
                pkt = Packet();   // set invalid to read from demuxer
                continue;
            }

            pkt.data = QByteArray::fromRawData(pkt.data.constData() + pkt.data.size() - decoder->undecodedSize(), decoder->undecodedSize());
            AudioFrame frame(decoder->frame());

            if (!frame) continue;

            ++audioFrameCount;
            QVERIFY(frame.timestamp() > last_timestamp);
            last_timestamp = frame.timestamp();

            auto format = frame.format();

            QVERIFY(format.isValid());
            QVERIFY(format.isFloat());
            QVERIFY(!format.isUnsigned());
            QVERIFY(format.isPlanar());
            QCOMPARE(format.planeCount(), 2);

            QCOMPARE(format.sampleRate(), 48000);

            QCOMPARE(format.channelLayoutFFmpeg(), 3);
            QCOMPARE(format.channelLayout(), AudioFormat::ChannelLayout::ChannelLayout_Stereo);

            QCOMPARE(format.channels(), 2);

            QCOMPARE(format.sampleFormat(), AudioFormat::SampleFormat::SampleFormat_FloatPlanar);
            QCOMPARE(format.sampleFormatFFmpeg(), 8);

            QCOMPARE(format.bytesPerFrame(), 8);
            QCOMPARE(format.bytesPerSample(), 4);
            QCOMPARE(format.sampleSize(), 4);
            QCOMPARE(format.bitRate(), 48000*2*4*8);       // 2 channels, 4 bytes per sample, 8 bits/byte
            QCOMPARE(format.bytesPerSecond(), 48000*2*4);  // 2 channels, 4 bytes per sample
        }

        QVERIFY(demux.atEnd());

        QCOMPARE(demux.frames(demux.audioStream()), 236);
        QCOMPARE(audioFrameCount, 235);  // missing a frame -- maybe because the loop ended at reading the last video frame?
    }
};

QTEST_MAIN(AudioDecodeTest)

#include "audiodecoder_utest.moc"
