/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 16/08/2016
 * Description : Full object detection class representing the output of the
 *               shape predictor class, containing 64 facial point including
 *               eye, nose, and mouth.
 *
 * SPDX-FileCopyrightText:      2016 by Omar Amin <Omar dot moh dot amin at gmail dot com>
 * SPDX-FileCopyrightText: 2016-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FULL_OBJECT_DETECTION_H
#define DIGIKAM_FULL_OBJECT_DETECTION_H

// C++ includes

#include <vector>

// Local includes

#include "digikam_opencv.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT FullObjectDetection
{
public:

    FullObjectDetection() = default;
    explicit FullObjectDetection(const cv::Rect& rect_);
    FullObjectDetection(const cv::Rect& rect_,
                        const std::vector<std::vector<float> >& parts_);

    const cv::Rect& get_rect()                        const;

    cv::Rect& get_rect();

    unsigned long num_parts()                         const;

    const std::vector<float>& part(unsigned long idx) const;

    std::vector<float>& part(unsigned long idx);

private:

    cv::Rect                         rect;
    std::vector<std::vector<float> > parts;
};

// -------------------------------------------------------------------

std::vector<cv::Rect> getEyes(const FullObjectDetection& shape);

} // namespace Digikam

#endif // DIGIKAM_FULL_OBJECT_DETECTION_H
