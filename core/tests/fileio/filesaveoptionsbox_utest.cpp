/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-06
 * Description : test for the filesaveoptionsbox
 *
 * Copyright (C) 2009 by Johannes Wienke <languitar at semipol dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "filesaveoptionsbox_utest.h"

// Qt includes

#include <QTest>
#include <QDir>

// Local includes

#include "filesaveoptionsbox.h"
#include "dpluginloader.h"

using namespace Digikam;

QTEST_MAIN(FileSaveOptionsBoxTest)

FileSaveOptionsBoxTest::FileSaveOptionsBoxTest(QObject* const parent)
    : QObject(parent)
{
}

void FileSaveOptionsBoxTest::testDiscoverFormat_data()
{
    QTest::addColumn<QString>("filename");
    QTest::addColumn<int>("format");

    QTest::newRow("jpg") << "test.jpg" << (int) FileSaveOptionsBox::JPEG;
    QTest::newRow("jpeg") << "test.jpeg" << (int) FileSaveOptionsBox::JPEG;
    QTest::newRow("JPG") << "test.JPG" << (int) FileSaveOptionsBox::JPEG;
    QTest::newRow("jpg") << "jpg" << (int) FileSaveOptionsBox::JPEG;
    QTest::newRow("jpeg") << "jpeg" << (int) FileSaveOptionsBox::JPEG;

    QTest::newRow("bla.tiff.jpeg") << "bla.tiff.jpeg" << (int) FileSaveOptionsBox::JPEG;
    QTest::newRow("bla.jpg.tiff") << "bla.jpg.tiff" << (int) FileSaveOptionsBox::TIFF;

#ifdef HAVE_JASPER
    QTest::newRow("bla.png.jpeg.pgx") << "bla.png.jpeg.pgx" << (int) FileSaveOptionsBox::JP2K;
#endif // HAVE_JASPER

#ifdef HAVE_X265
    QTest::newRow("bla.png.jpeg.heic") << "bla.png.jpeg.heic" << (int) FileSaveOptionsBox::HEIF;
#endif // HAVE_X265

    QTest::newRow("pgf") << "PGF" << (int) FileSaveOptionsBox::PGF;

    QTest::newRow("unknwon") << "i.dont.know" << (int) FileSaveOptionsBox::NONE; // krazy:exclude=spelling
}

void FileSaveOptionsBoxTest::testDiscoverFormat()
{
    QFETCH(QString, filename);
    QFETCH(int, format);

    FileSaveOptionsBox box;
    QCOMPARE((int) box.discoverFormat(filename), format);
}

void FileSaveOptionsBoxTest::testDiscoverFormatDefault()
{
    FileSaveOptionsBox box;
    QCOMPARE(box.discoverFormat(QLatin1String("unknown")), FileSaveOptionsBox::NONE);
    QCOMPARE(box.discoverFormat(QLatin1String("unknown"), FileSaveOptionsBox::PGF), FileSaveOptionsBox::PGF);
}

void FileSaveOptionsBoxTest::initTestCase()
{
    QDir dir(qApp->applicationDirPath());
    qputenv("DK_PLUGIN_PATH", dir.canonicalPath().toUtf8());
    DPluginLoader::instance()->init();
}

void FileSaveOptionsBoxTest::cleanupTestCase()
{
    DPluginLoader::instance()->cleanUp();
}
