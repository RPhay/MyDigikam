/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Image Quality Parser - Compression detection
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

#ifndef DIGIKAM_COMPRESSION_DETECTOR_H
#define DIGIKAM_COMPRESSION_DETECTOR_H

// Local includes

#include "dimg.h"
#include "digikam_opencv.h"
#include "detector.h"

namespace Digikam
{

class CompressionDetector : public DetectorDistortion
{
    Q_OBJECT

public:

    explicit CompressionDetector();
    ~CompressionDetector();

    float detect(const cv::Mat& image)                      const override;

private:

    template <typename Function>
    cv::Mat checkEdgesBlock(const cv::Mat& gray_image,
                            int blockSize,
                            Function accessEdges)           const;
    cv::Mat detectMonoColorRegion(const cv::Mat& image)     const;

    float normalize(const float number);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_COMPRESSION_DETECTOR_H
