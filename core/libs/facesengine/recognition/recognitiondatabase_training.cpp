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

void RecognitionDatabase::Private::train(const QList<Identity>& identitiesToBeTrained,
                                         TrainingDataProvider* const data,
                                         const QString& trainingContext)
{
#ifdef USE_DNN_RECOGNITION_BACKEND
    if (recognizeAlgorithm == RecognizeAlgorithm::DNN)
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Training using opencv DNN";
        trainIdentityBatchDNN(identitiesToBeTrained, data, trainingContext);
    }

#else
    if (recognizeAlgorithm == RecognizeAlgorithm::LBP)
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Training using opencv LBPH";
        trainIdentityBatchLBPH(identitiesToBeTrained, data, trainingContext);
    }
#endif
/*
NOTE: experimental and deprecated
    if (recognizeAlgorithm == RecognizeAlgorithm::EigenFace)
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Training using opencv EIGEN-faces";
        trainIdentityBatchEIGEN(identitiesToBeTrained, data, trainingContext);
    }

    if (recognizeAlgorithm == RecognizeAlgorithm::FisherFace)
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Training using opencv FISHER-faces";
        trainIdentityBatchFISHER(identitiesToBeTrained, data, trainingContext);
    }
*/
    else
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "No obvious recognize algorithm";
    }
}

#ifdef USE_DNN_RECOGNITION_BACKEND

void RecognitionDatabase::Private::trainIdentityBatchDNN(const QList<Identity>& identitiesToBeTrained,
                                                         TrainingDataProvider* const data,
                                                         const QString& trainingContext)
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
                cv::Mat cvImage     = preprocessingChain(imageList->image());
                cv::Mat cvImage_rgb = preprocessingChainRGB(imageList->image());

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

        qCDebug(DIGIKAM_FACESENGINE_LOG) << "DNN Training" << images.size() << "images for identity" << identity.id();

        try
        {
            // TODO : investigate this routine
            //recognizer->train(images, labels, trainingContext, images_rgb);
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

#else
void RecognitionDatabase::Private::trainIdentityBatchLBPH(const QList<Identity>& identitiesToBeTrained,
                                                          TrainingDataProvider* const data,
                                                          const QString& trainingContext)
{
/*
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
                cv::Mat cvImage     = preprocessingChain(imageList->image());
                cv::Mat cvImage_rgb = preprocessingChainRGB(imageList->image());

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

        qCDebug(DIGIKAM_FACESENGINE_LOG) << "LBPH Training" << images.size() << "images for identity" << identity.id();

        try
        {
            lbph()->train(images, labels, trainingContext);
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
*/
}

#endif

/*
NOTE: experimental and deprecated

void trainIdentityBatchEIGEN(const QList<Identity>& identitiesToBeTrained,
                             TrainingDataProvider* const data,
                             const QString& trainingContext)
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
                cv::Mat cvImage     = preprocessingChain(imageList->image());
                cv::Mat cvImage_rgb = preprocessingChainRGB(imageList->image());

                labels.push_back(identity.id());
                images.push_back(cvImage);
                images_rgb.push_back(cvImage_rgb);
            }
            catch (cv::Exception& e)
            {
                qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception preparing image for EIGEN:" << e.what();
            }
            catch (...)
            {
                qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
            }
        }

        qCDebug(DIGIKAM_FACESENGINE_LOG) << "EIGEN Training" << images.size() << "images for identity" << identity.id();

        try
        {
            eigen()->train(images, labels, trainingContext, images_rgb);
        }
        catch (cv::Exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception training EIGEN Recognizer:" << e.what();
        }
        catch (...)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
        }
    }
}

void trainIdentityBatchFISHER(const QList<Identity>& identitiesToBeTrained,
                              TrainingDataProvider* const data,
                              const QString& trainingContext)
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
                cv::Mat cvImage     = preprocessingChain(imageList->image());
                cv::Mat cvImage_rgb = preprocessingChainRGB(imageList->image());

                labels.push_back(identity.id());
                images.push_back(cvImage);
            }
            catch (cv::Exception& e)
            {
                qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception preparing image for FISHER:" << e.what();
            }
            catch (...)
            {
                qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
            }
        }

        qCDebug(DIGIKAM_FACESENGINE_LOG) << "FISHER Training" << images.size() << "images for identity" << identity.id();

        try
        {
            fisher()->train(images, labels, trainingContext);
        }
        catch (cv::Exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception training FISHER Recognizer:" << e.what();
        }
        catch (...)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
        }
    }
}

*/

// -------------------------------------------------------------------------------------

RecognitionDatabase::TrainingCostHint RecognitionDatabase::trainingCostHint() const
{
    return TrainingIsCheap;
}

void RecognitionDatabase::train(const Identity& identityToBeTrained,
                                TrainingDataProvider* const data,
                                const QString& trainingContext)
{
    train(QList<Identity>() << identityToBeTrained, data, trainingContext);
}

void RecognitionDatabase::train(const QList<Identity>& identitiesToBeTrained,
                                TrainingDataProvider* const data,
                                const QString& trainingContext)
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    QMutexLocker lock(&d->mutex);

    d->train(identitiesToBeTrained, data, trainingContext);
}

void RecognitionDatabase::train(const Identity& identityToBeTrained,
                                const QImage& image,
                                const QString& trainingContext)
{
    RecognitionTrainingProvider* const data = new RecognitionTrainingProvider(identityToBeTrained,
                                                                              QList<QImage>() << image);
    train(identityToBeTrained, data, trainingContext);
    delete data;
}

void RecognitionDatabase::train(const Identity& identityToBeTrained,
                                const QList<QImage>& images,
                                const QString& trainingContext)
{
    RecognitionTrainingProvider* const data = new RecognitionTrainingProvider(identityToBeTrained, images);
    train(identityToBeTrained, data, trainingContext);
    delete data;
}


#ifdef USE_DNN_RECOGNITION_BACKEND

void RecognitionDatabase::Private::clearDNN(const QList<int>& idsToClear,
                                            const QString& trainingContext)
{
    // force later reload
    // TODO : investigate this routine
    delete opencvdnn;
    opencvdnn = nullptr;

    if (idsToClear.isEmpty())
    {
        // TODO: reimplement facedb
        //FaceDbAccess().db()->clearDNNTraining(trainingContext);
    }
    else
    {
        //FaceDbAccess().db()->clearDNNTraining(idsToClear, trainingContext);
    }
}

#else

void RecognitionDatabase::Private::clearLBPH(const QList<int>& idsToClear,
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

#endif

/*
    NOTE: experimental and deprecated

void RecognitionDatabase::Private::clearEIGEN(const QList<int>& idsToClear,
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

void RecognitionDatabase::Private::clearFISHER()
{
    // force later reload

    delete opencvfisher;
    opencvfisher = nullptr;
}

*/
void RecognitionDatabase::Private::clear(const QList<int>& idsToClear,
                                         const QString& trainingContext)
{
#ifdef USE_DNN_RECOGNITION_BACKEND
    if (recognizeAlgorithm == RecognizeAlgorithm::DNN)
    {
        clearDNN(idsToClear, trainingContext);
    }

#else
    if (recognizeAlgorithm == RecognizeAlgorithm::LBP)
    {
        clearLBPH(idsToClear, trainingContext);
    }

#endif

/*
    NOTE: experimental and deprecated
    if (recognizeAlgorithm == RecognizeAlgorithm::EigenFace)
    {
        clearEIGEN(idsToClear, trainingContext);
    }

    if (recognizeAlgorithm == RecognizeAlgorithm::FisherFace)
    {
        clearFISHER();
    }
*/
    else
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "No obvious recognize algorithm";
    }
}

void RecognitionDatabase::clearAllTraining(const QString& trainingContext)
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    QMutexLocker lock(&d->mutex);

    FaceDbAccess().db()->clearIdentities();

    d->clear(QList<int>(), trainingContext);
}

void RecognitionDatabase::clearTraining(const QList<Identity>& identitiesToClean,
                                        const QString& trainingContext)
{
    if (!d || !d->dbAvailable || identitiesToClean.isEmpty())
    {
        return;
    }

    QMutexLocker lock(&d->mutex);
    QList<int>   ids;

    foreach (const Identity& id, identitiesToClean)
    {
        ids << id.id();
    }

    d->clear(ids, trainingContext);
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

} // namespace Digikam
