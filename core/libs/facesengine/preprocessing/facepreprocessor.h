/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-07-09
 * Description : Abstract class for preprocessor of facesengine
 *
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2019-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
