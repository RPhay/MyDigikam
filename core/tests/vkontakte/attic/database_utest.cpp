/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : Unit tests for VKontakte web service
 *
 * Copyright (C) 2011-2015 by Alexander Potashev <aspotashev at gmail dot com>
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "database_utest.h"

// Qt includes

#include <QTest>

// Local includes

#include "vkontakte_cidsnamesjob.h"

using namespace Vkontakte;

TestDatabaseMethods::TestDatabaseMethods()
    : VkTestBase()
{
}

void TestDatabaseMethods::initTestCase()
{
}

void TestDatabaseMethods::testCountriesById()
{
    CidsNamesJob* const job = new CidsNamesJob("database.getCountriesById", QString(), QIntList() << 1 << 2 << 3);
    job->exec();
    QVERIFY(!job->error());

    QMap<int, QString> names = job->names();
    QCOMPARE(names.size(), 3);
    QCOMPARE(names[1], QString::fromUtf8("Россия"));   // Russia
    QCOMPARE(names[2], QString::fromUtf8("Украина"));  // Ukraine
    QCOMPARE(names[3], QString::fromUtf8("Беларусь")); // Belarus
}

void TestDatabaseMethods::testCitiesById()
{
    CidsNamesJob* const job = new CidsNamesJob("database.getCitiesById", QString(), QIntList() << 1 << 2 << 4);
    job->exec();
    QVERIFY(!job->error());

    QMap<int, QString> names = job->names();
    QCOMPARE(names.size(), 3);
    QCOMPARE(names[1], QString::fromUtf8("Москва"));          // Moscow
    QCOMPARE(names[2], QString::fromUtf8("Санкт-Петербург")); // Saint-Petersburg
    QCOMPARE(names[4], QString::fromUtf8("Herzliya"));
}

QTEST_MAIN(TestDatabaseMethods)
