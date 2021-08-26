/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-04
 * Description : an unit-test to detect image quality level
 *
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "detectblur_utest.h"

// Qt includes

#include <QTest>
#include <QStringList>
#include <QFileInfoList>
#include <QDebug>
#include <QDir>

// Local includes

#include "digikam_globals.h"
#include "imagequalitycontainer.h"
#include "dpluginloader.h"
#include "digikam_debug.h"

using namespace Digikam;

QTEST_MAIN(ImgQSortTestDetectBlur)

ImgQSortTestDetectBlur::ImgQSortTestDetectBlur(QObject* const parent)
  : ImgQSortTest(parent)
{
    m_dataTestCases = dataTestCases;
}

void ImgQSortTestDetectBlur::testParseTestImagesForBlurDetection()
{
    QHash<QString, bool> results = testParseTestImages(QLatin1String("blurDetection"), 
                                                       ImgQSortTest_ParseTestImagesDefautDetection, DETECTBLUR);

    for (const auto& test_case : results.keys())
    {
        QVERIFY(results.value(test_case));
    }
}

void ImgQSortTestDetectBlur::testParseTestImagesForBlurDetection_SharpImage()
{
    QHash<QString, bool> results = testParseTestImages(QLatin1String("sharpImage"),
                                                       ImgQSortTest_ParseTestImagesDefautDetection, DETECTBLUR);

    for (const auto& test_case : results.keys())
    {
        QVERIFY(results.value(test_case));
    }
}

void ImgQSortTestDetectBlur::testParseTestImagesForBlurDetection_MotionBlurImage()
{
    QHash<QString, bool> results = testParseTestImages(QLatin1String("motionBlurImage"),
                                                       ImgQSortTest_ParseTestImagesDefautDetection, DETECTBLUR);

    for (const auto& test_case : results.keys())
    {
        QVERIFY(results.value(test_case));
    }
}

void ImgQSortTestDetectBlur::testParseTestImagesForBlurDetection_DefocusImage()
{
    QHash<QString, bool> results = testParseTestImages(QLatin1String("defocusImage"),
                                                       ImgQSortTest_ParseTestImagesDefautDetection, DETECTBLUR);

    for (const auto& test_case : results.keys())
    {
        QVERIFY(results.value(test_case));
    }
}

void ImgQSortTestDetectBlur::testParseTestImagesForBlurDetection_BlurBackGroundImage()
{
    QHash<QString, bool> results = testParseTestImages(QLatin1String("blurBackGroundImage"),
                                                       ImgQSortTest_ParseTestImagesDefautDetection, DETECTBLUR);

    for (const auto& test_case : results.keys())
    {
        QVERIFY(results.value(test_case));
    }
}

void ImgQSortTestDetectBlur::testParseTestImagesForBlurDetection_FailCase()
{
    QHash<QString, bool> results = testParseTestImages(QLatin1String("blurDetectionFailTest"),
                                                       ImgQSortTest_ParseTestImagesDefautDetection, DETECTBLUR);

    for (const auto& test_case : results.keys())
    {
        QEXPECT_FAIL("", "Will fix in the next release", Continue);

        QVERIFY(results.value(test_case));
    }
}
