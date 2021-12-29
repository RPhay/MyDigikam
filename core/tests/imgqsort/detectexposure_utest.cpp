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

#include "detectexposure_utest.h"

// Qt includes

#include <QTest>
#include <QStringList>
#include <QDebug>

// Local includes

#include "imgqsorttest.h"

using namespace Digikam;

QTEST_MAIN(ImgQSortTestDetecteExposure)

ImgQSortTestDetecteExposure::ImgQSortTestDetecteExposure(QObject* const parent)
    : ImgQSortTest(parent)
{
    m_dataTestCases = dataTestCases;
}

void ImgQSortTestDetecteExposure::testParseTestImagesForExposureDetection()
{
    QHash<QString, bool> results = testParseTestImages(QLatin1String("exposureDetection"),
                                                       ImgQSortTest_ParseTestImagesDefautDetection, DETECTEXPOSURE);

    for (const auto& test_case : results.keys())
    {
        QVERIFY(results.value(test_case));
    }
}

void ImgQSortTestDetecteExposure::testParseTestImagesForExposureDetection_backlight()
{
    QHash<QString, bool> results = testParseTestImages(QLatin1String("exposureBacklight"),
                                                       ImgQSortTest_ParseTestImagesDefautDetection, DETECTEXPOSURE);

    for (const auto& test_case : results.keys())
    {
        QVERIFY(results.value(test_case));
    }
}

void ImgQSortTestDetecteExposure::testParseTestImagesForExposureDetection_sun()
{
    QHash<QString, bool> results = testParseTestImages(QLatin1String("exposureSun"),
                                                       ImgQSortTest_ParseTestImagesDefautDetection, DETECTEXPOSURE);

    for (const auto& test_case : results.keys())
    {
        QVERIFY(results.value(test_case));
    }
}
