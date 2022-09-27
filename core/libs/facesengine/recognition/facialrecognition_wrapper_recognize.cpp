/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : Recognizing functions of recognition wrapper
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

QList<Identity> FacialRecognitionWrapper::recognizeFaces(ImageListProvider* const images)
{
    if (!d || !d->dbAvailable)
    {
        return QList<Identity>();
    }

    QMutexLocker lock(&d->mutex);

    QVector<int> ids;

    try
    {
        ids = d->recognizer->recognize(images->images());
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";
    }

    QList<Identity> results;

    for (int i = 0 ; i < ids.size() ; ++i)
    {
        results << d->identityCache.value(ids.at(i));
    }

    return results;
}

QList<Identity> FacialRecognitionWrapper::recognizeFaces(const QList<QImage*>& images)
{
    QListImageListProvider provider;
    provider.setImages(images);

    return recognizeFaces(&provider);
}

Identity FacialRecognitionWrapper::recognizeFace(QImage* const image)
{
    QList<Identity> result = recognizeFaces(QList<QImage*>() << image);

    if (result.isEmpty())
    {
        return Identity();
    }

    return result.first();
}

} // namespace Digikam
