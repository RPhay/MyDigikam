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

    if      (d->recognizeAlgorithm == RecognizeAlgorithm::LBP)
    {
        d->train(d->lbph(),  identitiesToBeTrained, data, trainingContext);
    }
    else if (d->recognizeAlgorithm == RecognizeAlgorithm::EigenFace)
    {
        d->train(d->eigen(), identitiesToBeTrained, data, trainingContext);
    }
    else if (d->recognizeAlgorithm == RecognizeAlgorithm::FisherFace)
    {
        // No method to call
    }
    else if (d->recognizeAlgorithm == RecognizeAlgorithm::DNN)
    {
        d->train(d->dnn(),   identitiesToBeTrained, data, trainingContext);
    }
    else
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "No obvious recognize algorithm";
    }
}

void RecognitionDatabase::train(const Identity& identityToBeTrained,
                                const QImage& image,
                                const QString& trainingContext)
{
    SimpleTrainingDataProvider* const data = new SimpleTrainingDataProvider(identityToBeTrained,
                                                                            QList<QImage>() << image);
    train(identityToBeTrained, data, trainingContext);
    delete data;
}

void RecognitionDatabase::train(const Identity& identityToBeTrained,
                                const QList<QImage>& images,
                                const QString& trainingContext)
{
    SimpleTrainingDataProvider* const data = new SimpleTrainingDataProvider(identityToBeTrained, images);
    train(identityToBeTrained, data, trainingContext);
    delete data;
}

void RecognitionDatabase::clearAllTraining(const QString& trainingContext)
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    QMutexLocker lock(&d->mutex);

    d->clear(d->lbph(),   QList<int>(), trainingContext);
    d->clear(d->eigen(),  QList<int>(), trainingContext);
    d->clear(d->fisher(), QList<int>(), trainingContext);
    d->clear(d->dnn(),    QList<int>(), trainingContext);
}

void RecognitionDatabase::clearTraining(const QList<Identity>& identitiesToClean, const QString& trainingContext)
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

    if      (d->recognizeAlgorithm == RecognizeAlgorithm::LBP)
    {
        d->clear(d->lbph(), ids, trainingContext);
    }
    else if (d->recognizeAlgorithm == RecognizeAlgorithm::EigenFace)
    {
        d->clear(d->eigen(), ids, trainingContext);
    }
    else if (d->recognizeAlgorithm == RecognizeAlgorithm::FisherFace)
    {
        d->clear(d->fisher(), ids, trainingContext);
    }
    else if (d->recognizeAlgorithm == RecognizeAlgorithm::DNN)
    {
        d->clear(d->dnn(), ids, trainingContext);
    }
    else
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "No obvious recognize algorithm";
    }
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
