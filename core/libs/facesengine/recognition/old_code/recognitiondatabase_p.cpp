/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : The recognition database wrapper
 *
 * Copyright (C)      2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C)      2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
/*
      NOTE: experimental and deprecated

      opencvfisher(nullptr),
      opencveigen(nullptr),
*/

#ifdef USE_DNN_RECOGNITION_BACKEND

      opencvdnn(nullptr),

#else

      opencvlbph(nullptr),

#endif

      funnel(nullptr)
{
    DbEngineParameters params = CoreDbAccess::parameters().faceParameters();
    params.setFaceDatabasePath(CoreDbAccess::parameters().faceParameters().getFaceDatabaseNameOrDir());
    FaceDbAccess::setParameters(params);

    dbAvailable               = FaceDbAccess::checkReadyForUse(nullptr);
    recognizeAlgorithm        = RecognizeAlgorithm::DNN;

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
/*
    NOTE: experimental and deprecated

    delete opencvfisher;
    delete opencveigen;
*/

#ifdef USE_DNN_RECOGNITION_BACKEND

    delete opencvdnn;

#else

    delete opencvlbph;

#endif

    delete funnel;
}

cv::Mat RecognitionDatabase::Private::preprocessingChain(const QImage& image)
{
    try
    {
        cv::Mat cvImage;

#ifdef USE_DNN_RECOGNITION_BACKEND

        if (recognizeAlgorithm == RecognizeAlgorithm::DNN)
        {
            cvImage = dnn()->prepareForRecognition(image);
        }

#else

        if (recognizeAlgorithm == RecognizeAlgorithm::LBP)
        {
            cvImage = lbph()->prepareForRecognition(image);
        }

#endif

/*
        NOTE: experimental and deprecated

        else if (recognizeAlgorithm == RecognizeAlgorithm::EigenFace)
        {
            cvImage = eigen()->prepareForRecognition(image);
        }
        else if (recognizeAlgorithm == RecognizeAlgorithm::FisherFace)
        {
            cvImage = fisher()->prepareForRecognition(image);
        }
*/
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

#ifdef USE_DNN_RECOGNITION_BACKEND

        if (recognizeAlgorithm == RecognizeAlgorithm::DNN)
        {
            cvImage = dnn()->prepareForRecognition(image);
        }

#else

        if (recognizeAlgorithm == RecognizeAlgorithm::LBP)
        {
            cvImage = lbph()->prepareForRecognition(image);
        }

#endif

/*
        NOTE: experimental and deprecated

        else if (recognizeAlgorithm == RecognizeAlgorithm::EigenFace)
        {
            cvImage = eigen()->prepareForRecognition(image);
        }
        else if (recognizeAlgorithm == RecognizeAlgorithm::FisherFace)
        {
            cvImage = fisher()->prepareForRecognition(image);
        }
*/

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

} // namespace Digikam
