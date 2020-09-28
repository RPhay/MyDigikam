/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2020-05-22
 * Description : Wrapper of face recognition using OpenFace
 *
 * Copyright (C) 2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#include "opencvdnnfacerecognizer_p.h"

namespace Digikam
{

OpenCVDNNFaceRecognizer::OpenCVDNNFaceRecognizer(Classifier method)
    : d(new Private(method))
{
}

OpenCVDNNFaceRecognizer::~OpenCVDNNFaceRecognizer()
{
    delete d;
}

void OpenCVDNNFaceRecognizer::setNbNeighBors(int k)
{
    d->kNeighbors = k;
}

void OpenCVDNNFaceRecognizer::setThreshold(float threshold)
{
    d->threshold = threshold;
}

cv::Mat OpenCVDNNFaceRecognizer::prepareForRecognition(QImage& inputImage)
{
    cv::Mat cvImage;    // = cv::Mat(image.height(), image.width(), CV_8UC3);
    cv::Mat cvImageWrapper;

    if (inputImage.format() != QImage::Format_ARGB32_Premultiplied)
    {
        inputImage = inputImage.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    }

    cvImageWrapper = cv::Mat(inputImage.height(), inputImage.width(), CV_8UC4, inputImage.scanLine(0), inputImage.bytesPerLine());
    cv::cvtColor(cvImageWrapper, cvImage, CV_RGBA2RGB);

/*
    resize(cvImage, cvImage, Size(256, 256), (0, 0), (0, 0), cv::INTER_LINEAR);
    equalizeHist(cvImage, cvImage);
*/
    return cvImage;
}

cv::Mat OpenCVDNNFaceRecognizer::prepareForRecognition(const cv::Mat& cvInputImage)
{
    int TargetInputSize = 256;

    cv::Mat cvOutputImage;

    cv::resize(cvInputImage, cvOutputImage, cv::Size(TargetInputSize, TargetInputSize));

    return cvOutputImage;
}


void OpenCVDNNFaceRecognizer::train(const QList<QImage*>& images,
                                    const int             label,
                                    const QString&        context)
{
    cv::parallel_for_(cv::Range(0, images.size()), Private::ParallelTrainer(d, images, label, context));

    d->newDataAdded = true;
}

int OpenCVDNNFaceRecognizer::recognize(QImage* inputImage)
{
    int id = -1;

    cv::Mat faceEmbedding = d->extractors[0]->getFaceEmbedding(prepareForRecognition(*inputImage));

    switch (d->method)
    {
        case SVM:
        {
            id = d->predictSVM(faceEmbedding);
            break;
        }

        case OpenCV_KNN:
        {
            id = d->predictKNN(faceEmbedding);
            break;
        }

        case Tree:
        {
            id = d->predictKDTree(faceEmbedding);
            break;
        }

        case DB:
        {
            id = d->predictDb(faceEmbedding);
            break;
        }

        default:
        {
            qCWarning(DIGIKAM_FACEDB_LOG) << "Not recognized classifying method";
        }
    }

    return id;
}

QVector<int> OpenCVDNNFaceRecognizer::recognize(const QList<QImage*>& inputImages)
{
    QVector<int> ids;

    cv::parallel_for_(cv::Range(0, inputImages.size()), Private::ParallelRecognizer(d, inputImages, ids));

    return ids;
}


void OpenCVDNNFaceRecognizer::clearTraining(const QList<int>& idsToClear, const QString& trainingContext)
{
    if (idsToClear.isEmpty())
    {
        FaceDbAccess().db()->clearDNNTraining(trainingContext);
    }
    else
    {
        FaceDbAccess().db()->clearDNNTraining(idsToClear, trainingContext);
    }
/*
    FaceDbAccess().db()->clearTreeDb();
*/
}

bool OpenCVDNNFaceRecognizer::registerTrainingData(const cv::Mat& preprocessedImage, int label)
{
    cv::Mat faceEmbedding = d->extractors[0]->getFaceEmbedding(preprocessedImage);

    if (d->method == Tree)
    {
        KDNode* const newNode = d->tree->add(faceEmbedding, label);

        if (!newNode)
        {
            qCWarning(DIGIKAM_FACEDB_LOG) << "Error insert new node";

            return false;
        }
    }

    return true;
}

int OpenCVDNNFaceRecognizer::verifyTestData(const cv::Mat& preprocessedImage)
{
    int id = -1;

    if (d->method == Tree)
    {
        id = d->predictKDTree(d->extractors[0]->getFaceEmbedding(preprocessedImage));
    }

    return id;
}

} // namespace Digikam
