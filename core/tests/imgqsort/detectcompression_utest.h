/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-04
 * Description : an unit-test to detect image quality level
 *
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_IMGQSORT_TEST_DETECT_COMPRESSION_H
#define DIGIKAM_IMGQSORT_TEST_DETECT_COMPRESSION_H

// Qt includes

#include <QObject>
#include <QString>

// Local includes

#include "imgqsorttest.h"

namespace Digikam

{

class ImgQSortTestDetectCompression : public ImgQSortTest
{
    Q_OBJECT

public:

    explicit ImgQSortTestDetectCompression(QObject* const parent = nullptr);

private:

private Q_SLOTS:

    void testParseTestImagesForCompressionDetection();
    void testParseTestImagesForCompressionDetection_failCase();

};

DataTestCases const dataTestCases =
    {
        {QLatin1String("compressionDetection"), PairImageQuality(QLatin1String("test_compressed_2.jpg"),2)},
        {QLatin1String("compressionDetection"), PairImageQuality(QLatin1String("test_compressed_4.jpg"),2)},
        {QLatin1String("compressionDetection"), PairImageQuality(QLatin1String("test_compressed_9.jpg"),3)},

        {QLatin1String("compressionDetection_failCase"), PairImageQuality(QLatin1String("test_compressed_1.jpg"),1)}, //False case : Pending instead of Accepted
    };

}

#endif // DIGIKAM_IMGQSORT_TEST_DETECT_COMPRESSION_H
