/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-07-09
 * Description : Preprocessor for face recognition
 *
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2019-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

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
    Private* const d = nullptr;
};

} // namespace Digikam
