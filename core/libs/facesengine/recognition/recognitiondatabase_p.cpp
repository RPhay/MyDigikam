/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : The recognition database wrapper
 *
 * Copyright (C)      2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C)      2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
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

#include "recognitiondatabase_p.h"

namespace Digikam
{

RecognitionDatabase::Private::Private()
    : mutex(QMutex::Recursive),
      opencvfisher(nullptr),
      opencveigen(nullptr),
      opencvlbph(nullptr),
      opencvdnn(nullptr),
      funnel(nullptr)
{
    DbEngineParameters params = CoreDbAccess::parameters().faceParameters();
    params.setFaceDatabasePath(CoreDbAccess::parameters().faceParameters().getFaceDatabaseNameOrDir());
    FaceDbAccess::setParameters(params);
    dbAvailable               = FaceDbAccess::checkReadyForUse(nullptr);
    recognizeAlgorithm        = RecognizeAlgorithm::LBP;

    if (dbAvailable)
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Face database ready for use";

        foreach (const Identity& identity, FaceDbAccess().db()->identities())
        {
            identityCache[identity.id()] = identity;
        }
    }
    else
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Failed to initialize face database";
    }
}

RecognitionDatabase::Private::~Private()
{
    delete opencvfisher;
    delete opencveigen;
    delete opencvlbph;
    delete opencvdnn;
    delete funnel;
}

void RecognitionDatabase::Private::applyParameters()
{
    if (lbphConst()   ||
        eigenConst()  ||
        fisherConst())
    {
        for (QVariantMap::const_iterator it = parameters.constBegin() ; it != parameters.constEnd() ; ++it)
        {
            if (it.key() == QLatin1String("threshold") || it.key() == QLatin1String("accuracy"))
            {
                if      (recognizeAlgorithm == RecognitionDatabase::RecognizeAlgorithm::LBP)
                {
                    lbph()->setThreshold(it.value().toFloat());
                }
                else if (recognizeAlgorithm == RecognitionDatabase::RecognizeAlgorithm::EigenFace)
                {
                    eigen()->setThreshold(it.value().toFloat());
                }
                else if (recognizeAlgorithm == RecognitionDatabase::RecognizeAlgorithm::FisherFace)
                {
                    fisher()->setThreshold(it.value().toFloat());
                }
                else
                {
                    qCCritical(DIGIKAM_FACESENGINE_LOG) << "No obvious recognize algorithm";
                }
            }
        }
    }

    if (recognizeAlgorithm == RecognitionDatabase::RecognizeAlgorithm::DNN)
    {
        float threshold = 0.8;

        if      (parameters.contains(QLatin1String("threshold")))
        {
            threshold = parameters.value(QLatin1String("threshold")).toFloat();
        }
        else if (parameters.contains(QLatin1String("accuracy")))
        {
            threshold = parameters.value(QLatin1String("accuracy")).toFloat();
        }

        OpenCVDNNFaceRecognizer::m_threshold = threshold;
    }
}

cv::Mat RecognitionDatabase::Private::preprocessingChain(const QImage& image)
{
    try
    {
        cv::Mat cvImage;

        if      (recognizeAlgorithm == RecognizeAlgorithm::LBP)
        {
            cvImage = lbph()->prepareForRecognition(image);
        }
        else if (recognizeAlgorithm == RecognizeAlgorithm::EigenFace)
        {
            cvImage = eigen()->prepareForRecognition(image);
        }
        else if (recognizeAlgorithm == RecognizeAlgorithm::FisherFace)
        {
            cvImage = fisher()->prepareForRecognition(image);
        }
        else if (recognizeAlgorithm == RecognizeAlgorithm::DNN)
        {
            cvImage = dnn()->prepareForRecognition(image);
        }
        else
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "No obvious recognize algorithm";
        }
/*
        cvImage = aligner()->align(cvImage);
        TanTriggsPreprocessor preprocessor;
        cvImage = preprocessor.preprocess(cvImage);
*/
        return cvImage;
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception:" << e.what();
        return cv::Mat();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
        return cv::Mat();
    }
}

cv::Mat RecognitionDatabase::Private::preprocessingChainRGB(const QImage& image)
{
    try
    {
        cv::Mat cvImage;

        if      (recognizeAlgorithm == RecognizeAlgorithm::LBP)
        {
            cvImage = lbph()->prepareForRecognition(image);
        }
        else if (recognizeAlgorithm == RecognizeAlgorithm::EigenFace)
        {
            cvImage = eigen()->prepareForRecognition(image);
        }
        else if (recognizeAlgorithm == RecognizeAlgorithm::FisherFace)
        {
            cvImage = fisher()->prepareForRecognition(image);
        }
        else if (recognizeAlgorithm == RecognizeAlgorithm::DNN)
        {
            cvImage = dnn()->prepareForRecognition(image);
        }
        else
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "No obvious recognize algorithm";
        }
/*
        cvImage = aligner()->align(cvImage);
        TanTriggsPreprocessor preprocessor;
        cvImage = preprocessor.preprocess(cvImage);
*/
        return cvImage;
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception:" << e.what();
        return cv::Mat();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
        return cv::Mat();
    }
}

void RecognitionDatabase::Private::clear(OpenCVLBPHFaceRecognizer* const,
                                         const QList<int>& idsToClear,
                                         const QString& trainingContext)
{
    // force later reload
    delete opencvlbph;
    opencvlbph = nullptr;

    if (idsToClear.isEmpty())
    {
        FaceDbAccess().db()->clearLBPHTraining(trainingContext);
    }
    else
    {
        FaceDbAccess().db()->clearLBPHTraining(idsToClear, trainingContext);
    }
}

void RecognitionDatabase::Private::clear(OpenCVEIGENFaceRecognizer* const,
                                         const QList<int>& idsToClear,
                                         const QString& trainingContext)
{
    // force later reload
    delete opencveigen;
    opencveigen = nullptr;

    if (idsToClear.isEmpty())
    {
        FaceDbAccess().db()->clearEIGENTraining(trainingContext);
    }
    else
    {
        FaceDbAccess().db()->clearEIGENTraining(idsToClear, trainingContext);
    }
}

void RecognitionDatabase::Private::clear(OpenCVFISHERFaceRecognizer* const,
                                         const QList<int>&,
                                         const QString&)
{
    // force later reload
    delete opencvfisher;
    opencvfisher = nullptr;
}

void RecognitionDatabase::Private::clear(OpenCVDNNFaceRecognizer* const,
                                         const QList<int>&,
                                         const QString&)
{
    // force later reload
    delete opencvdnn;
    opencvdnn = nullptr;
}

} // namespace Digikam
