/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-06-01
 * Description : Face recognition using deep learning
 *               The internal DNN library interface
 *
 * Copyright (C) 2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef FACE_EXTRACTOR_H
#define FACE_EXTRACTOR_H

// C++ includes

#include <vector>

// Local includes

#include "digikam_opencv.h"
#include "digikam_export.h"

namespace RecognitionTest
{
// Clone of DNNFaceExtractor
// TODO change it back when finish
// TODO remove export macro after testing
class DIGIKAM_GUI_EXPORT FaceExtractor
{

public:

    explicit FaceExtractor();
    ~FaceExtractor();

    cv::Mat alignFace(const cv::Mat& inputImage);

    std::vector<float> getFaceEmbedding(const cv::Mat& faceImage);

    static double cosineDistance(std::vector<float> v1, std::vector<float> v2);

    static double L2Distance(std::vector<float> v1, std::vector<float> v2);

    static double L2NormDistance(std::vector<float> v1, std::vector<float> v2);

private:

    class Private;
    Private* d;
};

} // namespace RecognitionTest

#endif // FACE_EXTRACTOR_H
