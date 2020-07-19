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
#include "facedatabase.h"
#include "spatial_database.h"
#include "faceembedding_db.h"

using namespace Digikam;

namespace RecognitionTest
{

class Q_DECL_HIDDEN FaceRecognizer::Private
{
public:

    Private(Classifier method)
        : method(method),
          extractor(new FaceExtractor),
          facedb(new FaceDatabase),
          embeddingDb(new FaceEmbeddingDb),
          treedb(nullptr),
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
                tree = embeddingDb->reconstructTree();
                break;
            case DB:
                treedb = new SpatialDatabase();
                break;
            default:
                qFatal("Invalid classifier");
        }
    }

    ~Private()
    {
        delete extractor;
        delete tree;
        delete facedb;
        delete embeddingDb;
        delete treedb;
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

    FaceDatabase* facedb;
    FaceEmbeddingDb* embeddingDb;
    SpatialDatabase* treedb;

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

    svm->train(embeddingDb->trainData());

    qDebug() << "Support vector machine trains in" << timer.elapsed() << "ms";

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

    knn->train(embeddingDb->trainData());

    qDebug() << "KNN trains in" << timer.elapsed() << "ms";

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
    if (! tree)
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
            int label = (*node)->getIdentity().id();

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
    if (!treedb)
    {
        return -1;
    }

    QMap<double, QVector<int> > closestNeighbors = treedb->getClosestNeighbors(faceEmbedding, 1.0, k);

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

Identity FaceRecognizer::findIdenity(const cv::Mat& preprocessedImage)
{
    int id = -1;

    switch (d->method)
    {
        case SVM:
            id = d->predictSVM(d->extractor->getFaceDescriptor(preprocessedImage));
            break;
        case OpenCV_KNN:
            id = d->predictKNN(d->extractor->getFaceDescriptor(preprocessedImage), d->kNeighbors);
            break;
        case Tree:
            id = d->predictKDTree(d->extractor->getFaceDescriptor(preprocessedImage), d->kNeighbors);
            break;
        case DB:
            id = d->predictDb(d->extractor->getFaceDescriptor(preprocessedImage), d->kNeighbors);
            break;
    }

    Identity identity;
    identity.setId(id);

    if (id > 0)
    {
        QString label = d->facedb->queryLabel(id);
        identity.setAttribute(QLatin1String("fullName"), label);
    }

    return identity;
}

Identity FaceRecognizer::newIdentity(const cv::Mat& preprocessedImage)
{
    std::vector<float> faceEmbedding = d->extractor->getFaceEmbedding(preprocessedImage);

    // new identity
    Identity id;
    QJsonArray jsonFaceEmbedding = FaceExtractor::encodeVector(faceEmbedding);
    id.setAttribute(QLatin1String("faceEmbedding"),
                    QString::fromLatin1(QJsonDocument(jsonFaceEmbedding).toJson(QJsonDocument::Compact)));

    return id;
}

int FaceRecognizer::saveIdentity(Identity& id, bool newLabel)
{
    if (newLabel)
    {
        QString label = id.attribute(QLatin1String("fullName"));

        if (label.isEmpty())
        {
            qWarning() << "new Identity isn't labeled";

            return -1;
        }

        // register
        int index = d->facedb->registerLabel(label);

        id.setId(index);
    }
    else if (id.isNull())
    {
        qWarning() << "new id is null";
        return -1;
    }

    int index = id.id();

    if (index < 0)
    {
        qWarning() << "Error insert label";
        return -1;
    }

    QJsonArray jsonFaceEmbedding = QJsonDocument::fromJson(id.attribute(QLatin1String("faceEmbedding")).toLatin1()).array();
    cv::Mat recordedFaceEmbedding = FaceExtractor::vectortomat(FaceExtractor::decodeVector(jsonFaceEmbedding));

    d->embeddingDb->insert(recordedFaceEmbedding, index);

    if (d->method == DB)
    {
        if (! d->treedb->insert(recordedFaceEmbedding, index))
        {
            qWarning() << "Error insert face embedding";
            return -1;
        }
    }
    else if(d->method == Tree)
    {
        d->tree->add(recordedFaceEmbedding, id);
    }

    return index;
}

bool FaceRecognizer::insertData(const cv::Mat& nodePos, const int label)
{
    if (!d->treedb)
    {
        return false;
    }

    return d->treedb->insert(nodePos, label);
}

QMap<double, QVector<int> > FaceRecognizer::getClosestNodes(const cv::Mat& position,
                                                            double sqRange,
                                                            int maxNbNeighbors)
{
    if (!d->treedb)
    {
        return QMap<double, QVector<int> >();
    }

    return d->treedb->getClosestNeighbors(position, sqRange, maxNbNeighbors);
}

}
