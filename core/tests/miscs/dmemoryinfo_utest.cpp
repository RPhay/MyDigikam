/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-25
 * Description : a test for the DMemoryInfo
 *
 * Copyright (C) 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
