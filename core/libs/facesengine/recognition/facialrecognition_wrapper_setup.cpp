/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : The parameters of recognition wrapper
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

void FacialRecognitionWrapper::Private::applyParameters()
{
    int k           = 5;
    float threshold = 0.6;

    if      (parameters.contains(QLatin1String("k-nearest")))
    {
        k = parameters.value(QLatin1String("k-nearest")).toInt();
    }
    else if (parameters.contains(QLatin1String("threshold")))
    {
        threshold = parameters.value(QLatin1String("threshold")).toFloat();
    }
    else if (parameters.contains(QLatin1String("accuracy")))
    {
        threshold = parameters.value(QLatin1String("accuracy")).toFloat();
    }

    threshold = 1 - threshold;

    qCDebug(DIGIKAM_FACESENGINE_LOG) << "recognition threshold" << threshold;

    recognizer->setNbNeighBors(k);
    recognizer->setThreshold(threshold);
}

void FacialRecognitionWrapper::setParameter(const QString& parameter, const QVariant& value)
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    QMutexLocker lock(&d->mutex);

    d->parameters.insert(parameter, value);
    d->applyParameters();
}

void FacialRecognitionWrapper::setParameters(const QVariantMap& parameters)
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    QMutexLocker lock(&d->mutex);

    for (QVariantMap::const_iterator it  = parameters.begin() ;
                                     it != parameters.end() ;
                                     ++it)
    {
        d->parameters.insert(it.key(), it.value());
    }

    d->applyParameters();
}

QVariantMap FacialRecognitionWrapper::parameters() const
{
    if (!d || !d->dbAvailable)
    {
        return QVariantMap();
    }

    QMutexLocker lock(&d->mutex);

    return d->parameters;
}

} // namespace Digikam
