/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-01
 * Description : unit test of qtav static configuration
 *
 * SPDX-FileCopyrightText: 2022 by Steve Robbins <steve at sumost dot ca>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

// Qt includes

#include <QTest>

// Local includes

#include "VideoDecoder.h"
#include "VideoEncoder.h"
#include "AudioDecoder.h"
#include "AudioEncoder.h"
#include "AVMuxer.h"
#include "LibAVFilter.h"

using namespace QtAV;

class CheckConfig : public QObject
{
    Q_OBJECT

public:

    CheckConfig(QObject* const parent = nullptr)
        : QObject(parent)
    {
    }

private Q_SLOTS:

    void DecoderConfig()
    {
        auto videoCodecs = VideoDecoder::supportedCodecs();
        QVERIFY(videoCodecs.size() > 0);
        //QVERIFY(videoCodecs.contains(QLatin1String("mpegvideo")));

        auto audioCodecs = AudioDecoder::supportedCodecs();
        QVERIFY(audioCodecs.size() > 0);
        //QVERIFY(audioCodecs.contains(QLatin1String("mp3")));
    }

    void EncoderConfig()
    {
        auto videoCodecs = VideoEncoder::supportedCodecs();
        QVERIFY(videoCodecs.size() > 0);
        //QVERIFY(videoCodecs.contains(QLatin1String("mpeg4")));

        auto audioCodecs = AudioEncoder::supportedCodecs();
        QVERIFY(audioCodecs.size() > 0);
        //QVERIFY(audioCodecs.contains(QLatin1String("mp2")));
    }

    void MuxerConfig()
    {
        auto formats = AVMuxer::supportedFormats();
        QVERIFY(formats.size() > 0);
        //QVERIFY(formats.contains(QLatin1String("mpeg")));

        auto extensions = AVMuxer::supportedExtensions();
        QVERIFY(extensions.size() > 0);
        //QVERIFY(extensions.contains(QLatin1String("xbm")));
    }

    void LibAvConfig()
    {
        auto videoFilters = LibAVFilter::videoFilters();
        QVERIFY(videoFilters.size() > 0);
        //QVERIFY(videoFilters.contains(QLatin1String("chromakey")));

        auto audioFilters = LibAVFilter::audioFilters();
        QVERIFY(audioFilters.size() > 0);
        //QVERIFY(audioFilters.contains(QLatin1String("earwax")));
    }
};

QTEST_MAIN(CheckConfig)

#include "checkconfig_utest.moc"
