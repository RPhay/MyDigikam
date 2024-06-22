/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-07-09
 * Description : Abstract class for preprocessor of facesengine
 *
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2019-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "digikam_opencv.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT FacePreprocessor
{

public:

    FacePreprocessor()          = default;
    virtual ~FacePreprocessor() = default;

    virtual cv::Mat preprocess(const cv::Mat& image) const = 0;

private:

    // Disable
    FacePreprocessor(const FacePreprocessor&)            = delete;
    FacePreprocessor& operator=(const FacePreprocessor&) = delete;
};

} // namespace Digikam
