/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-01
 * Description : unit test of qtav de-multiplexor
 *
 * SPDX-FileCopyrightText: 2022 by Steve Robbins <steve at sumost dot ca>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

// Qt includes

#include <QTest>

// Local includes

#include <sstream>
#include "AVDemuxer.h"
#include "qtavtestdatadir.h"

using namespace QtAV;

class DemuxTest : public QObject
{
    Q_OBJECT

private:

    QtAVTestDataDir m_testDataDir;
    QString testFile1() { return m_testDataDir.basemediav1_mp4(); }

    void checkPackets(const QString& filename)
    {
        AVDemuxer demux;
        QVERIFY(demux.setMedia(filename));
        QVERIFY(demux.load());

        QCOMPARE(demux.audioStreams().size(), 1);
        QCOMPARE(demux.videoStreams().size(), 1);
        QCOMPARE(demux.subtitleStreams().size(), 0);

        int audioStreamNum = demux.audioStream();
        int videoStreamNum = demux.videoStream();

        QCOMPARE(demux.startTime(), demux.startTimeUs() / 1000);
        QCOMPARE(demux.duration(), demux.durationUs() / 1000);

        qreal audio_dts = -1;
        qreal video_dts = -1;
        int packet_num = 0;

        while(demux.readPacket())
        {
            ++packet_num;
            std::stringstream ss;

            if      (demux.stream() == audioStreamNum)
            {
                ss << "failed at audio packet " << packet_num;
            }
            else if (demux.stream() == videoStreamNum)
            {
                ss << "failed at video packet " << packet_num;
            }
            else
            {
                QFAIL("read unknown stream");
            }

            auto temp   = ss.str();
            auto errMsg = temp.c_str();

            QVERIFY2(!demux.atEnd(), errMsg);

            auto p      = demux.packet();
            QVERIFY2(p.isValid(), errMsg);
            QVERIFY2(!p.isCorrupt, errMsg);

            // decoding time stamps (DTS) should be sequential

            if (demux.stream() == audioStreamNum)
            {
                QVERIFY2(p.dts >= audio_dts, errMsg);
                audio_dts = p.dts;
            }
            else
            {
                QVERIFY2(p.dts >= video_dts, errMsg);
                video_dts = p.dts;
            }
        }

        QVERIFY(demux.atEnd());

        // still loaded? QCOMPARE(demux.mediaStatus(), MediaStatus::EndOfMedia);
    }

private Q_SLOTS:

    void supportedFormats()
    {
        auto formats = AVDemuxer::supportedFormats();
        QVERIFY(formats.contains(QLatin1String("avi")));
    }

    void supportedExtensions()
    {
        auto extensions = AVDemuxer::supportedExtensions();
        QVERIFY(extensions.contains(QLatin1String("avi")));
    }

    void supportedProtocols()
    {
        auto protocols = AVDemuxer::supportedProtocols();
        QVERIFY(protocols.contains(QLatin1String("file")));
    }

    void status()
    {
        AVDemuxer demux;
        QCOMPARE(demux.mediaStatus(), MediaStatus::NoMedia);
        QCOMPARE(demux.atEnd(), false);
        QCOMPARE(demux.fileName(), QLatin1String(""));
        QCOMPARE(demux.isLoaded(), false);

        QVERIFY(demux.setMedia(testFile1()));
        QCOMPARE(demux.mediaStatus(), MediaStatus::NoMedia);
        QCOMPARE(demux.atEnd(), false);
        QCOMPARE(demux.fileName(), testFile1());
        QCOMPARE(demux.isLoaded(), false);

        QVERIFY(demux.load());
        QCOMPARE(demux.mediaStatus(), MediaStatus::LoadedMedia);
        QCOMPARE(demux.atEnd(), false);
        QCOMPARE(demux.fileName(), testFile1());
        QCOMPARE(demux.isLoaded(), true);

        QVERIFY(demux.unload());
        QCOMPARE(demux.mediaStatus(), MediaStatus::LoadedMedia);  // ??? why status shows loaded?
        QCOMPARE(demux.atEnd(), false);
        QCOMPARE(demux.fileName(), testFile1());
        QCOMPARE(demux.isLoaded(), false);
    }

    void checkBasemedia() { checkPackets(m_testDataDir.basemediav1_mp4()); }
    void checkRiffMpeg()  { checkPackets(m_testDataDir.riffMpeg_avi());    } 
    void checkMpeg2()     { checkPackets(m_testDataDir.mpeg2_mp4());       }
    void checkMpeg4()     { checkPackets(m_testDataDir.mpeg4_mp4());       }
};

QTEST_MAIN(DemuxTest)

#include "demux_utest.moc"
