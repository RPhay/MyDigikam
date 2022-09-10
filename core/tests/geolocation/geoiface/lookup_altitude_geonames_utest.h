/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-05-07
 * Description : Test for the geonames based altitude lookup class
 *
 * SPDX-FileCopyrightText: 2010-2011 by Michael G. Hansen <mike at mghansen dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_LOOKUP_ALTITUDE_GEONAMES_UTEST_H
#define DIGIKAM_LOOKUP_ALTITUDE_GEONAMES_UTEST_H

// Qt includes

#include <QTest>

class TestLookupAltitudeGeonames : public QObject
{
    Q_OBJECT

public:

    explicit TestLookupAltitudeGeonames(QObject* const parent = nullptr)
        : QObject(parent)
    {
    }

private Q_SLOTS:

    void testNoOp();
    void testSimpleLookup();
};

#endif // DIGIKAM_LOOKUP_ALTITUDE_GEONAMES__UTESTH
