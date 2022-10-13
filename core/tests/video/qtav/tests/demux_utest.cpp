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
#include "QtAvTestDataDir.h"


using namespace QtAV;

class DemuxTest : public QObject
{
    Q_OBJECT

private:
    QtAvTestDataDir _testDataDir;
    QString testFile1() { return _testDataDir.basemediav1_mp4(); }

    void checkPackets(QString filename, int audioFrames, int videoFrames)
    {
        AVDemuxer demux;
        QVERIFY(demux.setMedia(filename));
        QVERIFY(demux.load());

        QCOMPARE(demux.audioStreams().size(), 1);
        QCOMPARE(demux.videoStreams().size(), 1);
        QCOMPARE(demux.subtitleStreams().size(), 0);
        
        int audioStreamNum = demux.audioStream();
        int videoStreamNum = demux.videoStream();

        QCOMPARE(demux.frames(demux.audioStream()), audioFrames);
        QCOMPARE(demux.frames(demux.videoStream()), videoFrames);

        QCOMPARE(demux.startTime(), demux.startTimeUs() / 1000);
        QCOMPARE(demux.duration(), demux.durationUs() / 1000);

        qreal audio_dts = -1;
        qreal video_dts = -1;
        
        int audioFrameCount = 0;
        int videoFrameCount = 0;
        while(demux.readFrame())
        {
            std::stringstream ss;

            if (demux.stream() == audioStreamNum)
            {
                ++audioFrameCount;
                ss << "failed at audio frame " << audioFrameCount;
            }
            else if (demux.stream() == videoStreamNum)
            {
                ++videoFrameCount;
                ss << "failed at video frame " << videoFrameCount;
            }
            else
            {
                QFAIL("read unknown stream");
            }

            auto temp = ss.str();
            auto errMsg = temp.c_str();

            QVERIFY2(!demux.atEnd(), errMsg);

            auto p = demux.packet();
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

        QCOMPARE(audioFrameCount, audioFrames); 
        QCOMPARE(videoFrameCount, videoFrames);
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
    
    void checkBasemedia() { checkPackets(_testDataDir.basemediav1_mp4(), 236, 150); }
    void checkRiffMpeg() { checkPackets(_testDataDir.riffMpeg_avi(), 210, 151); }
    void checkMpeg2() { checkPackets(_testDataDir.mpeg2_mp4(), 236, 150); }
    void checkMpeg4() { checkPackets(_testDataDir.mpeg4_mp4(), 236, 150); }
};

QTEST_MAIN(DemuxTest)
#include "demux_utest.moc"
