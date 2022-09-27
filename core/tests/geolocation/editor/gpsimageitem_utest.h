/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-06-28
 * Description : Test loading and saving of data in GPSItemContainer.
 *
 * SPDX-FileCopyrightText: 2010 by Michael G. Hansen <mike at mghansen dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_GPSIMAGEITEM_UTEST_H
#define DIGIKAM_GPSIMAGEITEM_UTEST_H

// Qt includes

#include <QTest>

// local includes

class TestGPSItemContainer : public QObject
{
    Q_OBJECT

public:

    explicit TestGPSItemContainer(QObject* const parent = nullptr)
        : QObject(parent)
    {
    }

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();
    void testNoOp();
    void testBasicLoading();
};

#endif // DIGIKAM_GPSIMAGEITEM_UTEST_H
