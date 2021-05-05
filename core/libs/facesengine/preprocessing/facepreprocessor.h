/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-07-09
 * Description : Abstract class for preprocessor of facesengine
 *
 * Copyright (C) 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * Copyright (C) 2019-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_FACE_PREPROCESSOR_H
#define DIGIKAM_FACE_PREPROCESSOR_H

// Local includes

#include "digikam_opencv.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT FacePreprocessor
{

public:

    explicit FacePreprocessor();
    virtual ~FacePreprocessor();

    virtual cv::Mat preprocess(const cv::Mat& image) const = 0;

private:

    // Disable
    FacePreprocessor(const FacePreprocessor&)            = delete;
    FacePreprocessor& operator=(const FacePreprocessor&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_FACE_PREPROCESSOR_H
