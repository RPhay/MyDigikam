/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : an unit-test to detect image quality level
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "detectnoise_utest.h"

// Qt includes

#include <QTest>
#include <QStringList>
#include <QFileInfoList>
#include <QDebug>
#include <QDir>

// Local includes

#include "digikam_debug.h"
#include "imgqsorttest.h"

using namespace Digikam;

QTEST_MAIN(ImgQSortTestDetectNoise)

ImgQSortTestDetectNoise::ImgQSortTestDetectNoise(QObject* const parent)
    : ImgQSortTest(parent)
{
    m_dataTestCases = dataTestCases;
}

void ImgQSortTestDetectNoise::testParseTestImagesForNoiseDetection()
{
    QHash<QString, bool> results = testParseTestImages(QLatin1String("noiseDetection"),
                                                       ImgQSortTest_ParseTestImagesDefautDetection, DETECTNOISE);

    for (const auto& test_case : results.keys())
    {
        QVERIFY(results.value(test_case));
    }
}

void ImgQSortTestDetectNoise::testParseTestImagesForImageHighSO()
{
    QHash<QString, bool> results = testParseTestImages(QLatin1String("highISO"),
                                                       ImgQSortTest_ParseTestImagesDefautDetection, DETECTNOISE);

    for (const auto& test_case : results.keys())
    {
        QVERIFY(results.value(test_case));
    }
}

void ImgQSortTestDetectNoise::testParseTestImagesForVariousTypeNoise()
{
    QHash<QString, bool> results = testParseTestImages(QLatin1String("variousTypesNoise"),
                                                       ImgQSortTest_ParseTestImagesDefautDetection, DETECTNOISE);

    for (const auto& test_case : results.keys())
    {
        QVERIFY(results.value(test_case));
    }
}

void ImgQSortTestDetectNoise::testParseTestImagesForVariousTypeNoiseFailCase()
{
    QHash<QString, bool> results = testParseTestImages(QLatin1String("variousTypesNoiseFailCase"),
                                                       ImgQSortTest_ParseTestImagesDefautDetection, DETECTNOISE);

    for (const auto& test_case : results.keys())
    {
        QEXPECT_FAIL("", "Will fix in the next release", Continue);

        QVERIFY(results.value(test_case));
    }
}
