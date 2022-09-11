/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-05-12
 * Description : Test parsing rg data.
 *
 * SPDX-FileCopyrightText: 2010 by Michael G. Hansen <mike at mghansen dot de>
 * SPDX-FileCopyrightText: 2010 by Gabriel Voicu <ping dot gabi at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_RGPARSING_UTEST_H
#define DIGIKAM_RGPARSING_UTEST_H

// Qt includes

#include <QTest>

class TestRGParsing : public QObject
{
    Q_OBJECT

public:

    explicit TestRGParsing(QObject* const parent = nullptr)
        : QObject(parent)
    {
    }

private Q_SLOTS:

    void testNoOp();
    void testVerifyRG();
};

#endif // DIGIKAM_RGPARSING_UTEST_H
