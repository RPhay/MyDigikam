/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-02-17
 * Description : an unit-test to convert RAW file to DNG
 *
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "raw2dng_utest.h"

// Qt includes

#include <QApplication>
#include <QTest>
#include <QFileInfo>
#include <QDir>

// Local includes

#include "digikam_debug.h"
#include "metaengine.h"
#include "dtestdatadir.h"
#include "dngwriter.h"
#include "wstoolutils.h"

using namespace Digikam;

QTEST_GUILESS_MAIN(Raw2DngTest)

Raw2DngTest::Raw2DngTest(QObject* const parent)
    : QObject(parent)
{
}

void Raw2DngTest::testRaw2Dng()
{
    MetaEngine::initializeExiv2();

    QString fname = DTestDataDir::TestData(QString::fromUtf8("core/tests/metadataengine"))
                    .root().path() + QLatin1String("/IMG_2520.CR2");
    qCDebug(DIGIKAM_TESTS_LOG) << "Test Data File:" << fname;

    QString tmpPath   = QString::fromLatin1(QTest::currentAppName());
    tmpPath.replace(QLatin1String("./"), QString());
    QDir tmpDir   = WSToolUtils::makeTemporaryDir(tmpPath.toLatin1().data());
    qCDebug(DIGIKAM_TESTS_LOG) << "Temporary Dir :" << tmpDir.path();

    DNGWriter dngProcessor;
    dngProcessor.setInputFile(fname);
    QString path = tmpDir.filePath(QFileInfo(fname).fileName().trimmed());
    dngProcessor.setOutputFile(tmpDir.path() + QString::fromLatin1("/%1").arg(path));
    QVERIFY2(dngProcessor.convert(), "Cannot convert RAW to DNG");
}
