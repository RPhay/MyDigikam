/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-11-10
 * Description : Face classifier
 *
 * SPDX-FileCopyrightText: 2024      by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024      by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// other includes

// #include <opencv2/ml.hpp>

// local includes

#include "faceclassifierbase.h"
#include "facescansettings.h"

namespace Digikam
{

class DIGIKAM_EXPORT FaceClassifier : public FaceClassifierBase
{
public:
    static FaceClassifier* instance();

    /* Tunes backend parameters.
     * Available parameters:
     * "accuracy", synonymous: "threshold", range: 0-1, type: float
     * Determines recognition threshold, 0->accept very insecure recognitions, 1-> be very sure about a recognition.
     *
     * "recognizeModel" : sets the recognizer model used to instantiate the correct recognizer
     */
    void        setParameters(const FaceScanSettings& parameters);

    bool ready()                                        const;
 
    int predict(const cv::Mat& target)                  const;
    int predict(const cv::UMat& target)                 const;

    bool retrain();

protected:

    bool loadTrainingData();

    void slotTrainingComplete();

private:

    class Private;
    static Private* d;

    cv::Ptr<cv::ml::KNearest>   createKNearest();
    cv::Ptr<cv::ml::SVM>        createSVM();

    int                         predictFullSearch(const cv::Mat& target)    const;
    int                         predictClassifier(const cv::Mat& target)    const;
    int                         listSearch(const cv::Mat& target, const QMap<int, QList<cv::Mat> >& identityFeatures) const;

    bool                        validateKNNSVMResult(const cv::Mat& target, int label) const;
    bool                        featureSFaceCompare(const cv::Mat& target, const cv::Mat& sample, float& distance) const;

    // hide
    FaceClassifier();
    ~FaceClassifier();

    FaceClassifier(FaceClassifier&)                     = delete;

    friend class FaceClassifierCreator;

};

}