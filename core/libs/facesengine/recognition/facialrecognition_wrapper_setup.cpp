/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : The parameters of recognition wrapper
 *
 * SPDX-FileCopyrightText: 2010      by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2010      by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2020      by Nghia Duong <minhnghiaduong997 at gmail dot com>
 * SPDX-FileCopyrightText: 2024      by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "facialrecognition_wrapper_p.h"

// Local includes

#include "dnnmodeldefinitions.h"

namespace Digikam
{

void FacialRecognitionWrapper::Private::applyParameters()
{
    int k                                           = 5;
    int threshold                                   = DNN_MODEL_THRESHOLD_NOT_SET;
    FaceScanSettings::FaceRecognitionModel oldModel = recognizeModel;

    /*
        if      (parameters.contains(QLatin1String("k-nearest")))
        {
            k = parameters.value(QLatin1String("k-nearest")).toInt();
        }
        else if (parameters.contains(QLatin1String("threshold")))
        {
            threshold = parameters.value(QLatin1String("threshold")).toFloat();
        }
        else
    */
    if (parameters.contains(QLatin1String("recognizeAccuracy")))
    {
        threshold = parameters.value(QLatin1String("recognizeAccuracy")).toInt();
    }

    if (parameters.contains(QLatin1String("recognizeModel")))
    {
        recognizeModel = static_cast<FaceScanSettings::FaceRecognitionModel>(parameters.value(QLatin1String("recognizeModel")).toInt());
    }

    // Check if d and if recModel is changing. Rebuild d if needed.

    if (recognizeModel != oldModel)
    {
        if (recognizer)
        {
            delete recognizer;
        }

        recognizer = new OpenCVDNNFaceRecognizer(OpenCVDNNFaceRecognizer::Tree, recognizeModel);
    }

    qCDebug(DIGIKAM_FACESENGINE_LOG) << "recognition threshold" << threshold;

    recognizer->setNbNeighbors(k);
    recognizer->setThreshold(threshold);
}

void FacialRecognitionWrapper::setParameter(const QString& parameter, const QVariant& value)
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    d->trainingLock.lockForWrite();

    d->parameters.insert(parameter, value);
    d->applyParameters();

    d->trainingLock.unlock();
}

void FacialRecognitionWrapper::setParameters(const QVariantMap& parameters)
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    d->trainingLock.lockForWrite();

    for (QVariantMap::const_iterator it  = parameters.begin() ;
         it != parameters.end() ;
         ++it)
    {
        d->parameters.insert(it.key(), it.value());
    }

    d->applyParameters();

    d->trainingLock.unlock();
}

void FacialRecognitionWrapper::setParameters(const FaceScanSettings& parameters)
{
    FaceScanSettings::FaceRecognitionModel oldModel = d->recognizeModel;
    int threshold                                   = parameters.recognizeAccuracy;
    d->recognizeModel                               = parameters.recognizeModel;

    // Check if d and if recModel is changing. Rebuild d if needed.

    if (d->recognizeModel != oldModel)
    {
        if (d->recognizer)
        {
            delete d->recognizer;
        }

        d->recognizer = new OpenCVDNNFaceRecognizer(OpenCVDNNFaceRecognizer::Tree, d->recognizeModel);
    }

    d->recognizer->setThreshold(threshold);
}

QVariantMap FacialRecognitionWrapper::parameters() const
{
    if (!d || !d->dbAvailable)
    {
        return QVariantMap();
    }

    d->trainingLock.lockForRead();

    QVariantMap result = d->parameters;

    d->trainingLock.unlock();

    return result;
}

} // namespace Digikam
