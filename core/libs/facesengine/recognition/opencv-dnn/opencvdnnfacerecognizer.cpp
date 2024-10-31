/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2020-05-22
 * Description : Wrapper of face recognition using OpenFace.
 *
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2020-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2020      by Nghia Duong <minhnghiaduong997 at gmail dot com>
 * SPDX-FileCopyrightText: 2024      by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "opencvdnnfacerecognizer_p.h"

namespace Digikam
{

OpenCVDNNFaceRecognizer::Private* OpenCVDNNFaceRecognizer::d = nullptr;

OpenCVDNNFaceRecognizer::OpenCVDNNFaceRecognizer(Classifier method, FaceScanSettings::FaceRecognitionModel recModel)
{
    if (!d)
    {
        d = new Private(method, recModel);
    }
    else
    {
        ++(d->ref);
    }
}

OpenCVDNNFaceRecognizer::~OpenCVDNNFaceRecognizer()
{
    --(d->ref);

    if (0 == d->ref)
    {
        delete d;
        d = nullptr;
    }
}

void OpenCVDNNFaceRecognizer::setNbNeighbors(int k)
{
    d->kNeighbors = k;
}

void OpenCVDNNFaceRecognizer::setThreshold(int threshold)
{
    d->uiThreshold = threshold;
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

void OpenCVDNNFaceRecognizer::train(const QList<QPair<QImage*,
                                    QString>>& images,
                                    const int label)
{
    cv::parallel_for_(cv::Range(0, images.size()), Private::ParallelTrainer(d, images, label));

    d->newDataAdded = true;
}

bool OpenCVDNNFaceRecognizer::remove(const QString& hash)
{

    bool result = FaceDbAccess().db()->removeFaceVector(hash);

    if (result)
    {
        // rebuild the tree

        if (d->tree)
        {
            delete d->tree;
            d->tree = nullptr; // safety in case reconstructTree fails
        }

        d->tree = FaceDbAccess().db()->reconstructTree(d->recognizeModel);
    }

    return result;
}

int OpenCVDNNFaceRecognizer::recognize(QPair<QImage*, QString> inputImage)
{
    int id = -1;

    cv::Mat faceEmbedding = d->extractors[0]->getFaceEmbedding(prepareForRecognition(*(inputImage.first)));

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

QVector<int> OpenCVDNNFaceRecognizer::recognize(const QList<QPair<QImage*, QString>>& inputImages)
{
    QVector<int> ids;

    cv::parallel_for_(cv::Range(0, inputImages.size()), Private::ParallelRecognizer(d, inputImages, ids));

    return ids;
}

void OpenCVDNNFaceRecognizer::clearTraining(const QList<int>& idsToClear)
{
    if (idsToClear.isEmpty())
    {
        FaceDbAccess().db()->clearDNNTraining();
    }
    else
    {
        FaceDbAccess().db()->clearDNNTraining(idsToClear);
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
        KDNodeBase* const newNode = d->tree->add(faceEmbedding, label);

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
