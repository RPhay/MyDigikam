/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-06-29
 * Description : Test the functions for dealing with DatabaseFields
 *
 * SPDX-FileCopyrightText: 2013 by Michael G. Hansen <mike at mghansen dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DATABASE_FIELDS_UTEST_H
#define DIGIKAM_DATABASE_FIELDS_UTEST_H

// Qt includes

#include <QTest>

class DatabaseFieldsTest : public QObject
{
    Q_OBJECT

public:

    explicit DatabaseFieldsTest(QObject* const parent = nullptr)
        : QObject(parent)
    {
    }

private Q_SLOTS:

    void testMinSizeType();
    void testIterators();
    void testMetaInfo();
    void testIteratorsSetOnly();
    void testSet();
    void testSetHashAddSets();
    void testHashRemoveAll();

    void initTestCase();
    void cleanupTestCase();
};

#endif // DIGIKAM_DATABASE_FIELDS_UTEST_H
