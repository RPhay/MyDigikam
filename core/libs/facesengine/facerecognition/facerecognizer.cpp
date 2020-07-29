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

#include "facerecognizer.h"

// Qt includes
#include <QElapsedTimer>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFileInfo>

// Local includes
#include "digikam_debug.h"
#include "faceextractor.h"
#include "spatial_database.h"

#include "facedbaccess.h"
#include "facedb.h"

using namespace Digikam;

namespace RecognitionTest
{

class Q_DECL_HIDDEN FaceRecognizer::Private
{
public:

    Private(Classifier method)
        : method(method),
          extractor(new FaceExtractor),
          //treedb(nullptr),
          tree(nullptr),
          kNeighbors(3)
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
                //treedb = new SpatialDatabase();
                break;
            default:
                qFatal("Invalid classifier");
        }
    }

    ~Private()
    {
        delete extractor;
        delete tree;
        //delete treedb;
    }

public:

    bool trainSVM() const;
    bool trainKNN() const;

    int predictSVM(cv::Mat faceEmbedding) const;
    int predictKNN(cv::Mat faceEmbedding, int k) const;

    int predictKDTree(const cv::Mat& faceEmbedding, int k) const;
    int predictDb(const cv::Mat& faceEmbedding, int k) const;

public:

    Classifier method;

    FaceExtractor* extractor;
    cv::Ptr<cv::ml::SVM> svm;
    cv::Ptr<cv::ml::KNearest> knn;

    //SpatialDatabase* treedb;

    KDTree* tree;
    int kNeighbors;
};


bool FaceRecognizer::Private::trainSVM() const
{
    if (svm->empty())
    {
        return false;
    }

    QElapsedTimer timer;
    timer.start();

    svm->train(FaceDbAccess().db()->trainData());

    qCDebug(DIGIKAM_FACEDB_LOG) << "Support vector machine trains in" << timer.elapsed() << "ms";

    return (svm->isTrained());
}

bool FaceRecognizer::Private::trainKNN() const
{
    if (knn->empty())
    {
        return false;
    }

    QElapsedTimer timer;
    timer.start();

    knn->train(FaceDbAccess().db()->trainData());

    qCDebug(DIGIKAM_FACEDB_LOG) << "KNN trains in" << timer.elapsed() << "ms";

    return (knn->isTrained());
}

int FaceRecognizer::Private::predictSVM(cv::Mat faceEmbedding) const
{
    if (!svm->isTrained())
    {
        if (!trainSVM())
        {
            return -1;
        }
    }

    return (int(svm->predict(faceEmbedding)));
}

int FaceRecognizer::Private::predictKNN(cv::Mat faceEmbedding, int k) const
{
    if (!knn->isTrained())
    {
        if (!trainKNN())
        {
            return -1;
        }
    }

    cv::Mat output;
    knn->findNearest(faceEmbedding, k, output);

    return (int(output.at<float>(0)));
}

int FaceRecognizer::Private::predictKDTree(const cv::Mat& faceEmbedding, int k) const
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

int FaceRecognizer::Private::predictDb(const cv::Mat& faceEmbedding, int k) const
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

FaceRecognizer::FaceRecognizer(Classifier method)
    : d(new Private(method))
{
}

FaceRecognizer::~FaceRecognizer()
{
    delete d;
}

void FaceRecognizer::setNbNeighBors(int k)
{
    d->kNeighbors = k;
}

cv::Mat FaceRecognizer::prepareForRecognition(const QImage& inputImage)
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

bool FaceRecognizer::insertData(const cv::Mat& nodePos, const int label, const QString& context)
{
    int nodeId = FaceDbAccess().db()->insertFaceVector(nodePos, label, context);

    if (nodeId <= 0)
    {
        qWarning() << "error inserting face embedding to database";
    }

    if (d->method == DB)
    {
        if (! FaceDbAccess().db()->insertToTreeDb(nodeId, nodePos))
        {
            qWarning() << "Error insert face embedding";
            return false;
        }
    }
    else if(d->method == Tree)
    {
        KDNode* newNode = d->tree->add(nodePos, label);

        if (newNode)
        {
            newNode->setNodeId(nodeId);
        }
        else
        {
            qWarning() << "Error insert new node" << nodeId;

            return false;
        }
    }

    return true;
}

void FaceRecognizer::train(const QList<QImage>& images,
                           const int label,
                           const QString& context)
{
    for (QList<QImage>::const_iterator image  = images.cbegin();
                                       image != images.cend();
                                     ++image)
    {
        cv::Mat faceEmbedding = d->extractor->getFaceDescriptor(prepareForRecognition(*image)).clone();

        if (!insertData(faceEmbedding, label, context))
        {
            qWarning() << "Fail to register a face of identity" << label;
        }
    }
}

int FaceRecognizer::recognize(const QImage& inputImage)
{
    int id = -1;

    cv::Mat faceEmbedding = d->extractor->getFaceDescriptor(prepareForRecognition(inputImage)).clone();

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
            qWarning() << "Not recognized classifying method";
    }

    return id;
}


QMap<double, QVector<int> > FaceRecognizer::getClosestNodes(const cv::Mat& position,
                                                            double sqRange,
                                                            int maxNbNeighbors)
{
    return FaceDbAccess().db()->getClosestNeighborsTreeDb(position, sqRange, maxNbNeighbors);
}

}
