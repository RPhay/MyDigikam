/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : an unit-test to detect image quality level
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021      by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
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

int    s_newArgc = 0;                                                                                                           \
char** s_newArgv = nullptr;                                                                                                     \

int main(int argc, char* argv[])
{
    // This test generate more than 2000 warnings on the console.
    // This code is a hack to disable warnings to prevent breaking workflow with CI/CD on gitlab.

    int i     = 0;                                                                                                      \
    s_newArgc = argc + 2;                                                                                               \
    s_newArgv = new char*[s_newArgc];                                                                                   \
                                                                                                                        \
    for (i = 0 ; i < argc ; ++i)                                                                                        \
    {                                                                                                                   \
        s_newArgv[i] = new char[strlen(argv[i]) + 1];                                                                   \
        strcpy(s_newArgv[i], argv[i]);                                                                                  \
    }                                                                                                                   \
                                                                                                                        \
    const char* const headLessOpt1 = "-maxwarnings";                                                                       \
    s_newArgv[i]                   = new char[strlen(headLessOpt1) + 1];                                                \
    strcpy(s_newArgv[i], headLessOpt1);                                                                                 \
                                                                                                                        \
    const char* const headLessOpt2 = "0";                                                                       \
    s_newArgv[i + 1]               = new char[strlen(headLessOpt2) + 1];                                                \
    strcpy(s_newArgv[i + 1], headLessOpt2);                                                                             \
                                                                                                                        \
    argc = s_newArgc;
    argv = s_newArgv;                                                                      \

    QTEST_MAIN_IMPL(ImgQSortTestGeneral)
}

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

    if (!m_hasExifTool)
    {
        m_dataTestCases.remove(QLatin1String("sunImage"),
                               PairImageQuality(QLatin1String("exposure_sun_1.jpg"), 1));
        m_dataTestCases.insert(QLatin1String("sunImage"),
                               PairImageQuality(QLatin1String("exposure_sun_1.jpg"), 2));
    }

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
