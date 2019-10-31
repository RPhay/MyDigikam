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

RecognitionDatabase::RecognitionDatabase()
    : d(new Private)
{
}

RecognitionDatabase::~RecognitionDatabase()
{
    delete d;
}

RecognitionDatabase::RecognitionDatabase(const RecognitionDatabase& other)
    : d(other.d)
{
}

QString RecognitionDatabase::backendIdentifier() const
{
    if      (d->recognizeAlgorithm == RecognizeAlgorithm::EigenFace)
    {
        return QLatin1String("eigenfaces");
    }
    else if (d->recognizeAlgorithm == RecognizeAlgorithm::DNN)
    {
        return QLatin1String("dnn");
    }
    else if (d->recognizeAlgorithm == RecognizeAlgorithm::FisherFace)
    {
        return QLatin1String("fisherfaces");
    }

    // d->recognizeAlgorithm == RecognizeAlgorithm::LPB
    return QLatin1String("opencvlbph");
}

void RecognitionDatabase::setParameter(const QString& parameter, const QVariant& value)
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    QMutexLocker lock(&d->mutex);

    d->parameters.insert(parameter, value);
    d->applyParameters();
}

void RecognitionDatabase::setParameters(const QVariantMap& parameters)
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    QMutexLocker lock(&d->mutex);

    for (QVariantMap::const_iterator it = parameters.begin() ; it != parameters.end() ; ++it)
    {
        d->parameters.insert(it.key(), it.value());
    }

    d->applyParameters();
}

void RecognitionDatabase::setRecognizerThreshold(float threshold)
{
    OpenCVDNNFaceRecognizer::m_threshold = threshold;
}

QVariantMap RecognitionDatabase::parameters() const
{
    if (!d || !d->dbAvailable)
    {
        return QVariantMap();
    }

    QMutexLocker lock(&d->mutex);

    return d->parameters;
}

int RecognitionDatabase::recommendedImageSize(const QSize& availableSize) const
{
    // hardcoded for now, change when we know better.
    Q_UNUSED(availableSize)

    return 256;
}

Identity RecognitionDatabase::recognizeFace(const QImage& image)
{
    QList<Identity> result = recognizeFaces(QList<QImage>() << image);

    if (result.isEmpty())
    {
        return Identity();
    }

    return result.first();
}

QList<Identity> RecognitionDatabase::recognizeFaces(const QList<QImage>& images)
{
    QListImageListProvider provider(images);

    return recognizeFaces(&provider);
}

void RecognitionDatabase::activeFaceRecognizer(RecognizeAlgorithm algorithmType)
{
    if (algorithmType == RecognizeAlgorithm::EigenFace ||
        algorithmType == RecognizeAlgorithm::FisherFace)
    {
        d->recognizeAlgorithm = RecognizeAlgorithm::LBP;
    }
    else
    {
        d->recognizeAlgorithm = algorithmType;
    }
}

void RecognitionDatabase::createDNNDebug()
{
    d->createDNNDebug();
}

QList<Identity> RecognitionDatabase::recognizeFaces(ImageListProvider* const images)
{
    if (!d || !d->dbAvailable)
    {
        return QList<Identity>();
    }

    QMutexLocker lock(&d->mutex);

    QList<Identity> result;

    for ( ; !images->atEnd() ; images->proceed())
    {
        int id = -1;

        try
        {
            if      (d->recognizeAlgorithm == RecognizeAlgorithm::LBP)
            {
                id = d->lbph()->recognize(d->preprocessingChain(images->image()));
            }
            else if (d->recognizeAlgorithm == RecognizeAlgorithm::EigenFace)
            {
                id = d->eigen()->recognize(d->preprocessingChain(images->image()));
            }
            else if (d->recognizeAlgorithm == RecognizeAlgorithm::FisherFace)
            {
                id = d->fisher()->recognize(d->preprocessingChain(images->image()));
            }
            else if (d->recognizeAlgorithm == RecognizeAlgorithm::DNN)
            {
                id = d->dnn()->recognize(d->preprocessingChainRGB(images->image()));
            }
            else
            {
                qCCritical(DIGIKAM_FACESENGINE_LOG) << "No obvious recognize algorithm";
            }
        }
        catch (cv::Exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception:" << e.what();
        }
        catch (...)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
        }

        if (id == -1)
        {
            result << Identity();
        }
        else
        {
            result << d->identityCache.value(id);
        }
    }

    return result;
}

void RecognitionDatabase::clusterFaces(const QList<QImage>& images,
                                       std::vector<int>& clusteredIndices,
                                       QStringList dataset,
                                       int nbOfClusters) const
{
    std::vector<cv::Mat> preprocessedImages;

    foreach (const QImage& image, images)
    {
        preprocessedImages.push_back(d->preprocessingChainRGB(image));
    }

    d->dnn()->cluster(preprocessedImages, clusteredIndices, dataset, nbOfClusters);
}

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

bool RecognitionDatabase::integrityCheck()
{
    if (!d || !d->dbAvailable)
    {
        return false;
    }

    QMutexLocker lock(&d->mutex);

    return FaceDbAccess().db()->integrityCheck();
}

void RecognitionDatabase::vacuum()
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    QMutexLocker lock(&d->mutex);

    return FaceDbAccess().db()->vacuum();
}

} // namespace Digikam
