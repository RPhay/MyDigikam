/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : an unit-test to detect image quality level
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
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

#ifndef DIGIKAM_DETECT_GENERAL_UTEST_H
#define DIGIKAM_DETECT_GENERAL_UTEST_H

// Qt includes

#include <QObject>
#include <QDir>
#include <QMultiMap>
#include <QString>

// Local includes

#include "imgqsorttest.h"

class ImgQSortTestGeneral : public ImgQSortTest
{
    Q_OBJECT

public:

    explicit ImgQSortTestGeneral(QObject* const parent = nullptr);

private Q_SLOTS:

    void testParseTestImagesForGeneralBadImage();
    void testParseTestImagesForGeneralQuiteBadImage();
    void testParseTestImagesForGeneralNormalImage();
    void testParseTestImagesForGeneralQuiteGoodImage();
    void testParseTestImagesForGeneralGoodImage();
    void testParseTestImagesForSunImage();
    void testParseTestImagesForGeneral_failCase();
};

DataTestCases const dataTestCases =
{
    {QLatin1String("badImage"),         PairImageQuality(QLatin1String("general_bad_image_1.jpg"),1)},
    {QLatin1String("badImage"),         PairImageQuality(QLatin1String("general_bad_image_3.jpg"),1)},
    {QLatin1String("badImage"),         PairImageQuality(QLatin1String("general_bad_image_5.jpg"),1)},
    {QLatin1String("badImage"),         PairImageQuality(QLatin1String("general_bad_image_6.jpg"),1)},


    {QLatin1String("quiteBadImage"),    PairImageQuality(QLatin1String("general_quite_bad_image_1.jpg"),1)},
    {QLatin1String("quiteBadImage"),    PairImageQuality(QLatin1String("general_quite_bad_image_2.jpg"),1)},
    {QLatin1String("quiteBadImage"),    PairImageQuality(QLatin1String("general_quite_bad_image_3.jpg"),1)},
    {QLatin1String("quiteBadImage"),    PairImageQuality(QLatin1String("general_quite_bad_image_4.jpg"),1)},
    {QLatin1String("quiteBadImage"),    PairImageQuality(QLatin1String("general_quite_bad_image_5.jpg"),1)},
    {QLatin1String("quiteBadImage"),    PairImageQuality(QLatin1String("general_quite_bad_image_6.jpg"),1)},

    {QLatin1String("normalImage"),      PairImageQuality(QLatin1String("general_normal_image_1.jpg"),2)},
    {QLatin1String("normalImage"),      PairImageQuality(QLatin1String("general_normal_image_2.jpg"),2)},
    {QLatin1String("normalImage"),      PairImageQuality(QLatin1String("general_normal_image_3.jpg"),2)},
    {QLatin1String("normalImage"),      PairImageQuality(QLatin1String("general_normal_image_4.jpg"),2)},
    {QLatin1String("normalImage"),      PairImageQuality(QLatin1String("general_normal_image_5.jpg"),2)},
    {QLatin1String("normalImage"),      PairImageQuality(QLatin1String("general_normal_image_6.jpg"),2)},

    {QLatin1String("quiteGoodImage"),   PairImageQuality(QLatin1String("general_quite_good_image_2.jpg"),3)},
    {QLatin1String("quiteGoodImage"),   PairImageQuality(QLatin1String("general_quite_good_image_3.jpg"),3)},
    {QLatin1String("quiteGoodImage"),   PairImageQuality(QLatin1String("general_quite_good_image_4.jpg"),3)},
    {QLatin1String("quiteGoodImage"),   PairImageQuality(QLatin1String("general_quite_good_image_5.jpg"),3)},

    {QLatin1String("goodImage"),        PairImageQuality(QLatin1String("general_good_image_1.jpg"),3)},
    {QLatin1String("goodImage"),        PairImageQuality(QLatin1String("general_good_image_2.jpg"),3)},
    {QLatin1String("goodImage"),        PairImageQuality(QLatin1String("general_good_image_3.jpg"),3)},
    {QLatin1String("goodImage"),        PairImageQuality(QLatin1String("general_good_image_4.jpg"),3)},
    {QLatin1String("goodImage"),        PairImageQuality(QLatin1String("general_good_image_5.jpg"),3)},
    {QLatin1String("goodImage"),        PairImageQuality(QLatin1String("general_good_image_6.jpg"),3)},

    {QLatin1String("sunImage"),         PairImageQuality(QLatin1String("exposure_sun_1.jpg"),2)},
    {QLatin1String("sunImage"),         PairImageQuality(QLatin1String("exposure_sun_2.jpg"),2)},
    {QLatin1String("sunImage"),         PairImageQuality(QLatin1String("exposure_sun_3.jpg"),2)},

    {QLatin1String("failCase"),         PairImageQuality(QLatin1String("general_bad_image_2.jpg"),1)},
    {QLatin1String("failCase"),         PairImageQuality(QLatin1String("general_bad_image_4.jpg"),1)},
    {QLatin1String("failCase"),         PairImageQuality(QLatin1String("general_quite_good_image_1.jpg"),3)},
    {QLatin1String("failCase"),         PairImageQuality(QLatin1String("general_quite_good_image_6.jpg"),3)},
};

#endif // DIGIKAM_DETECT_GENERAL_UTEST_H
