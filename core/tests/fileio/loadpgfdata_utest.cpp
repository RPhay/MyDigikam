/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-06-09
 * Description : an unit-test to load PGF data and convert to QImage
 *
 * SPDX-FileCopyrightText: 2011-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "loadpgfdata_utest.h"

// Qt includes

#include <QTest>
#include <QFile>
#include <QIODevice>
#include <QImage>

// Local includes

#include "digikam_debug.h"
#include "dtestdatadir.h"
#include "pgfutils.h"

using namespace Digikam;

QTEST_GUILESS_MAIN(LoadPGFDataTest)

LoadPGFDataTest::LoadPGFDataTest(QObject* const parent)
    : QObject(parent)
{
}

void LoadPGFDataTest::testLoadData()
{
    qCDebug(DIGIKAM_TESTS_LOG) << "Using LibPGF version: " << PGFUtils::libPGFVersion();
    QImage img;

    // Write PGF file.

    QString fname = DTestDataDir::TestData(QString::fromUtf8("core/tests/fileio"))
                       .root().path() + QLatin1String("/raw.pgf");
    qCDebug(DIGIKAM_TESTS_LOG) << "Test Data File:" << fname;

    QFile file(fname);

    QVERIFY2(file.open(QIODevice::ReadOnly), "Cannot read Raw PGF stream...");

    QByteArray data(file.size(), '\x00');
    QDataStream stream(&file);
    stream.readRawData(data.data(), data.size());

    // PGF => QImage conversion

    QVERIFY2(PGFUtils::readPGFImageData(data, img), "Failed to read Raw PGF stream...");

    QVERIFY2(img.save(file.fileName() + QString::fromUtf8("-converted.png"), "PNG"), "Cannot save Raw PGF stream with QImage");

    QFile().remove(file.fileName() + QString::fromUtf8("-converted.png"));
}
