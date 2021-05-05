/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : Training functions of recognition wrapper
 *
 * Copyright (C)      2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C)      2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * Copyright (C)      2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#include "facialrecognition_wrapper_p.h"

namespace Digikam
{

void FacialRecognitionWrapper::Private::trainIdentityBatch(const QList<Identity>&      identitiesToBeTrained,
                                                           TrainingDataProvider* const data,
                                                           const QString&              trainingContext)
{
    foreach (const Identity& identity, identitiesToBeTrained)
    {
        ImageListProvider* const imageList = data->newImages(identity);
        QList<QImage*> images              = imageList->images();

        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Training" << images.size() << "images for identity" << identity.id();

        try
        {
            recognizer->train(images, identity.id(), trainingContext);
        }
        catch (cv::Exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception training Recognizer:" << e.what();
        }
        catch (...)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
        }
    }
}

void FacialRecognitionWrapper::Private::clear(const QList<int>& idsToClear, const QString& trainingContext)
{
    recognizer->clearTraining(idsToClear, trainingContext);

    delete recognizer;

    recognizer = new OpenCVDNNFaceRecognizer(OpenCVDNNFaceRecognizer::Tree);
}

// -------------------------------------------------------------------------------------

void FacialRecognitionWrapper::train(const QList<Identity>& identitiesToBeTrained,
                                     TrainingDataProvider* const data,
                                     const QString& trainingContext)
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    QMutexLocker lock(&d->mutex);

    d->trainIdentityBatch(identitiesToBeTrained, data, trainingContext);
}

void FacialRecognitionWrapper::train(const Identity& identityToBeTrained,
                                     TrainingDataProvider* const data,
                                     const QString& trainingContext)
{
    train((QList<Identity>() << identityToBeTrained), data, trainingContext);
}

void FacialRecognitionWrapper::train(const Identity& identityToBeTrained,
                                     QImage* image,
                                     const QString& trainingContext)
{
    RecognitionTrainingProvider* const data = new RecognitionTrainingProvider(identityToBeTrained,
                                                                              QList<QImage*>() << image);
    train(identityToBeTrained, data, trainingContext);
    delete data;
}

void FacialRecognitionWrapper::train(const Identity& identityToBeTrained,
                                     const QList<QImage*>& images,
                                     const QString& trainingContext)
{
    RecognitionTrainingProvider* const data = new RecognitionTrainingProvider(identityToBeTrained, images);
    train(identityToBeTrained, data, trainingContext);
    delete data;
}

// -------------------------------------------------------------------------------------

void FacialRecognitionWrapper::clearAllTraining(const QString& trainingContext)
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    QMutexLocker lock(&d->mutex);

    d->identityCache.clear();
    FaceDbAccess().db()->clearIdentities();

    d->clear(QList<int>(), trainingContext);
}

void FacialRecognitionWrapper::clearTraining(const QList<Identity>& identitiesToClean,
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

} // namespace Digikam
