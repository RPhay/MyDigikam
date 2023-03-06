/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-09-18
 * Description : unit test of qtav video decoder
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
#include "VideoDecoder.h"
#include "Packet.h"
#include "digikam_debug.h"
#include "qtavtestdatadir.h"

using namespace QtAV;


class VideoDecoderTest : public QObject
{
    Q_OBJECT

private:

    QtAVTestDataDir m_testDataDir;
    QString testFile1() { return m_testDataDir.mpeg2_mp4(); }

private Q_SLOTS:

    void canConstruct()
    {
        auto decoder = VideoDecoder::create();
        QVERIFY(decoder != nullptr);

        QCOMPARE(decoder->name(), QString::fromUtf8("FFmpeg"));
        QCOMPARE(decoder->id(), VideoDecoderId_FFmpeg);
    }

    void readStream()
    {
        AVDemuxer demux;
        QVERIFY(demux.setMedia(testFile1()));
        QVERIFY(demux.load());

        auto decoder = VideoDecoder::create();
        QVERIFY(decoder != nullptr);

        decoder->setCodecContext(demux.videoCodecContext());
        QVERIFY(decoder->open());

        int streamNum        = demux.videoStream();
        int frameCount       = 0;
        qreal last_timestamp = -1;

        Packet pkt;

        while(!demux.atEnd())
        {
            if (!pkt.isValid())
            {
                if (!demux.readPacket()) continue;

                if (demux.stream() != streamNum) continue;

                pkt = demux.packet();
            }

            if (!decoder->decode(pkt))
            {
                pkt = Packet();   // set invalid to read from demuxer
                continue;
            }

            pkt.data = QByteArray::fromRawData(pkt.data.constData() + pkt.data.size() - decoder->undecodedSize(), decoder->undecodedSize());
            VideoFrame frame(decoder->frame());

            if (!frame) continue;

            ++frameCount;
            //QVERIFY(frame.timestamp() >= last_timestamp);
            last_timestamp = frame.timestamp();
            Q_UNUSED(last_timestamp);

            auto format    = frame.format();

            QVERIFY(format.isValid());
            QVERIFY(format.isPlanar());
            QCOMPARE(format.planeCount(), 3);

            QCOMPARE(format.channels(), 3);
        }

        QVERIFY(demux.atEnd());

        QCOMPARE(demux.frames(demux.videoStream()), 150);
        QCOMPARE(frameCount, 149);  
    }
};

QTEST_MAIN(VideoDecoderTest)

#include "videodecoder_utest.moc"
