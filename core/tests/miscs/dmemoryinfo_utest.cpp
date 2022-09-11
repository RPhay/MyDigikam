/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-25
 * Description : a test for the DMemoryInfo
 *
 * SPDX-FileCopyrightText: 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dmemoryinfo_utest.h"

// Qt includes

#include <QTest>

// Local includes

#include "dmemoryinfo.h"

using namespace Digikam;

QTEST_GUILESS_MAIN(DMemoryInfoTest)

DMemoryInfoTest::DMemoryInfoTest(QObject* const parent)
    : QObject(parent)
{
}

void DMemoryInfoTest::isNull()
{
    DMemoryInfo m;

    QVERIFY(!m.isNull());
}

void DMemoryInfoTest::operators()
{
    DMemoryInfo m;
    auto m1 = m;

    QVERIFY(m == m1);
    QVERIFY(m.totalPhysical() != 0);
    QCOMPARE(m.totalPhysical(), m1.totalPhysical());     // cppcheck-suppress knownConditionTrueFalse
}
