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

#include "opencvdnnfacerecognizer.h"

// Qt includes
#include <QElapsedTimer>
#include <QtConcurrent>
#include <QFuture>

// Local includes
#include "digikam_debug.h"
#include "dnnfaceextractor.h"
#include "kd_tree.h"

#include "facedbaccess.h"
#include "facedb.h"

namespace Digikam
{

class Q_DECL_HIDDEN OpenCVDNNFaceRecognizer::Private
{
public:

    Private(Classifier method)
        : method(method),
          extractor(new DNNFaceExtractor),
          tree(nullptr),
          kNeighbors(3),
          newDataAdded(true)
    {
        switch (method)
        {
            case SVM:
                svm = cv::ml::SVM::create();
                svm->setKernel(cv::ml::SVM::LINEAR);
                break;
            case OpenCV_KNN:
                knn = cv::ml::KNearest::create();
                knn->setAlgorithmType(cv::ml::KNearest::BRUTE_FORCE);
                knn->setIsClassifier(true);
                break;
            case Tree:
                tree = FaceDbAccess().db()->reconstructTree();
                break;
            case DB:
                break;
            default:
                qFatal("Invalid classifier");
        }
    }

    ~Private()
    {
        delete extractor;
        delete tree;
    }

public:

    bool trainSVM();
    bool trainKNN();

    int predictSVM(cv::Mat faceEmbedding);
    int predictKNN(cv::Mat faceEmbedding, int k);

    int predictKDTree(const cv::Mat& faceEmbedding, int k) const;
    int predictDb(const cv::Mat& faceEmbedding, int k) const;

    bool insertData(const cv::Mat& position, const int label, const QString& context = QString());

public:

    Classifier method;

    DNNFaceExtractor* extractor;
    cv::Ptr<cv::ml::SVM> svm;
    cv::Ptr<cv::ml::KNearest> knn;

    KDTree* tree;
    int kNeighbors;

    bool newDataAdded;
};

bool OpenCVDNNFaceRecognizer::Private::trainSVM()
{
    QElapsedTimer timer;
    timer.start();

    svm->train(FaceDbAccess().db()->trainData());

    qCDebug(DIGIKAM_FACEDB_LOG) << "Support vector machine trains in" << timer.elapsed() << "ms";

    return (svm->isTrained());
}

bool OpenCVDNNFaceRecognizer::Private::trainKNN()
{
    QElapsedTimer timer;
    timer.start();

    knn->train(FaceDbAccess().db()->trainData());

    qCDebug(DIGIKAM_FACEDB_LOG) << "KNN trains in" << timer.elapsed() << "ms";

    return (knn->isTrained());
}

int OpenCVDNNFaceRecognizer::Private::predictSVM(cv::Mat faceEmbedding)
{
    if (newDataAdded)
    {
        if (!trainSVM())
        {
            return -1;
        }

        newDataAdded = false;
    }

    return (int(svm->predict(faceEmbedding)));
}

int OpenCVDNNFaceRecognizer::Private::predictKNN(cv::Mat faceEmbedding, int k)
{
    if (newDataAdded)
    {
        if (!trainKNN())
        {
            return -1;
        }

        newDataAdded = false;
    }

    cv::Mat output;
    knn->findNearest(faceEmbedding, k, output);

    return (int(output.at<float>(0)));
}

int OpenCVDNNFaceRecognizer::Private::predictKDTree(const cv::Mat& faceEmbedding, int k) const
{
    if (!tree)
    {
        return -1;
    }

    // Look for K-nearest neighbor which have the sqr distance greater smaller than 1
    QMap<double, QVector<KDNode*> > closestNeighbors = tree->getClosestNeighbors(faceEmbedding, 1.0, k);

    QMap<int, QVector<double> > votingGroups;

    for (QMap<double, QVector<KDNode*> >::const_iterator iter  = closestNeighbors.cbegin();
                                                         iter != closestNeighbors.cend();
                                                       ++iter)
    {
        for (QVector<KDNode*>::const_iterator node  = iter.value().cbegin();
                                              node != iter.value().cend();
                                            ++node)
        {
            int label = (*node)->getIdentity();

            votingGroups[label].append(iter.key());
        }
    }

    double maxScore = 0;
    int prediction;

    for (QMap<int, QVector<double> >::const_iterator group  = votingGroups.cbegin();
                                                     group != votingGroups.cend();
                                                   ++group)
    {
        double score = 0;

        for (int i = 0; i < group.value().size(); ++i)
        {
            score += (1 - group.value()[i]);
        }

        if (score > maxScore)
        {
            maxScore   = score;
            prediction = group.key();
        }
    }

    return prediction;
}

int OpenCVDNNFaceRecognizer::Private::predictDb(const cv::Mat& faceEmbedding, int k) const
{
    QMap<double, QVector<int> > closestNeighbors = FaceDbAccess().db()->getClosestNeighborsTreeDb(faceEmbedding, 1.0, k);

    QMap<int, QVector<double> > votingGroups;

    for (QMap<double, QVector<int> >::const_iterator iter  = closestNeighbors.cbegin();
                                                     iter != closestNeighbors.cend();
                                                   ++iter)
    {
        for (int i = 0; i < iter.value().size(); ++i)
        {
            votingGroups[iter.value()[i]].append(iter.key());
        }
    }

    double maxScore = 0;
    int prediction;

    for (QMap<int, QVector<double> >::const_iterator group  = votingGroups.cbegin();
                                                     group != votingGroups.cend();
                                                   ++group)
    {
        double score = 0;

        for (int i = 0; i < group.value().size(); ++i)
        {
            score += (1 - group.value()[i]);
        }

        if (score > maxScore)
        {
            maxScore   = score;
            prediction = group.key();
        }
    }

    return prediction;
}

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

cv::Mat OpenCVDNNFaceRecognizer::prepareForRecognition(const QImage& inputImage)
{
    int TargetInputSize = 256;

    QImage image(inputImage);

    if ((inputImage.width() > TargetInputSize) || (inputImage.height() > TargetInputSize))
    {
        image = inputImage.scaled(TargetInputSize, TargetInputSize, Qt::IgnoreAspectRatio);
    }

    cv::Mat cvImage;    // = cv::Mat(image.height(), image.width(), CV_8UC3);
    cv::Mat cvImageWrapper;

    switch (image.format())
    {
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32:
        case QImage::Format_ARGB32_Premultiplied:

            // I think we can ignore premultiplication when converting to grayscale

            cvImageWrapper = cv::Mat(image.height(), image.width(), CV_8UC4, image.scanLine(0), image.bytesPerLine());
            cvtColor(cvImageWrapper, cvImage, CV_RGBA2RGB);

            break;

        default:
            image          = image.convertToFormat(QImage::Format_RGB888);
            cvImage        = cv::Mat(image.height(), image.width(), CV_8UC3, image.scanLine(0), image.bytesPerLine());

            //cvtColor(cvImageWrapper, cvImage, CV_RGB2GRAY);

            break;
    }
/*
    resize(cvImage, cvImage, Size(256, 256), (0, 0), (0, 0), INTER_LINEAR);
    equalizeHist(cvImage, cvImage);
*/
    return cvImage;
}

bool OpenCVDNNFaceRecognizer::Private::insertData(const cv::Mat& nodePos, const int label, const QString& context)
{
    int nodeId = FaceDbAccess().db()->insertFaceVector(nodePos, label, context);

    if (nodeId <= 0)
    {
        qCWarning(DIGIKAM_FACEDB_LOG) << "error inserting face embedding to database";
    }

    if (method == DB)
    {
        if (! FaceDbAccess().db()->insertToTreeDb(nodeId, nodePos))
        {
            qCWarning(DIGIKAM_FACEDB_LOG) << "Error insert face embedding";
            return false;
        }
    }
    else if(method == Tree)
    {
        KDNode* newNode = tree->add(nodePos, label);

        if (newNode)
        {
            newNode->setNodeId(nodeId);
        }
        else
        {
            qCWarning(DIGIKAM_FACEDB_LOG) << "Error insert new node" << nodeId;

            return false;
        }
    }

    return true;
}

void OpenCVDNNFaceRecognizer::train(const QList<QImage>& images,
                                    const int            label,
                                    const QString&       context)
{
    auto registerTraining = [this, label, context](const QImage& image)
    {
        cv::Mat faceEmbedding = d->extractor->getFaceEmbedding(prepareForRecognition(image)).clone();

        if (!d->insertData(faceEmbedding, label, context))
        {
            qCWarning(DIGIKAM_FACEDB_LOG) << "Fail to register a face of identity" << label;
        }
    };

    //QFuture<void> future = QtConcurrent::map(images, registerTraining);
    //future.waitForFinished();

    for (int i = 0; i < images.size(); ++i)
    {
        registerTraining(images[i]);
    }


    d->newDataAdded = true;
}

int OpenCVDNNFaceRecognizer::recognize(const QImage& inputImage)
{
    int id = -1;

    cv::Mat faceEmbedding = d->extractor->getFaceEmbedding(prepareForRecognition(inputImage)).clone();

    switch (d->method)
    {
        case SVM:
            id = d->predictSVM(faceEmbedding);
            break;
        case OpenCV_KNN:
            id = d->predictKNN(faceEmbedding, d->kNeighbors);
            break;
        case Tree:
            id = d->predictKDTree(faceEmbedding, d->kNeighbors);
            break;
        case DB:
            id = d->predictDb(faceEmbedding, d->kNeighbors);
            break;
        default:
            qCWarning(DIGIKAM_FACEDB_LOG) << "Not recognized classifying method";
    }

    return id;
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

    //FaceDbAccess().db()->clearTreeDb();
}

}
