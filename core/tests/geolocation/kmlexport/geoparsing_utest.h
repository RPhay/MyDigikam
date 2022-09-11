/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-01-17
 * Description : test parsing gpx data
 *
 * SPDX-FileCopyrightText: 2010      by Michael G. Hansen <mike at mghansen dot de>
 * SPDX-FileCopyrightText: 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_GEOPARSING_UTEST_H
#define DIGIKAM_GEOPARSING_UTEST_H

// Qt includes

#include <QTest>

class TestGPXParsing : public QObject
{
    Q_OBJECT

public:

    explicit TestGPXParsing(QObject* const parent = nullptr)
        : QObject(parent)
    {
    }

private Q_SLOTS:

    void testQDateTimeParsing();
    void testCustomParsing();
};

#endif // DIGIKAM_GEOPARSING_UTEST_H
