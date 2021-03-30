/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-06-01
 * Description : Face recognition using deep learning
 *               The internal DNN library interface
 *
 * Copyright (C) 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * Copyright (C) 2020-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2020      by Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#ifndef DNN_FACE_EXTRACTOR_H
#define DNN_FACE_EXTRACTOR_H

// C++ includes

#include <vector>

// Qt include

#include <QJsonArray>

// Local includes

#include "digikam_opencv.h"
#include "digikam_export.h"

namespace Digikam
{

// TODO: remove export macro after testing

class DIGIKAM_GUI_EXPORT DNNFaceExtractor
{

public:

    explicit DNNFaceExtractor();
    DNNFaceExtractor(const DNNFaceExtractor&);
    ~DNNFaceExtractor();

public:

    /**
     * Read pretrained neural network for face recognition.
     */
    bool loadModels();


    cv::Mat alignFace(const cv::Mat& inputImage) const;
    cv::Mat getFaceEmbedding(const cv::Mat& faceImage);

    /**
     * Calculate different between 2 vectors
     */
    static double cosineDistance(std::vector<float> v1, std::vector<float> v2);
    static double L2squareDistance(std::vector<float> v1, std::vector<float> v2);
    static double L2squareNormDistance(std::vector<float> v1, std::vector<float> v2);

    /**
     * Convert face embedding between different formats
     */
    static cv::Mat            vectortomat(const std::vector<float>& vector);
    static QJsonArray         encodeVector(const std::vector<float>& vector);
    static std::vector<float> decodeVector(const QJsonArray& json);

private:

    /// Disable
    DNNFaceExtractor& operator=(const DNNFaceExtractor&) = delete;

private:

    class Private;
    Private* d;
};

} // namespace Digikam

#endif // DNN_FACE_EXTRACTOR_H
