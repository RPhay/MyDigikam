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

/*
    NOTE: experimental and deprecated

    if ((algorithmType == RecognizeAlgorithm::EigenFace)  ||
        (algorithmType == RecognizeAlgorithm::FisherFace))
    {
        d->recognizeAlgorithm = RecognizeAlgorithm::LBP;
    }
    else
*/
    {
        d->recognizeAlgorithm = algorithmType;
    }
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

#ifdef USE_DNN_RECOGNITION_BACKEND

            if (d->recognizeAlgorithm == RecognizeAlgorithm::DNN)
            {
                // TODO : implement this routine
                //id = d->dnn()->recognize(d->preprocessingChainRGB(images->image()));
            }

#else

            if (d->recognizeAlgorithm == RecognizeAlgorithm::LBP)
            {
                id = d->lbph()->recognize(d->preprocessingChain(images->image()));
            }

#endif

/*
    NOTE: experimental and deprecated

            else if (d->recognizeAlgorithm == RecognizeAlgorithm::EigenFace)
            {
                id = d->eigen()->recognize(d->preprocessingChain(images->image()));
            }
            else if (d->recognizeAlgorithm == RecognizeAlgorithm::FisherFace)
            {
                id = d->fisher()->recognize(d->preprocessingChain(images->image()));
            }
*/

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

#ifdef USE_DNN_RECOGNITION_BACKEND

    // TODO: remove cluster function
   // d->dnn()->cluster(preprocessedImages, clusteredIndices, dataset, nbOfClusters);

#else

    Q_UNUSED(clusteredIndices)
    Q_UNUSED(dataset)
    Q_UNUSED(nbOfClusters)

#endif

}

} // namespace Digikam
