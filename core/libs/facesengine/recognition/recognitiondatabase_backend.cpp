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

void RecognitionDatabase::Private::applyParameters()
{

#ifdef USE_DNN_RECOGNITION_BACKEND

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

#else

    if (
        lbphConst()
/*
    NOTE: experimental and deprecated
        || eigenConst()
        || fisherConst()
*/
       )
    {
        for (QVariantMap::const_iterator it = parameters.constBegin() ; it != parameters.constEnd() ; ++it)
        {
            if ((it.key() == QLatin1String("threshold")) || (it.key() == QLatin1String("accuracy")))
            {
                if (recognizeAlgorithm == RecognitionDatabase::RecognizeAlgorithm::LBP)
                {
                    lbph()->setThreshold(it.value().toFloat());
                }
/*
                NOTE: experimental and deprecated

                else if (recognizeAlgorithm == RecognitionDatabase::RecognizeAlgorithm::EigenFace)
                {
                    eigen()->setThreshold(it.value().toFloat());
                }
                else if (recognizeAlgorithm == RecognitionDatabase::RecognizeAlgorithm::FisherFace)
                {
                    fisher()->setThreshold(it.value().toFloat());
                }
*/
                else
                {
                    qCCritical(DIGIKAM_FACESENGINE_LOG) << "No obvious recognize algorithm";
                }
            }
        }
    }

#endif

}

// --------------------------------------------------------------------------

QString RecognitionDatabase::backendIdentifier() const
{

#ifdef USE_DNN_RECOGNITION_BACKEND

    if (d->recognizeAlgorithm == RecognizeAlgorithm::DNN)
    {
        return QLatin1String("dnn");
    }

#else

    if (d->recognizeAlgorithm == RecognizeAlgorithm::LBP)
    {
        return QLatin1String("opencvlbph");
    }

#endif

/*
    NOTE: experimental and deprecated

    if (d->recognizeAlgorithm == RecognizeAlgorithm::EigenFace)
    {
        return QLatin1String("eigenfaces");
    }

    if (d->recognizeAlgorithm == RecognizeAlgorithm::FisherFace)
    {
        return QLatin1String("fisherfaces");
    }
*/
    return QString();
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

#ifdef USE_DNN_RECOGNITION_BACKEND

    OpenCVDNNFaceRecognizer::m_threshold = threshold;

#else

    Q_UNUSED(threshold)

#endif

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

} // namespace Digikam
