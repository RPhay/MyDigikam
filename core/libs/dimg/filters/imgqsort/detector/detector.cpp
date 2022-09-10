/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        :
 * Description : Image Quality Parser - blur detection
 *
 * SPDX-FileCopyrightText: 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2021      by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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

    try
    {
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
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
    }

    return cv::Mat();
}

// void DetectorDistortion::run()
// {
//     Q_EMIT resultReady(detect(m_image));
// }

} // namespace Digikam
