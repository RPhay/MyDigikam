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
//#include "facedbaccess.h"
//#include "facedb.h"
#include "dnnfacemodel.h"
#include "digikam_debug.h"
#include "faceextractor.h"
#include "kd_tree.h"
//#include "dnndbscan.h"

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
          tree(128)
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
        knn->save(knnFile.toStdString());
        svm->save(svmFile.toStdString());

        delete extractor;
    }

public:

    int trainSVM() const;
    //void onlineTrainSVM(const std::vector<float>& inputSample, int inputLabel) const;
    int trainKNN() const;
    void onlineTrainKNN(const std::vector<float>& inputSample, int inputLabel) const;

    Identity predictSVM(cv::Mat faceEmbedding) const;
    Identity predictKNN(cv::Mat faceEmbedding) const;

    void addIndentityToTree(const Identity& id);
    Identity predictKDTree(const std::vector<float>& faceEmbedding, int k) const;

    Identity predictCosine(const std::vector<float>& faceEmbedding, double threshold) const;
    Identity predictL2(const std::vector<float>& faceEmbedding, double threshold) const;

public:

    bool debugMode;
    int identityCounter;

    FaceExtractor* extractor;
    cv::Ptr<cv::ml::SVM> svm;
    cv::Ptr<cv::ml::KNearest> knn;

    QHash<QString, QVector<Identity> > faceLibrary;
    QVector<QString> labels;

    // TODO put file names to Settings
    const QString knnFile = QLatin1String("knn.bin");
    const QString svmFile = QLatin1String("svm.bin");

    KDTree tree;
};

int FaceRecognizer::Private::trainSVM() const
{
    cv::Mat features, label;
    int size = 0;

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < labels.size(); ++i)
    {
        for (QVector<Identity>::const_iterator iter  = faceLibrary[labels[i]].cbegin();
                                               iter != faceLibrary[labels[i]].cend();
                                             ++iter)
        {
            QJsonArray jsonFaceEmbedding = QJsonDocument::fromJson(iter->attribute(QLatin1String("faceEmbedding")).toLatin1()).array();
            std::vector<float> recordedFaceEmbedding = FaceExtractor::decodeVector(jsonFaceEmbedding);

            label.push_back(i);
            features.push_back(FaceExtractor::vectortomat(recordedFaceEmbedding));

            ++size;
        }
    }

    svm->train(features, 0, label);

    qDebug() << "Support vector machine trains" << size << "samples in" << timer.elapsed() << "ms";

    return size;
}

int FaceRecognizer::Private::trainKNN() const
{
    cv::Mat features, label;
    int size = 0;

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < labels.size(); ++i)
    {
        for (QVector<Identity>::const_iterator iter  = faceLibrary[labels[i]].cbegin();
                                               iter != faceLibrary[labels[i]].cend();
                                             ++iter)
        {
            QJsonArray jsonFaceEmbedding = QJsonDocument::fromJson(iter->attribute(QLatin1String("faceEmbedding")).toLatin1()).array();
            std::vector<float> recordedFaceEmbedding = FaceExtractor::decodeVector(jsonFaceEmbedding);

            label.push_back(i);
            features.push_back(FaceExtractor::vectortomat(recordedFaceEmbedding));

            ++size;
        }
    }

    knn->train(features, 0, label);

    qDebug() << "KNN trains" << size << "samples in" << timer.elapsed() << "ms";

    return size;
}

void FaceRecognizer::Private::onlineTrainKNN(const std::vector<float>& inputSample, int inputLabel) const
{
    cv::Mat feature, label;
    label.push_back(inputLabel);
    feature.push_back(inputSample);

    cv::Ptr<cv::ml::TrainData> trainingSample = cv::ml::TrainData::create(feature, 0, label);

    knn->train(trainingSample, cv::ml::StatModel::UPDATE_MODEL);
}


Identity FaceRecognizer::Private::predictSVM(cv::Mat faceEmbedding) const
{
    if (!svm->isTrained())
    {
        trainSVM();
    }

    float id = svm->predict(faceEmbedding);

    return faceLibrary[labels[int(id)]][0];
}

Identity FaceRecognizer::Private::predictKNN(cv::Mat faceEmbedding) const
{
    if (!knn->isTrained())
    {
        trainKNN();
    }

    cv::Mat output;
    knn->findNearest(faceEmbedding, 3, output);

    float id = output.at<float>(0);

    return faceLibrary[labels[int(id)]][0];
}

void FaceRecognizer::Private::addIndentityToTree(const Identity& id)
{
    QJsonArray jsonFaceEmbedding = QJsonDocument::fromJson(id.attribute(QLatin1String("faceEmbedding")).toLatin1()).array();
    std::vector<float> recordedFaceEmbedding = FaceExtractor::decodeVector(jsonFaceEmbedding);

    tree.add(recordedFaceEmbedding, id);
}

Identity FaceRecognizer::Private::predictKDTree(const std::vector<float>& faceEmbedding, int k) const
{
    // Look for K-nearest neighbor which have the sqr distance greater smaller than 1
    QMap<double, QVector<KDNode*> > closestNeighbors = tree.getClosestNeighbors(faceEmbedding, 1.0, k);

    QMap<QString, QVector<double> > votingGroups;

    for (QMap<double, QVector<KDNode*> >::const_iterator iter  = closestNeighbors.cbegin();
                                                         iter != closestNeighbors.cend();
                                                       ++iter)
    {
        for (QVector<KDNode*>::const_iterator node  = iter.value().cbegin();
                                              node != iter.value().cend();
                                            ++node)
        {
            QString label = (*node)->getIdentity().attribute(QLatin1String("fullName"));

            votingGroups[label].append(iter.key());
        }
    }

    double maxScore = 0;
    QString prediction;

    for (QMap<QString, QVector<double> >::const_iterator group  = votingGroups.cbegin();
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

    return faceLibrary[prediction][0];
}

Identity FaceRecognizer::Private::predictCosine(const std::vector<float>& faceEmbedding, double threshold) const
{
    double bestDistance = -1;
    QVector<Identity>::const_iterator prediction;

    for (QHash<QString, QVector<Identity> >::const_iterator group  = faceLibrary.cbegin();
                                                            group != faceLibrary.cend();
                                                          ++group)
    {
        for (QVector<Identity>::const_iterator iter  = group.value().cbegin();
                                               iter != group.value().cend();
                                             ++iter)
        {
            QJsonArray jsonFaceEmbedding = QJsonDocument::fromJson(iter->attribute(QLatin1String("faceEmbedding")).toLatin1()).array();
            std::vector<float> recordedFaceEmbedding = FaceExtractor::decodeVector(jsonFaceEmbedding);

            double distance = FaceExtractor::cosineDistance(recordedFaceEmbedding, faceEmbedding);

            if (distance > bestDistance)
            {
                bestDistance  = distance;
                prediction    = iter;
            }
        }
    }

    if (bestDistance > threshold)
    {
        return *prediction;
    }

    return Identity();
}

Identity FaceRecognizer::Private::predictL2(const std::vector<float>& faceEmbedding, double threshold) const
{
    double bestDistance = 1;
    QVector<Identity>::const_iterator prediction;

    for (QHash<QString, QVector<Identity> >::const_iterator group  = faceLibrary.cbegin();
                                                            group != faceLibrary.cend();
                                                          ++group)
    {
        for (QVector<Identity>::const_iterator iter  = group.value().cbegin();
                                               iter != group.value().cend();
                                             ++iter)
        {
            QJsonArray jsonFaceEmbedding = QJsonDocument::fromJson(iter->attribute(QLatin1String("faceEmbedding")).toLatin1()).array();
            std::vector<float> recordedFaceEmbedding = FaceExtractor::decodeVector(jsonFaceEmbedding);

            double distance = FaceExtractor::L2squareDistance(recordedFaceEmbedding, faceEmbedding);

            if (distance < bestDistance)
            {
                bestDistance = distance;
                prediction   = iter;
            }
        }
    }

    if (bestDistance < threshold)
    {
        return *prediction;
    }

    return Identity();
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
    // TODO: scan database for face

    // Use ML to predict label
    if (metric == SupportVectorMachine)
    {
        return d->predictSVM(d->extractor->getFaceDescriptor(preprocessedImage));
    }
    else if (metric == KNN)
    {
        return d->predictKNN(d->extractor->getFaceDescriptor(preprocessedImage));
    }

    std::vector<float> faceEmbedding = d->extractor->getFaceEmbedding(preprocessedImage);
    Identity id;

    switch (metric)
    {
        case Tree:
            id = d->predictKDTree(faceEmbedding, (int)threshold);
            break;
        case CosDistance:
            id = d->predictCosine(faceEmbedding, threshold);
            break;
        case L2Distance:
            id = d->predictL2(faceEmbedding, threshold);
            break;
        default:
            break;
    }

    if (id.isNull())
    {
        // new identity
        QJsonArray jsonFaceEmbedding = FaceExtractor::encodeVector(faceEmbedding);
        id.setAttribute(QLatin1String("faceEmbedding"),
                        QString::fromLatin1(QJsonDocument(jsonFaceEmbedding).toJson(QJsonDocument::Compact)));
    }

    return id;
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

void FaceRecognizer::saveIdentity(Identity& id)
{
    QString label = id.attribute(QLatin1String("fullName"));

    if (label.isEmpty())
    {
        qWarning() << "idenitity is empty";

        return;
    }

    // TODO save identity to database
    if (id.isNull())
    {
        id.setId(++d->identityCounter);
    }

    d->faceLibrary[label].append(id);

    if (! d->labels.contains(label))
    {
        d->labels.append(label);
    }

    // Create a KD-Node for this identity
    d->addIndentityToTree(id);
}

}
