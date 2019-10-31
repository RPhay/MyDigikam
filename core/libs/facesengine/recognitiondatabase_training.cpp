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

} // namespace Digikam
