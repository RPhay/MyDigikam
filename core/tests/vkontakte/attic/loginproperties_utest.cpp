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

#include "loginproperties_utest.h"

// Qt includes

#include <QTest>

// Local includes

#include "vkontakte_getapplicationpermissionsjob.h"
#include "vkontakte_getvariablejob.h"
#include "vkontakte_userinfojob.h"

using namespace Vkontakte;

const AppPermissions::Value testPermissions = AppPermissions::Photos | AppPermissions::Offline |
                                              AppPermissions::Notes  | AppPermissions::Messages;

TestLoginProperties::TestLoginProperties()
    : VkTestBase()
{
}

void TestLoginProperties::initTestCase()
{
    authenticate(testPermissions);
}

void TestLoginProperties::testGetApplicationPermissionsJob()
{
    GetApplicationPermissionsJob* const job = new GetApplicationPermissionsJob(accessToken());
    job->exec();
    QVERIFY(!job->error());

    QCOMPARE(job->permissions(), static_cast<int>(testPermissions));
}

void TestLoginProperties::testGetVariableJob()
{
    // Retrieve user info with UserInfoJob
    Vkontakte::UserInfoJob* const job1 = new Vkontakte::UserInfoJob(accessToken());
    job1->exec();
    QVERIFY(!job1->error());

    QList<UserInfoPtr> res = job1->userInfo();
    QCOMPARE(res.size(), 1);

    const UserInfoPtr user = res.at(0);

    // Test GetVariableJob
    foreach (int index, QList<int>() << 1280 << 1281)
    {
        GetVariableJob* const job2 = new GetVariableJob(accessToken(), index);
        job2->exec();
        QVERIFY(!job2->error());

        int type = static_cast<int>(job2->variable().type());

        if (index == 1280)
        {
            QCOMPARE(type, static_cast<int>(QMetaType::ULongLong));
            QCOMPARE(job2->variable().toInt(), user->uid());
        }
        else
        {
            // 1281
            QCOMPARE(type, static_cast<int>(QMetaType::QString));
            QCOMPARE(job2->variable().toString(), QString::fromUtf8("%1 %2").arg(user->firstName()).arg(user->lastName()));
        }
    }
}

QTEST_MAIN(TestLoginProperties)
