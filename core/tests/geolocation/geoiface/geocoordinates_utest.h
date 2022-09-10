/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-01-12
 * Description : Test for the GeoCoordinates class
 *
 * SPDX-FileCopyrightText: 2011 by Michael G. Hansen <mike at mghansen dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
