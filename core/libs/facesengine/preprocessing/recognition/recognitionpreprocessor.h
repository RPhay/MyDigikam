/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-07-09
 * Description : Preprocessor for face recognition
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

#ifndef DIGIKAM_RECOGNITION_PREPROCESSOR_H
#define DIGIKAM_RECOGNITION_PREPROCESSOR_H

#include "facepreprocessor.h"

// Local includes

#include "digikam_export.h"

namespace Digikam
{

enum PreprocessorSelection
{
    OPENFACE = 0
};

class DIGIKAM_EXPORT RecognitionPreprocessor: public FacePreprocessor
{

public:

    explicit RecognitionPreprocessor();
    ~RecognitionPreprocessor()                     override;

public:

    void init(PreprocessorSelection mode);

    cv::Mat preprocess(const cv::Mat& image) const override;

private:

    // Disable
    RecognitionPreprocessor(const RecognitionPreprocessor&)            = delete;
    RecognitionPreprocessor& operator=(const RecognitionPreprocessor&) = delete;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_RECOGNITION_PREPROCESSOR_H
