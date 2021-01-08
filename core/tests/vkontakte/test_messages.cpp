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

#include "test_messages.h"

// KDE includes

#include <qtest_kde.h>

// Local includes

#include "vkontakte_messageslistjob.h"
#include "vkontakte_allmessageslistjob.h"
#include "vkontakte_discussionslistjob.h"

using namespace Vkontakte;

TestMessages::TestMessages()
    : VkTestBase()
{
}

void TestMessages::initTestCase()
{
    authenticate(AppPermissions::Messages);
}

void TestMessages::testMessagesListJob()
{
    MessagesListJob* const job = new MessagesListJob(accessToken());
    job->exec();
    QVERIFY(!job->error());
}

void TestMessages::testAllMessagesListJob()
{
    AllMessagesListJob* const job = new AllMessagesListJob(accessToken());
    job->exec();
    QVERIFY(!job->error());
}

void TestMessages::testDiscussionsListJob()
{
    DiscussionsListJob* const job = new DiscussionsListJob(accessToken());
    job->exec();
    QVERIFY(!job->error());
}

QTEST_KDEMAIN(TestMessages, GUI)
