/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : Training functions of recognition wrapper
 *
 * Copyright (C)      2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C)      2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "facesengine_interface_p.h"

namespace Digikam
{

void FacesEngineInterface::Private::trainIdentityBatch(const QList<Identity>& identitiesToBeTrained,
                                                       TrainingDataProvider* const data,
                                                       const QString& trainingContext)
{
    foreach (const Identity& identity, identitiesToBeTrained)
    {
        ImageListProvider* const imageList = data->newImages(identity);

        QList<QImage> images = imageList->images();

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

void FacesEngineInterface::train(const QList<Identity>& identitiesToBeTrained,
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

void FacesEngineInterface::train(const Identity& identityToBeTrained,
                                 TrainingDataProvider* const data,
                                 const QString& trainingContext)
{
    train(QList<Identity>() << identityToBeTrained, data, trainingContext);
}

void FacesEngineInterface::train(const Identity& identityToBeTrained,
                                 const QImage& image,
                                 const QString& trainingContext)
{
    RecognitionTrainingProvider* const data = new RecognitionTrainingProvider(identityToBeTrained,
                                                                              QList<QImage>() << image);
    train(identityToBeTrained, data, trainingContext);
    delete data;
}

void FacesEngineInterface::train(const Identity& identityToBeTrained,
                                 const QList<QImage>& images,
                                 const QString& trainingContext)
{
    RecognitionTrainingProvider* const data = new RecognitionTrainingProvider(identityToBeTrained, images);
    train(identityToBeTrained, data, trainingContext);
    delete data;
}

} // namespace Digikam
