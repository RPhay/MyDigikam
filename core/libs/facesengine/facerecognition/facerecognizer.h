/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2020-05-22
 * Description : Wrapper of face recognition using OpenFace
 *
 * Copyright (C) 2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
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


#ifndef FACERECOGNIZER_H
#define FACERECOGNIZER_H

// Qt include

#include <QImage>

// Local library

#include "digikam_opencv.h"
#include "digikam_export.h"
#include "identity.h"

using namespace Digikam;

namespace RecognitionTest
{
// TODO remove export macro after testing
class DIGIKAM_GUI_EXPORT FaceRecognizer
{
public:

    enum ComparisonMetric
    {
        CosDistance = 0,
        L2Distance,
        SupportVectorMachine,
        KNN,
        Tree,
    };

    /**
     *  @brief FaceRecognizer:Master class to control entire recognition using OpenFace algorithm
     */
    explicit FaceRecognizer(bool debug = false);
    ~FaceRecognizer();

public:

    /**
     *  Returns a cvMat created from the inputImage, optimized for recognition
     */
    cv::Mat prepareForRecognition(const QImage& inputImage);

    /**
     * @brief findIdenity : look for an identity based on the face image
     * @param inputImage
     * @return
     */
    Identity findIdenity(const cv::Mat& preprocessedImage, ComparisonMetric metric, double threshold = 0.7);

    /**
     * @brief newIdentity : construct an instance of Identity with face embedding, and return it
     *                      for idenity register
     * @param inputImage
     * @return
     */
    Identity newIdentity(const cv::Mat& preprocessedImage);

    /**
     * @brief saveIdentity : save identity with label and face embedding
     * @param id : identity of face
     * @param newLabel : add new label to database
     * @return
     */
    int saveIdentity(Identity& id, bool newLabel);

private:

    /// Hidden copy constructor and assignment operator.
    FaceRecognizer(const FaceRecognizer&);
    FaceRecognizer& operator=(const FaceRecognizer&);

    class Private;
    Private* d;
};

}
#endif // FACERECOGNIZER_H
