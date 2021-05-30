/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-01-12
 * Description : Test for the GeoCoordinates class
 *
 * Copyright (C) 2011 by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef DIGIKAM_GEOCOORDINATES_UTEST_H
#define DIGIKAM_GEOCOORDINATES_UTEST_H

// Qt includes

#include <QTest>

class TestGeoCoordinates : public QObject
{
    Q_OBJECT

public:

    explicit TestGeoCoordinates(QObject* const parent = nullptr)
        : QObject(parent)
    {
    }

private Q_SLOTS:

    void testNoOp();
    void testGeoCoordinates();
    void testMovable();
};

#endif // DIGIKAM_GEOCOORDINATES_UTEST_H
