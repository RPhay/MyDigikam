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

#include "detectgeneral_utest.h"

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

using namespace Digikam;

QTEST_MAIN(ImgQSortTestGeneral)

ImgQSortTestGeneral::ImgQSortTestGeneral(QObject* const)
{
    m_dataTestCases = dataTestCases;
}

void ImgQSortTestGeneral::testParseTestImagesForGeneralBadImage()
{
    QHash<QString, bool> results = testParseTestImages(QLatin1String("badImage"),
                                                       ImgQSortTest_ParseTestImagesDefautDetection, DETECTIONGENERAL);

    for (const auto& test_case : results.keys())
    {
        QVERIFY(results.value(test_case));
    }
}

void ImgQSortTestGeneral::testParseTestImagesForGeneralQuiteBadImage()
{
    QHash<QString, bool> results = testParseTestImages(QLatin1String("quiteBadImage"),
                                                       ImgQSortTest_ParseTestImagesDefautDetection, DETECTIONGENERAL);

    for (const auto& test_case : results.keys())
    {
        QVERIFY(results.value(test_case));
    }
}

void ImgQSortTestGeneral::testParseTestImagesForGeneralNormalImage()
{
    QHash<QString, bool> results = testParseTestImages(QLatin1String("normalImage"),
                                                       ImgQSortTest_ParseTestImagesDefautDetection, DETECTIONGENERAL);

    for (const auto& test_case : results.keys())
    {
        QVERIFY(results.value(test_case));
    }
}

void ImgQSortTestGeneral::testParseTestImagesForGeneralQuiteGoodImage()
{
    QHash<QString, bool> results = testParseTestImages(QLatin1String("quiteGoodImage"),
                                                       ImgQSortTest_ParseTestImagesDefautDetection, DETECTIONGENERAL);

    for (const auto& test_case : results.keys())
    {
        QVERIFY(results.value(test_case));
    }
}

void ImgQSortTestGeneral::testParseTestImagesForGeneralGoodImage()
{
    QHash<QString, bool> results = testParseTestImages(QLatin1String("goodImage"),
                                                       ImgQSortTest_ParseTestImagesDefautDetection, DETECTIONGENERAL);

    for (const auto& test_case : results.keys())
    {
        QVERIFY(results.value(test_case));
    }
}

void ImgQSortTestGeneral::testParseTestImagesForSunImage()
{
    CustomDetection customSettings {true,true,true,false};

    QHash<QString, bool> results = testParseTestImages(QLatin1String("sunImage"),
                                                       ImgQSortTest_ParseTestImagesCustomDetection, customSettings);

    for (const auto& test_case : results.keys())
    {
        QVERIFY(results.value(test_case));
    }   
}

void ImgQSortTestGeneral::testParseTestImagesForGeneral_failCase()
{
    QHash<QString, bool> results = testParseTestImages(QLatin1String("failCase"),
                                                       ImgQSortTest_ParseTestImagesDefautDetection, DETECTIONGENERAL);

    for (const auto& test_case : results.keys())
    {
        QEXPECT_FAIL("", "Will fix in the next release", Continue);

        QVERIFY(results.value(test_case));
    }
}
