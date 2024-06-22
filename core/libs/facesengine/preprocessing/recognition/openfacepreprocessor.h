/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-07-09
 * Description : Preprocessor for openface nn model
 *
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2019-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// C++ includes

#include <array>

// Qt includes

#include <QMutex>

// Local includes

#include "digikam_opencv.h"
#include "shapepredictor.h"

namespace Digikam
{

class OpenfacePreprocessor
{

public:

    OpenfacePreprocessor();
    ~OpenfacePreprocessor() = default;

    /**
     * Load shapepredictor model for face alignment with 68 points of face landmark extraction.
     */
    bool loadModels();

    cv::Mat process(const cv::Mat& image);

private:

    cv::Size               outImageSize;

    cv::Mat                faceTemplate;
    std::array<int, 3>     outerEyesNosePositions;

    RedEye::ShapePredictor sp;

    QMutex                 mutex;

private:

    // Disable
    OpenfacePreprocessor(const OpenfacePreprocessor&)            = delete;
    OpenfacePreprocessor& operator=(const OpenfacePreprocessor&) = delete;
};

} // namespace Digikam
