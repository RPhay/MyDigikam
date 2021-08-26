/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 
 * Description : Image Quality Parser - Noise detection
 *
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * References  : https://cse.buffalo.edu/~siweilyu/papers/ijcv14.pdf
 * 
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

#include "exposure_detector.h"

// Qt includes

#include <QtMath>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN ExposureDetector::Private
{
public:
    explicit Private()
      : threshold_overexposed(245),
        threshold_demi_overexposed(235),
        threshold_underexposed(15),
        threshold_demi_underexposed(30),
        weight_over_exposure(15),
        weight_demi_over_exposure(1),
        weight_under_exposure(15),
        weight_demi_under_exposure(6)
    {
    }

    int threshold_overexposed;
    int threshold_demi_overexposed;
    int threshold_underexposed;
    int threshold_demi_underexposed;

    int weight_over_exposure;
    int weight_demi_over_exposure;
    
    int weight_under_exposure;
    int weight_demi_under_exposure;

};

ExposureDetector::ExposureDetector()
    :  DetectorDistortion(),
       d(new Private)
{
}

ExposureDetector::~ExposureDetector()
{
    delete d;
}

float ExposureDetector::detect(const cv::Mat& image) const
{
    float overexposed = percent_overexposed(image);
    
    float underexposed = percent_underexposed(image);

    return std::max(overexposed, underexposed);
}

float ExposureDetector::percent_overexposed(const cv::Mat& image) const
{
    int over_exposed_pixel = count_by_condition(image, d->threshold_overexposed, 255);

    int demi_over_exposed_pixel = count_by_condition(image, d->threshold_demi_overexposed,d->threshold_overexposed);

    int normal_pixel = image.total() - over_exposed_pixel - demi_over_exposed_pixel;

    return static_cast<float>(static_cast<float>(over_exposed_pixel * d->weight_over_exposure + demi_over_exposed_pixel * d->weight_demi_over_exposure) / 
                              static_cast<float>(normal_pixel + over_exposed_pixel * d->weight_over_exposure + demi_over_exposed_pixel * d->weight_demi_over_exposure));
}

float ExposureDetector::percent_underexposed(const cv::Mat& image) const
{
    int under_exposed_pixel = count_by_condition(image, 0, d->threshold_underexposed);

    int demi_under_exposed_pixel = count_by_condition(image, d->threshold_underexposed, d->threshold_demi_underexposed);

    int normal_pixel = image.total() - under_exposed_pixel - demi_under_exposed_pixel;

    return static_cast<float>(static_cast<float>(under_exposed_pixel * d->weight_under_exposure + demi_under_exposed_pixel * d->weight_demi_under_exposure) / 
                              static_cast<float>(normal_pixel + under_exposed_pixel * d->weight_under_exposure + demi_under_exposed_pixel * d->weight_demi_under_exposure));
}

int ExposureDetector::count_by_condition(const cv::Mat& image, int minVal, int maxVal) const
{
    cv::Mat mat = (image >= minVal) & (image < maxVal);

    return cv::countNonZero(mat);
}

}
