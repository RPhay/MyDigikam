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

#include "albums_utest.h"

// Qt includes

#include <QTest>

// Local includes

#include "vkontakte_createalbumjob.h"
#include "vkontakte_albumlistjob.h"
#include "vkontakte_editalbumjob.h"
#include "vkontakte_deletealbumjob.h"

#define ALBUM1_NAME (QLatin1String("__album for unit testing of digiKam #1"))
#define ALBUM2_NAME (QLatin1String("__album for unit testing of digiKam #2"))

using namespace Vkontakte;

TestAlbums::TestAlbums()
    : VkTestBase()
{
}

void TestAlbums::initTestCase()
{
    authenticate(Vkontakte::AppPermissions::Photos);

    // Create albums for testing
    QList<QString> albumNames;
    albumNames << ALBUM1_NAME;
    albumNames << ALBUM2_NAME;

    foreach (const QString& name, albumNames)
    {
        CreateAlbumJob* const job = new CreateAlbumJob(accessToken(),
                                                       name,
                                                       QString::fromUtf8("Description for %1").arg(name),
                                                       AlbumInfo::PRIVACY_PRIVATE, AlbumInfo::PRIVACY_PRIVATE);
        job->exec();
        QVERIFY(!job->error());

        m_albumIds.append(job->album().aid());
    }
}

void TestAlbums::testListJob()
{
    AlbumListJob* const job = new AlbumListJob(accessToken());
    job->exec();
    QVERIFY(!job->error());

    QList<AlbumInfo> list = job->list();

    // We created two albums but could remove only one in testDeleteJob(),
    // so at least one album should exist at the moment.
    QVERIFY(list.size() >= 1);
}

void TestAlbums::testEditJob()
{
    const int albumId = m_albumIds[0];
    const QString newTitle(QStringLiteral("what's up?"));
    const QString newDescription(QLatin1String("my description"));

    // Change album title, description and permissions
    {
        EditAlbumJob* const job = new EditAlbumJob(accessToken(),
                                                   albumId,
                                                   newTitle,
                                                   newDescription,
                                                   AlbumInfo::PRIVACY_PUBLIC, AlbumInfo::PRIVACY_FRIENDS);
        job->exec();
        QVERIFY(!job->error());
    }

    // Verify that album properties have changed
    AlbumListJob* const listJob = new AlbumListJob(accessToken(), -1, QList<int>() << albumId);
    listJob->exec();
    QVERIFY(!listJob->error());

    QList<AlbumInfo> list = listJob->list();
    QCOMPARE(list.size(), 1);

    const AlbumInfo album = list.at(0);
    QCOMPARE(album.aid(), albumId);
    QCOMPARE(album.title(), newTitle);
    QCOMPARE(album.description(), newDescription);
    QCOMPARE(album.privacy(), static_cast<int>(AlbumInfo::PRIVACY_PUBLIC));

    // Privacy mode for comments is not returned by the VK server,
    // so it is not our fault.
//     QCOMPARE(album.commentPrivacy(), static_cast<int>(AlbumInfo::PRIVACY_FRIENDS));
}

void TestAlbums::testDeleteJob()
{
    const int albumId = m_albumIds[1];

    // Delete album
    {
        DeleteAlbumJob* const job = new DeleteAlbumJob(accessToken(), albumId);
        job->exec();
        QVERIFY(!job->error());
    }

    // Verify that the album does not exist anymore
    AlbumListJob* const listJob = new AlbumListJob(accessToken(), -1, QList<int>() << albumId);
    listJob->exec();
    QVERIFY(!listJob->error());

    QList<AlbumInfo> list = listJob->list();
    QCOMPARE(list.size(), 0);
}

QTEST_MAIN(TestAlbums)
