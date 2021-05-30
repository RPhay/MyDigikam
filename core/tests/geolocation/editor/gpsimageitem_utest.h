/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-06-28
 * Description : Test loading and saving of data in GPSItemContainer.
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
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
