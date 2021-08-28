/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Image Quality Parser - noise detection
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

#ifndef DIGIKAM_NOISE_DETECTOR_H
#define DIGIKAM_NOISE_DETECTOR_H

// Local includes

#include "dimg.h"
#include "digikam_opencv.h"
#include "detector.h"

namespace Digikam
{

class NoiseDetector : public DetectorDistortion
{
    Q_OBJECT

public:

    typedef QList<cv::Mat> Mat3D;

public:

    explicit NoiseDetector();
    ~NoiseDetector();

    float detect(const cv::Mat& image)                                          const override;

public:

    static const Mat3D filtersHaar;

private:

    Mat3D   decompose_by_filter(const cv::Mat& image, const Mat3D& filters)     const;
    void    calculate_variance_kurtosis(const Mat3D& channels,
                                        cv::Mat& variance,
                                        cv::Mat& kurtosis)                      const;
    float   noise_variance(const cv::Mat& variance, const cv::Mat& kurtosis)    const;
    float   normalize(const float number)                                       const;

    cv::Mat raw_moment(const NoiseDetector::Mat3D& mat, int order)              const;
    cv::Mat pow_mat(const cv::Mat& mat, float ordre)                            const;
    float   mean_mat(const cv::Mat& mat)                                        const;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_NOISE_DETECTOR_H
