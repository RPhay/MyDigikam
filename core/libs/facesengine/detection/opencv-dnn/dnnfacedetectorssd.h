/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-08-08
 * Description : Derived class to perform SSD neural network inference
 *               for face detection
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

#ifndef DIGIKAM_FACESENGINE_DNN_FACE_DETECTOR_SSD_H
#define DIGIKAM_FACESENGINE_DNN_FACE_DETECTOR_SSD_H

// Local includes

#include "dnnfacedetectorbase.h"

namespace Digikam
{

class DNNFaceDetectorSSD: public DNNFaceDetectorBase
{

public:

    explicit DNNFaceDetectorSSD();
    ~DNNFaceDetectorSSD()                                   override;

    bool loadModels();

    void detectFaces(const cv::Mat& inputImage,
                     const cv::Size& paddedSize,
                     std::vector<cv::Rect>& detectedBboxes) override;

private:

    void postprocess(cv::Mat detectionMat,
                     const cv::Size& paddedSize,
                     std::vector<cv::Rect>& detectedBboxes) const;

private:

    // Hidden copy constructor and assignment operator.
    DNNFaceDetectorSSD(const DNNFaceDetectorSSD&);
    DNNFaceDetectorSSD& operator=(const DNNFaceDetectorSSD&);
};

} // namespace Digikam

#endif // DIGIKAM_FACESENGINE_DNN_FACE_DETECTOR_SSD_H
