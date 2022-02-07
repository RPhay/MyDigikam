/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        :
 * Description : Image Quality Parser - blur detection
 *
 * Copyright (C) 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021      by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
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

#include "detector.h"

// Qt includes

#include <QtMath>

// Local includes

#include "digikam_debug.h"
#include "focuspoints_extractor.h"

namespace Digikam
{

DetectorDistortion::DetectorDistortion(QObject* const parent)
    : QObject(parent)
{
}

// Maybe this function will move to read_image() of imagequalityparser
// in case all detector of IQS use cv::Mat
cv::Mat DetectorDistortion::prepareForDetection(const DImg& inputImage)
{
    if (inputImage.isNull() || !inputImage.size().isValid())
    {
        return cv::Mat();
    }

    cv::Mat cvImage;

    int type               = inputImage.sixteenBit() ? CV_16UC4 : CV_8UC4;

    cv::Mat cvImageWrapper = cv::Mat(inputImage.height(), inputImage.width(), type, inputImage.bits());

    cv::cvtColor(cvImageWrapper, cvImage, cv::COLOR_RGBA2BGR);

    if (type == CV_16UC4)
    {
        cvImage.convertTo(cvImage, CV_8UC3, 1 / 256.0);
    }

    return cvImage;
}

// void DetectorDistortion::run()
// {
//     emit resultReady(detect(m_image));
// }

} // namespace Digikam
