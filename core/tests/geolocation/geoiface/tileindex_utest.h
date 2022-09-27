/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-01-12
 * Description : Test the TileIndex class
 *
 * SPDX-FileCopyrightText: 2011 by Michael G. Hansen <mike at mghansen dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_TILEINDEX_UTEST_H
#define DIGIKAM_TILEINDEX_UTEST_H

// Qt includes

#include <QTest>

class TestTileIndex : public QObject
{
    Q_OBJECT

public:

    explicit TestTileIndex(QObject* const parent = nullptr)
        : QObject(parent)
    {
    }

private Q_SLOTS:

    void testNoOp();
    void testBasics();
    void testIntListInteraction();
    void testResizing();
    void testFromCoordinates();
    void testToCoordinates();
    void testToCoordinatesCorners();
    void testMovable();
};

#endif // DIGIKAM_TILEINDEX_UTEST_H
