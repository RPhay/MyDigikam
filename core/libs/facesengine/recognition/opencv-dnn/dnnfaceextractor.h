/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-06-01
 * Description : Face recognition using deep learning
 *               The internal DNN library interface
 *
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2020-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2020      by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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

    DNNFaceExtractor();
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
    static double cosineDistance(const std::vector<float>& v1, const std::vector<float>& v2);
    static double L2squareDistance(const std::vector<float>& v1, const std::vector<float>& v2);
    static double L2squareNormDistance(const std::vector<float>& v1, const std::vector<float>& v2);

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
    Private* const d = nullptr;
};

} // namespace Digikam

#endif // DNN_FACE_EXTRACTOR_H
