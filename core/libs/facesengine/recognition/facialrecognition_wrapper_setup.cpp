/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : The parameters of recognition wrapper
 *
 * SPDX-FileCopyrightText:      2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText:      2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText:      2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText:      2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "facialrecognition_wrapper_p.h"

namespace Digikam
{

void FacialRecognitionWrapper::Private::applyParameters()
{
    int k           = 5;
    float threshold = 0.6F;

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
