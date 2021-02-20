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

#include "imgqsorttest.h"
#include "imgqsorttest_shared.h"

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

QTEST_MAIN(ImgQSortTest)


ImgQSortTest::ImgQSortTest(QObject* const)
{
}

void ImgQSortTest::initTestCase()
{
    QDir dir(QFINDTESTDATA("../../dplugins/dimg"));
    qputenv("DK_PLUGIN_PATH", dir.canonicalPath().toUtf8());
    DPluginLoader::instance()->init();
}

void ImgQSortTest::cleanupTestCase()
{
}

QDir ImgQSortTest::imageDir() const
{
    QDir dir(QFINDTESTDATA("data/"));
    qDebug() << "Images Directory:" << dir;
    return dir;
}

void ImgQSortTest::testParseTestImagesForExposureDetection()
{
    QFileInfoList list = imageDir().entryInfoList(QStringList() << QLatin1String("test_overexposed*.jpg"),
                                                  QDir::Files, QDir::Name);

    list += imageDir().entryInfoList(QStringList() << QLatin1String("test_underexposed*.jpg"),
                                     QDir::Files, QDir::Name);

    QMultiMap<int, QString> results = ImgQSortTest_ParseTestImages(DetectExposure, list);

    QVERIFY(results.count(NoPickLabel)   == 0);
    QVERIFY(results.count(RejectedLabel) == 0);
    QVERIFY(results.count(PendingLabel)  == 6);
    QVERIFY(results.count(AcceptedLabel) == 12);
}

void ImgQSortTest::testParseTestImagesForNoiseDetection()
{
    QFileInfoList list = imageDir().entryInfoList(QStringList() << QLatin1String("test_noised*.jpg"),
                                                  QDir::Files, QDir::Name);

    QMultiMap<int, QString> results = ImgQSortTest_ParseTestImages(DetectNoise, list);

/*  FIXME: Algorithm do not work yet
    QVERIFY(results.count(NoPickLabel)   == 0);
    QVERIFY(results.count(RejectedLabel) == 1);
    QVERIFY(results.count(PendingLabel)  == 8);
    QVERIFY(results.count(AcceptedLabel) == 0);
*/
}

void ImgQSortTest::testParseTestImagesForBlurDetection()
{
    QFileInfoList list = imageDir().entryInfoList(QStringList() << QLatin1String("test_blurred*.jpg"),
                                                  QDir::Files, QDir::Name);

    QMultiMap<int, QString> results = ImgQSortTest_ParseTestImages(DetectBlur, list);

/*  FIXME: Algorithm do not work yet
    QVERIFY(results.count(NoPickLabel)   == 0);
    QVERIFY(results.count(RejectedLabel) == 1);
    QVERIFY(results.count(PendingLabel)  == 8);
    QVERIFY(results.count(AcceptedLabel) == 0);
*/
}

void ImgQSortTest::testParseTestImagesForCompressionDetection()
{
    QFileInfoList list = imageDir().entryInfoList(QStringList() << QLatin1String("test_compressed*.jpg"),
                                                  QDir::Files, QDir::Name);

    QMultiMap<int, QString> results = ImgQSortTest_ParseTestImages(DetectCompression, list);

/*  FIXME: Algorithm do not work yet
    QVERIFY(results.count(NoPickLabel)   == 9);
    QVERIFY(results.count(RejectedLabel) == 0);
    QVERIFY(results.count(PendingLabel)  == 0);
    QVERIFY(results.count(AcceptedLabel) == 0);
*/
}
