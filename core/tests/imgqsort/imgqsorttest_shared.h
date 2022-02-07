/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : an unit-test to detect image quality level
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_IMGQSORT_TEST_SHARED_H
#define DIGIKAM_IMGQSORT_TEST_SHARED_H

// Qt includes

#include <QMultiMap>
#include <QString>
#include <QFileInfoList>
#include <QObject>
#include <QDir>
#include <QTest>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"

namespace Digikam
{

enum DetectionType
{
    DETECTBLUR = 0,
    DETECTNOISE,
    DETECTCOMPRESSION,
    DETECTEXPOSURE,
    DETECTIONGENERAL
};

struct CustomDetection
{
    bool detectBlur;
    bool detectNoise;
    bool detectExposure;
    bool detectCompression;
};

QHash<QString, int> ImgQSortTest_ParseTestImagesDefautDetection(DetectionType type, const QFileInfoList& list);

QHash<QString, int> ImgQSortTest_ParseTestImagesCustomDetection(const CustomDetection& customSetting, const QFileInfoList& list);

} // namespace Digikam

#endif // DIGIKAM_IMGQSORT_TEST_SHARED_H
