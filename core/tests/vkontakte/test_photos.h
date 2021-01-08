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

#ifndef DIGIKAM_VKONTAKTE_TEST_PHOTOS_H
#define DIGIKAM_VKONTAKTE_TEST_PHOTOS_H

// Qt includes

#include <QObject>
#include <QVector>

// Local includes

#include "vktestbase.h"

/**
 * What is tested here:
 *   class UploadPhotosJob - tested in initTestCase()
 *   class PhotoListJob
 *   class PhotoJob - tested in testPhotoListJob()
 */
class TestPhotos : public VkTestBase
{
    Q_OBJECT

public:

    TestPhotos();

private Q_SLOTS:

    void initTestCase();
    void testPhotoListJob();
    void testUploadMultiple();

private:

    int m_albumId;
    int m_photoId;
};

#endif // DIGIKAM_VKONTAKTE_TEST_PHOTOS_H
