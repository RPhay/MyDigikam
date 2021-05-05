/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2020-05-22
 * Description : Wrapper of face recognition using OpenFace
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

#ifndef OPENCV_DNN_FACERECOGNIZER_H
#define OPENCV_DNN_FACERECOGNIZER_H

// Qt includes

#include <QImage>

// Local includes

#include "digikam_opencv.h"
#include "digikam_export.h"
#include "identity.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT OpenCVDNNFaceRecognizer
{
public:

    enum Classifier
    {
        SVM = 0,
        OpenCV_KNN,
        Tree,
        DB,
    };

    /**
     *  @brief OpenCVDNNFaceRecognizer:Master class to control entire recognition using OpenFace algorithm
     */
    explicit OpenCVDNNFaceRecognizer(Classifier method = Tree);
    ~OpenCVDNNFaceRecognizer();

public:

    /**
     *  Returns a cvMat created from the inputImage, optimized for recognition
     */
    static cv::Mat prepareForRecognition(QImage& inputImage);

    /**
     *  Returns a cvMat created from the cvinputImage, optimized for recognition
     */
    static cv::Mat prepareForRecognition(const cv::Mat& cvinputImage);

    /**
     * Register faces corresponding to an identity
     */
    void train(const QList<QImage*>& images, const int label, const QString& context);

    /**
     * Try to recognize the given image.
     * Returns the identity id.
     * If the identity cannot be recognized, returns -1.
     * TODO: verify workflow to economize this routine
     */
    int recognize(QImage* inputImage);

    /**
     * Try to recognize a list of given images.
     * Returns a list of identity ids.
     * If an identity cannot be recognized, returns -1.
     */
    QVector<int> recognize(const QList<QImage*>& inputImages);

    /**
     * clear specified trained data
     */
    void clearTraining(const QList<int>& idsToClear, const QString& trainingContext);

    /**
     * set K parameter of K-Nearest neighbors algorithm
     */
    void setNbNeighBors(int k);

    /**
     * set maximum square distance of 2 vector
     */
    void setThreshold(float threshold);

    /**
     * @brief register training data for unit test
     */
    bool registerTrainingData(const cv::Mat& preprocessedImage, int label);

    /**
     * @brief predict label of test data for unit test
     */
    int verifyTestData(const cv::Mat& preprocessedImage);

private:

    // Disable
    OpenCVDNNFaceRecognizer(const OpenCVDNNFaceRecognizer&)            = delete;
    OpenCVDNNFaceRecognizer& operator=(const OpenCVDNNFaceRecognizer&) = delete;

private:

    class Private;
    Private* d;
};

} // namespace Digikam

#endif // OPENCV_DNN_FACERECOGNIZER_H
