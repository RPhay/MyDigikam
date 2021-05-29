/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-07-12
 * Description : An unit-test to print all available metadata tags provided by Exiv2.
 *
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "printtagslist_utest.h"

// Qt includes

#include <QTextStream>

QTEST_MAIN(PrintTagsListTest)

PrintTagsListTest::PrintTagsListTest(QObject* const parent)
    : AbstractUnitTest(parent)
{
}

void PrintTagsListTest::parseTagsList(const DMetadata::TagsMap& tags)
{
    QString output;
    QTextStream stream(&output);
    stream << endl;

    qCDebug(DIGIKAM_TESTS_LOG) << "Found" << tags.size() << "tags:" << endl;

    for (DMetadata::TagsMap::const_iterator it = tags.constBegin() ; it != tags.constEnd() ; ++it )
    {
        QString     key    = it.key();
        QStringList values = it.value();
        QString     name   = values[0];
        QString     title  = values[1];
        QString     desc   = values[2];

        // None of these strings can be null, event if strings are translated.
        QVERIFY(!key.isNull());
        QVERIFY(!name.isNull());
        QVERIFY(!title.isNull());
        QVERIFY(!desc.isNull());

        stream << key << endl
               << "    " << name  << endl
               << "    " << title << endl
               << "    " << desc  << endl;
    }

    qCDebug(DIGIKAM_TESTS_LOG).noquote() << output;
}

void PrintTagsListTest::testPrintAllAvailableStdExifTags()
{
    QScopedPointer<DMetadata> meta(new DMetadata);

    qCDebug(DIGIKAM_TESTS_LOG) << "-- Standard Exif Tags -------------------------------------------------------------";

    DMetadata::TagsMap exiftags = meta->getStdExifTagsList();
    QVERIFY(!exiftags.isEmpty());

    parseTagsList(exiftags);
}

void PrintTagsListTest::testPrintAllAvailableMakernotesTags()
{
    QScopedPointer<DMetadata> meta(new DMetadata);

    qCDebug(DIGIKAM_TESTS_LOG) << "-- Makernote Tags -----------------------------------------------------------------";

    DMetadata::TagsMap mntags = meta->getMakernoteTagsList();

    QVERIFY(!mntags.isEmpty());

    parseTagsList(mntags);
}

void PrintTagsListTest::testPrintAllAvailableIptcTags()
{
    QScopedPointer<DMetadata> meta(new DMetadata);

    qCDebug(DIGIKAM_TESTS_LOG) << "-- Standard Iptc Tags -----------------------------------------------------------------";

    DMetadata::TagsMap iptctags = meta->getIptcTagsList();

    QVERIFY(!iptctags.isEmpty());

    parseTagsList(iptctags);
}

void PrintTagsListTest::testPrintAllAvailableXmpTags()
{
    QScopedPointer<DMetadata> meta(new DMetadata);

    qCDebug(DIGIKAM_TESTS_LOG) << "-- Standard Xmp Tags -----------------------------------------------------------------";

    DMetadata::TagsMap xmptags = meta->getXmpTagsList();

    if (meta->supportXmp())
    {
        QVERIFY(!xmptags.isEmpty());

        parseTagsList(xmptags);
    }
    else
    {
        QWARN("Exiv2 has no XMP support...");
    }
}
