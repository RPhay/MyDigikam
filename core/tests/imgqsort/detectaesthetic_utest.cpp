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
#include "detectaesthetic_utest.h"

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

QTEST_MAIN(ImgQSortTestDetectQesthetic)

ImgQSortTestDetectQesthetic::ImgQSortTestDetectQesthetic(QObject* const parent)
    : ImgQSortTest(parent)
{
    m_dataTestCases = dataTestCases;
}

void ImgQSortTestDetectQesthetic::testParseTestImagesForAestheticDetection()
{
   QHash<QString, bool> results = testParseTestImages(QLatin1String("aestheticDetection"),
                                                       ImgQSortTest_ParseTestImagesDefautDetection, DETECTAESTHETIC);

    for (const auto& test_case : results.keys())
    {
        QVERIFY(results.value(test_case));
    }
}
