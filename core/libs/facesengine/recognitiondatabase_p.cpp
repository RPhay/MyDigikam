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

// NOTE: Takes care that there may be multiple values of attribute in identity's attributes
bool RecognitionDatabase::Private::identityContains(const Identity& identity, const QString& attribute, const QString& value) const
{
    const QMap<QString, QString> map          = identity.attributesMap();
    QMap<QString, QString>::const_iterator it = map.constFind(attribute);

    for ( ; it != map.constEnd() && it.key() == attribute ; ++it)
    {
        if (it.value() == value)
        {
            return true;
        }
    }

    return false;
}

Identity RecognitionDatabase::Private::findByAttribute(const QString& attribute, const QString& value) const
{
    foreach (const Identity& identity, identityCache)
    {
        if (identityContains(identity, attribute, value))
        {
            return identity;
        }
    }

    return Identity();
}

// NOTE: Takes care that there may be multiple values of attribute in valueMap
Identity RecognitionDatabase::Private::findByAttributes(const QString& attribute, const QMap<QString, QString>& valueMap) const
{
    QMap<QString, QString>::const_iterator it = valueMap.find(attribute);

    for ( ; it != valueMap.end() && it.key() == attribute ; ++it)
    {
        foreach (const Identity& identity, identityCache)
        {
            if (identityContains(identity, attribute, it.value()))
            {
                return identity;
            }
        }
    }

    return Identity();
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

void RecognitionDatabase::Private::train(OpenCVLBPHFaceRecognizer* const r,
                                         const QList<Identity>& identitiesToBeTrained,
                                         TrainingDataProvider* const data,
                                         const QString& trainingContext)
{
    trainIdentityBatchLBPH(r, identitiesToBeTrained, data, trainingContext, this);
}

void RecognitionDatabase::Private::train(OpenCVEIGENFaceRecognizer* const r,
                                         const QList<Identity>& identitiesToBeTrained,
                                         TrainingDataProvider* const data,
                                         const QString& trainingContext)
{
    qCDebug(DIGIKAM_FACESENGINE_LOG) << "Training using opencv eigenfaces";
    trainIdentityBatchEIGEN(r, identitiesToBeTrained, data, trainingContext, this);
}

void RecognitionDatabase::Private::train(OpenCVDNNFaceRecognizer* const r,
                                         const QList<Identity>& identitiesToBeTrained,
                                         TrainingDataProvider* const data,
                                         const QString& trainingContext)
{
    qCDebug(DIGIKAM_FACESENGINE_LOG) << "Training using opencv DNN";
    trainIdentityBatchDNN(r, identitiesToBeTrained, data, trainingContext, this);
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

// --- Static Training methods -------------------------------------------------------------------------------

/// Training where the train method takes one identity and one image
/*
template <class Recognizer>
static void trainSingle(Recognizer* const r,
                        const Identity& identity,
                        TrainingDataProvider* const data,
                        const QString& trainingContext,
                        RecognitionDatabase::Private* const d)
{
    ImageListProvider* const images = data->newImages(identity);

    qCDebug(DIGIKAM_FACESENGINE_LOG) << "Training " << images->size() << " images for identity " << identity.id();

    for ( ; !images->atEnd() ; images->proceed())
    {
        try
        {
            r->train(identity.id(), d->preprocessingChain(images->image()), trainingContext);
        }
        catch (cv::Exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception:" << e.what();
        }
        catch (...)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
        }
    }
}
*/

/**
 * Training where the train method takes a list of identities and images,
 * and updating per-identity is non-inferior to updating all at once.
 */
void trainIdentityBatchLBPH(OpenCVLBPHFaceRecognizer* const r,
                            const QList<Identity>& identitiesToBeTrained,
                            TrainingDataProvider* const data,
                            const QString& trainingContext,
                            RecognitionDatabase::Private* const d)
{
    foreach (const Identity& identity, identitiesToBeTrained)
    {
        std::vector<int>     labels;
        std::vector<cv::Mat> images;

        ImageListProvider* const imageList = data->newImages(identity);
        images.reserve(imageList->size());

        for ( ; !imageList->atEnd() ; imageList->proceed())
        {
            try
            {
                cv::Mat cvImage     = d->preprocessingChain(imageList->image());
                cv::Mat cvImage_rgb = d->preprocessingChainRGB(imageList->image());

                labels.push_back(identity.id());
                images.push_back(cvImage);
            }
            catch (cv::Exception& e)
            {
                qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception preparing image for LBPH:" << e.what();
            }
            catch (...)
            {
                qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
            }
        }

        qCDebug(DIGIKAM_FACESENGINE_LOG) << "LBPH Training " << images.size() << " images for identity " << identity.id();

        try
        {
            r->train(images, labels, trainingContext);
        }
        catch (cv::Exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception training LBPH Recognizer:" << e.what();
        }
        catch (...)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
        }
    }
}

void trainIdentityBatchEIGEN(OpenCVEIGENFaceRecognizer* const r,
                             const QList<Identity>& identitiesToBeTrained,
                             TrainingDataProvider* const data,
                             const QString& trainingContext,
                             RecognitionDatabase::Private* const d)
{
    foreach (const Identity& identity, identitiesToBeTrained)
    {
        std::vector<int>     labels;
        std::vector<cv::Mat> images;
        std::vector<cv::Mat> images_rgb;

        ImageListProvider* const imageList = data->newImages(identity);
        images.reserve(imageList->size());

        for ( ; !imageList->atEnd() ; imageList->proceed())
        {
            try
            {
                cv::Mat cvImage     = d->preprocessingChain(imageList->image());
                cv::Mat cvImage_rgb = d->preprocessingChainRGB(imageList->image());

                labels.push_back(identity.id());
                images.push_back(cvImage);
                images_rgb.push_back(cvImage_rgb);
            }
            catch (cv::Exception& e)
            {
                qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception preparing image for Eigen:" << e.what();
            }
            catch (...)
            {
                qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
            }
        }

        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Eigen Training " << images.size() << " images for identity " << identity.id();

        try
        {
            r->train(images, labels, trainingContext, images_rgb);
        }
        catch (cv::Exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception training Eigen Recognizer:" << e.what();
        }
        catch (...)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
        }
    }
}

void trainIdentityBatchDNN(OpenCVDNNFaceRecognizer* const r,
                           const QList<Identity>& identitiesToBeTrained,
                           TrainingDataProvider* const data,
                           const QString& trainingContext,
                           RecognitionDatabase::Private* const d)
{
    foreach (const Identity& identity, identitiesToBeTrained)
    {
        std::vector<int>     labels;
        std::vector<cv::Mat> images;
        std::vector<cv::Mat> images_rgb;

        ImageListProvider* const imageList = data->newImages(identity);
        images.reserve(imageList->size());

        for ( ; !imageList->atEnd() ; imageList->proceed())
        {
            try
            {
                cv::Mat cvImage     = d->preprocessingChain(imageList->image());
                cv::Mat cvImage_rgb = d->preprocessingChainRGB(imageList->image());

                labels.push_back(identity.id());
                images.push_back(cvImage);
                images_rgb.push_back(cvImage_rgb);
            }
            catch (cv::Exception& e)
            {
                qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception preparing image for DNN:" << e.what();
            }
            catch (...)
            {
                qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
            }
        }

        qCDebug(DIGIKAM_FACESENGINE_LOG) << "DNN Training " << images.size() << " images for identity " << identity.id();

        try
        {
            r->train(images, labels, trainingContext, images_rgb);
        }
        catch (cv::Exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception training DNN Recognizer:" << e.what();
        }
        catch (...)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
        }
    }
}

} // namespace Digikam
