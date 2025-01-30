/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-11-10
 * Description : Face classifier
 *
 * SPDX-FileCopyrightText: 2024-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "faceclassifierbase.h"
#include "facescansettings.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT FaceClassifier : public FaceClassifierBase
{
public:

    static FaceClassifier* instance();

    /**
     * @brief Tunes backend parameters.
     *
     * Available parameters:
     *
     * "accuracy", synonymous: "threshold", range: 0-1, type: float
     * Determines recognition threshold, 0->accept very insecure recognitions, 1-> be very sure about a recognition.
     *
     * "recognizeModel" : sets the recognizer model used to instantiate the correct recognizer
     */
    void setParameters(const FaceScanSettings& parameters);

    bool ready()                                        const;

    int predict(const cv::Mat& target)                  const override;
    int predict(const cv::UMat& target)                 const override;

    bool retrain()                                            override;

protected:

    bool loadTrainingData()                                   override;

    void trainingComplete();

private:

    cv::Ptr<cv::ml::KNearest> createKNearest();
    cv::Ptr<cv::ml::SVM>      createSVM();

    int                       predictFullSearch(const cv::Mat& target)                                              const;
    int                       predictClassifier(const cv::Mat& target)                                              const;
    int                       listSearch(const cv::Mat& target, const QMap<int, QList<cv::Mat> >& identityFeatures) const;

    bool                      validateKNNSVMResult(const cv::Mat& target, int label)                                const;
    bool                      featureSFaceCompare(const cv::Mat& target, const cv::Mat& sample, float& distance)    const;

private:

    class Private;
    static Private* d;

private:

    // Disable
    FaceClassifier();
    ~FaceClassifier()                                         override;
    FaceClassifier(const FaceClassifier&)                     = delete;

private:

    friend class FaceClassifierCreator;
};

} // namespace Digikam
