/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2020-05-22
 * Description : Wrapper of face recognition using OpenFace.
 *
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2020-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2020      by Nghia Duong <minhnghiaduong997 at gmail dot com>
 * SPDX-FileCopyrightText: 2024      by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QImage>

// Local includes

#include "digikam_opencv.h"
#include "digikam_export.h"
#include "identity.h"
#include "facescansettings.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT OpenCVDNNFaceRecognizer
{
public:

    enum Classifier
    {
        SVM = 0,            ///< Support Vector Machines                  (https://docs.opencv.org/4.x/dc/dd6/ml_intro.html#ml_intro_svm)
        OpenCV_KNN,         ///< K-Nearest Neighbors                      (https://docs.opencv.org/4.x/dc/dd6/ml_intro.html#ml_intro_knn)
        Tree,               ///< K-Nearest Neighbors Tree                 (https://en.wikipedia.org/wiki/K-nearest_neighbors_algorithm)
        DB                  ///< Closest Neighbors Tree from the database
    };

    /**
     *  @brief OpenCVDNNFaceRecognizer:Master class to control entire recognition using OpenFace algorithm.
     */
    explicit OpenCVDNNFaceRecognizer(Classifier method, FaceScanSettings::FaceRecognitionModel recModel);
    ~OpenCVDNNFaceRecognizer();

public:

    /**
     *  Returns a cvMat created from the inputImage, optimized for recognition.
     */
    static cv::Mat prepareForRecognition(QImage& inputImage);

    /**
     *  Returns a cvMat created from the cvinputImage, optimized for recognition.
     */
    static cv::Mat prepareForRecognition(const cv::Mat& cvinputImage);

    /**
     *  Returns a cvMat of the extracted features from the cvinputImage, optimized for recognition.
     */
    bool remove(const QString& hash);

    /**
     * Register faces corresponding to an identity.
     */
    void train(const QList<QPair<QImage*, QString> >& images, const int label);

    /**
     * Try to recognize the given image.
     * Returns the identity id.
     * If the identity cannot be recognized, returns -1.
     * TODO: verify workflow to economize this routine.
     */
    int recognize(QPair<QImage*, QString> inputImage);

    /**
     * Try to recognize a list of given images.
     * Returns a list of identity ids.
     * If an identity cannot be recognized, returns -1.
     */
    QVector<int> recognize(const QList<QPair<QImage*, QString> >& inputImages);

    /**
     * Clear specified trained data.
     */
    void clearTraining(const QList<int>& idsToClear);

    /**
     * Set K parameter of K-Nearest neighbors algorithm.
     */
    void setNbNeighbors(int k);

    /**
     * Set maximum square distance of 2 vectors.
     */
    void setThreshold(int threshold);

    /**
     * @brief register training data for unit test.
     */
    bool registerTrainingData(const cv::Mat& preprocessedImage, int label);

    /**
     * @brief predict label of test data for unit test.
     */
    int verifyTestData(const cv::Mat& preprocessedImage);

private:

    // Disable
    OpenCVDNNFaceRecognizer(const OpenCVDNNFaceRecognizer&)            = delete;
    OpenCVDNNFaceRecognizer& operator=(const OpenCVDNNFaceRecognizer&) = delete;

private:

    class Private;
/*
    Private* const d = nullptr;
*/
    static Private* d;
};

} // namespace Digikam
