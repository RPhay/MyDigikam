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

// cv include
#include <opencv2/ml.hpp>

// Local includes
#include "dnnfacemodel.h"
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

    Private(bool debug)
        : debugMode(debug),
          identityCounter(0),
          extractor(new FaceExtractor),
          tree(embeddingDb.reconstructTree())
    {
        QFileInfo svmWeightsFile(svmFile);
        if (svmWeightsFile.exists())
        {
            svm = cv::ml::SVM::load(svmFile.toStdString());
        }
        else
        {
            svm = cv::ml::SVM::create();
        }

        // use linear mapping
        svm->setKernel(cv::ml::SVM::LINEAR);
        QFileInfo knnWeightsFile(knnFile);

        if (knnWeightsFile.exists())
        {
            knn = cv::ml::KNearest::load(knnFile.toStdString());
        }
        else
        {
            knn = cv::ml::KNearest::create();
        }

        knn->setAlgorithmType(cv::ml::KNearest::BRUTE_FORCE);
        knn->setIsClassifier(true);
    }

    ~Private()
    {
        if (knn->isTrained())
        {
            knn->save(knnFile.toStdString());
        }

        if (svm->isTrained())
        {
            svm->save(svmFile.toStdString());
        }

        delete extractor;
    }

public:

    bool trainSVM() const;
    bool trainKNN() const;

    int predictSVM(cv::Mat faceEmbedding) const;
    int predictKNN(cv::Mat faceEmbedding) const;

    int predictKDTree(const cv::Mat& faceEmbedding, int k) const;
    int predictDb(const cv::Mat& faceEmbedding, int k) const;

public:

    bool debugMode;
    int identityCounter;

    FaceExtractor* extractor;
    cv::Ptr<cv::ml::SVM> svm;
    cv::Ptr<cv::ml::KNearest> knn;

    // TODO put file names to Settings
    const QString knnFile = QLatin1String("knn.bin");
    const QString svmFile = QLatin1String("svm.bin");

    FaceDatabase db;
    FaceEmbeddingDb embeddingDb;
    SpatialDatabase treedb;
    KDTree tree;
};

bool FaceRecognizer::Private::trainSVM() const
{
    QElapsedTimer timer;
    timer.start();

    cv::Ptr<cv::ml::TrainData> trainData = embeddingDb.trainData();
    qDebug() << "train samples:" << trainData->getNSamples();
    svm->train(trainData);

    qDebug() << "Support vector machine trains in" << timer.elapsed() << "ms";

    return (svm->isTrained());
}

bool FaceRecognizer::Private::trainKNN() const
{
    QElapsedTimer timer;
    timer.start();

    knn->train(embeddingDb.trainData());

    qDebug() << "KNN trains in" << timer.elapsed() << "ms";

    return (knn->isTrained());
}

int FaceRecognizer::Private::predictSVM(cv::Mat faceEmbedding) const
{
    if (!svm->isTrained())
    {
        trainSVM();
    }

    return (int(svm->predict(faceEmbedding)));
}

int FaceRecognizer::Private::predictKNN(cv::Mat faceEmbedding) const
{
    if (!knn->isTrained())
    {
        trainKNN();
    }

    cv::Mat output;
    knn->findNearest(faceEmbedding, 3, output);

    return (int(output.at<float>(0)));
}

int FaceRecognizer::Private::predictKDTree(const cv::Mat& faceEmbedding, int k) const
{
    // Look for K-nearest neighbor which have the sqr distance greater smaller than 1
    QMap<double, QVector<KDNode*> > closestNeighbors = tree.getClosestNeighbors(faceEmbedding, 1.0, k);

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
    QMap<double, QVector<int> > closestNeighbors = treedb.getClosestNeighbors(faceEmbedding, 1.0, k);

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

FaceRecognizer::FaceRecognizer(bool debug)
    : d(new Private(debug))
{
}

FaceRecognizer::~FaceRecognizer()
{
    delete d;
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

Identity FaceRecognizer::findIdenity(const cv::Mat& preprocessedImage, ComparisonMetric metric, double threshold)
{
    int id;

    switch (metric)
    {
        case SVM:
            id = d->predictSVM(d->extractor->getFaceDescriptor(preprocessedImage));
            break;
        case KNN:
            id = d->predictKNN(d->extractor->getFaceDescriptor(preprocessedImage));
            break;
        case Tree:
            id = d->predictKDTree(d->extractor->getFaceDescriptor(preprocessedImage), (int)threshold);
            break;
        case DB:
            id = d->predictDb(d->extractor->getFaceDescriptor(preprocessedImage), (int)threshold);
            break;
    }

    QString label = d->db.queryLabel(id);

    Identity identity;
    identity.setId(id);
    identity.setAttribute(QLatin1String("fullName"), label);

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
        int index = d->db.registerLabel(label);

        id.setId(index);

        qDebug() << "label" << label << "registered with key" << index;
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
/*
    if (! d->treedb.insert(recordedFaceEmbedding, index))
    {
        qWarning() << "Error insert face embedding";
        return -1;
    }
*/
    // Create a KD-Node for this identity
    d->tree.add(recordedFaceEmbedding, id);
    d->embeddingDb.insert(recordedFaceEmbedding, index);

    return index;
}

bool FaceRecognizer::insertData(const cv::Mat& nodePos, const int label)
{
    return d->treedb.insert(nodePos, label);
}

QMap<double, QVector<int> > FaceRecognizer::getClosestNodes(const cv::Mat& position,
                                                            double sqRange,
                                                            int maxNbNeighbors)
{
    return d->treedb.getClosestNeighbors(position, sqRange, maxNbNeighbors);
}

}
