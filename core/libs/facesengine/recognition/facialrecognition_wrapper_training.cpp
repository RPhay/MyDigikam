/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : Training functions of recognition wrapper
 *
 * SPDX-FileCopyrightText: 2010      by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2010      by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * SPDX-FileCopyrightText: 2010-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2020      by Nghia Duong <minhnghiaduong997 at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "facialrecognition_wrapper_p.h"

namespace Digikam
{

void FacialRecognitionWrapper::Private::trainIdentityBatch(const QList<Identity>&      identitiesToBeTrained,
                                                           TrainingDataProvider* const data)
{
    for (const Identity& identity : std::as_const(identitiesToBeTrained))
    {
        // TODO: possible memory leak?
        //       Where/when do the images get deleted?
        //       Need to investigate.

        ImageListProvider* const imageList     = data->newImages(identity);
        QList<QPair<QImage*, QString> > images = imageList->images();

        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Training" << images.size() << "images for identity" << identity.id();

        try
        {
            recognizer->train(images, identity.id());
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

void FacialRecognitionWrapper::Private::clear(const QList<int>& idsToClear)
{
    recognizer->clearTraining(idsToClear);

    delete recognizer;

    recognizer = new OpenCVDNNFaceRecognizer(OpenCVDNNFaceRecognizer::Tree, recognizeModel);
}

void FacialRecognitionWrapper::Private::clear(const QString& hash)
{
    trainingLock.lockForWrite();

    recognizer[0].remove(hash);

    trainingLock.unlock();
}

// -------------------------------------------------------------------------------------

void FacialRecognitionWrapper::train(const QList<Identity>& identitiesToBeTrained,
                                     TrainingDataProvider* const data)
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    d->trainingLock.lockForWrite();

    d->trainIdentityBatch(identitiesToBeTrained, data);

    d->trainingLock.unlock();
}

void FacialRecognitionWrapper::train(const Identity& identityToBeTrained,
                                     TrainingDataProvider* const data)
{
    train((QList<Identity>() << identityToBeTrained), data);
}

void FacialRecognitionWrapper::train(const Identity& identityToBeTrained,
                                     const QPair<QImage*, QString>& image)
{
    RecognitionTrainingProvider* const data = new RecognitionTrainingProvider(identityToBeTrained,
                                                                              QList<QPair<QImage*, QString> >() << image);
    train(identityToBeTrained, data);

    delete data;
}

void FacialRecognitionWrapper::train(const Identity& identityToBeTrained,
                                     const QList<QPair<QImage*, QString> >& images)
{
    RecognitionTrainingProvider* const data = new RecognitionTrainingProvider(identityToBeTrained, images);
    train(identityToBeTrained, data);

    delete data;
}

// -------------------------------------------------------------------------------------

void FacialRecognitionWrapper::clearAllTraining()
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    d->trainingLock.lockForWrite();

    d->identityCache.clear();
    FaceDbAccess().db()->clearIdentities();

    d->clear(QList<int>());

    d->trainingLock.unlock();
}

void FacialRecognitionWrapper::clearTraining(const QList<Identity>& identitiesToClean)
{
    if (!d || !d->dbAvailable || identitiesToClean.isEmpty())
    {
        return;
    }

    d->trainingLock.lockForWrite();

    QList<int> ids;

    for (const Identity& id : std::as_const(identitiesToClean))
    {
        ids << id.id();
    }

    d->clear(ids);

    d->trainingLock.unlock();
}

/*
void FacialRecognitionWrapper::clearTraining(const QString& hash)
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    d->trainingLock.lockForWrite();

    d->clear(hash);

    d->trainingLock.unlock();
}
*/

} // namespace Digikam
