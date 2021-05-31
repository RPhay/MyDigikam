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

#include "photos_utest.h"

// Qt incudes

#include <QTest>
#include <QImage>

// Local includes

#include "vkontakte_uploadphotosjob.h"
#include "vkontakte_photolistjob.h"
#include "vkontakte_photojob.h"
#include "vkontakte_createalbumjob.h"

#define ALBUM1_NAME (QLatin1String("__album for unit testing of digiKam #1"))

using namespace Vkontakte;

TestPhotos::TestPhotos()
    : VkTestBase(),
      m_albumId(-1),
      m_photoId(-1)
{
}

void TestPhotos::initTestCase()
{
    authenticate(AppPermissions::Photos);

    // Create album for testing

    CreateAlbumJob* const albumJob = new CreateAlbumJob(accessToken(),
                                                        ALBUM1_NAME,
                                                        QString::fromUtf8("Description for %1").arg(ALBUM1_NAME),
                                                        AlbumInfo::PRIVACY_PRIVATE, AlbumInfo::PRIVACY_PRIVATE);
    albumJob->exec();
    QVERIFY(!albumJob->error());

    m_albumId = albumJob->album().aid();

    // Upload photo for testing

    UploadPhotosJob* const job = new UploadPhotosJob(accessToken(),
                                                     QStringList() << QLatin1String(AUTOTESTS_DATA_DIR) + QLatin1String("/image1.png"),
                                                     true, m_albumId);
    job->exec();
    QVERIFY(!job->error());

    QList<PhotoInfo> list = job->list();
    QCOMPARE(list.size(), 1);

    const PhotoInfo photo = list.at(0);
    m_photoId             = photo.pid();
}

void TestPhotos::testPhotoListJob()
{
    PhotoListJob* const job = new PhotoListJob(accessToken(), 0, m_albumId, QList<int>() << m_photoId);
    job->exec();
    QVERIFY(!job->error());

    QList<PhotoInfo> list = job->list();
    QCOMPARE(list.size(), 1);

    const PhotoInfo photo = list.at(0);
    QCOMPARE(photo.pid(), m_photoId);

    // Download the image

    PhotoJob* const photoJob = new PhotoJob(photo.urlMaxResolution());
    photoJob->exec();
    QVERIFY(!photoJob->error());

    QImage image = photoJob->photo();
    QCOMPARE(image.size(), QSize(10, 10));
}

void TestPhotos::testUploadMultiple()
{
    // Upload multiple photos at once
    const int count = 10;

    QStringList files;

    for (int i = 0 ; i < count ; ++i)
    {
        files << QStringLiteral(AUTOTESTS_DATA_DIR) + QLatin1String("/image1.png");
    }

    UploadPhotosJob* const job = new UploadPhotosJob(accessToken(), files, true, m_albumId);
    job->exec();
    QVERIFY(!job->error());

    QList<PhotoInfo> list = job->list();
    QCOMPARE(list.size(), count);
}

QTEST_MAIN(TestPhotos)
