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
#include "kd_tree.h"

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

        mlp = cv::ml::ANN_MLP::create();
        // use 16 bit to index label
        int layer_sz[] = { 128, 200, 250, 150, 16 };
        int nlayers = 5;
        cv::Mat layer_sizes(1, nlayers, CV_32S, layer_sz);

        int method = cv::ml::ANN_MLP::BACKPROP;
        double method_param = 0.001;
        int max_iter = 10000;

        mlp->setLayerSizes(layer_sizes);
        mlp->setActivationFunction(cv::ml::ANN_MLP::SIGMOID_SYM, 0, 0);
        mlp->setTermCriteria(cv::TermCriteria(cv::TermCriteria::EPS, max_iter, 0));
        mlp->setTrainMethod(method, method_param);

        logisticRegression = cv::ml::LogisticRegression::create();
        logisticRegression->setLearningRate(0.001);
        logisticRegression->setTermCriteria(cv::TermCriteria(cv::TermCriteria::EPS, max_iter, 0));
        logisticRegression->setRegularization(cv::ml::LogisticRegression::REG_DISABLE);
        logisticRegression->setTrainMethod(cv::ml::LogisticRegression::BATCH);
        logisticRegression->setMiniBatchSize(1);
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

    int trainSVM() const;
    int trainKNN() const;
    int trainMLP() const;
    int trainLogisticRegression() const;

    void onlineTrainSVM(const std::vector<float>& inputSample, int inputLabel) const;
    void onlineTrainKNN(const std::vector<float>& inputSample, int inputLabel) const;

    Identity predictSVM(cv::Mat faceEmbedding) const;
    Identity predictKNN(cv::Mat faceEmbedding) const;
    Identity predictMLP(cv::Mat faceEmbedding, float threshold) const;
    Identity predictLogisticRegression(cv::Mat faceEmbedding) const;

    void addIndentityToTree(const Identity& id);
    Identity predictKDTree(const cv::Mat& faceEmbedding, int k) const;

    Identity predictCosine(const std::vector<float>& faceEmbedding, double threshold) const;
    Identity predictL2(const std::vector<float>& faceEmbedding, double threshold) const;

    Identity predictDb(const cv::Mat& faceEmbedding, int k) const;
public:

    bool debugMode;
    int identityCounter;

    FaceExtractor* extractor;
    cv::Ptr<cv::ml::SVM> svm;
    cv::Ptr<cv::ml::KNearest> knn;
    cv::Ptr<cv::ml::ANN_MLP> mlp;
    cv::Ptr<cv::ml::LogisticRegression> logisticRegression;

    QHash<QString, QVector<Identity> > faceLibrary;
    QVector<QString> labels;

    // TODO put file names to Settings
    const QString knnFile = QLatin1String("knn.bin");
    const QString svmFile = QLatin1String("svm.bin");

    KDTree tree;
    FaceDatabase db;
    SpatialDatabase treedb;
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

void FaceRecognizer::Private::onlineTrainSVM(const std::vector<float>& inputSample, int inputLabel) const
{
    cv::Mat feature, label;
    label.push_back(inputLabel);
    feature.push_back(FaceExtractor::vectortomat(inputSample));

    cv::Ptr<cv::ml::TrainData> trainingSample = cv::ml::TrainData::create(feature, 0, label);

    qDebug() << "online train svn";

    if (svm->train(trainingSample, cv::ml::StatModel::UPDATE_MODEL))
    {
        qDebug() << "online train svm";
    }
    else
    {
        qDebug() << "fail online train svm";
    }
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


int FaceRecognizer::Private::trainMLP() const
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

            cv::Mat train_response = cv::Mat::zeros(1, 16, CV_32F);

            int index = i;
            int j = 0;

            while (index > 1)
            {
                int newIndex = index / 2;

                train_response.at<float>(0, j) = index - newIndex*2;

                index = newIndex;
                ++j;
            }

            train_response.at<float>(0, j) = index;

            label.push_back(train_response);
            features.push_back(FaceExtractor::vectortomat(recordedFaceEmbedding));

            ++size;
        }
    }

    mlp->train(features, 0, label);

    qDebug() << "MLP trains" << size << "samples in" << timer.elapsed() << "ms";

    return size;
}

int FaceRecognizer::Private::trainLogisticRegression() const
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

            label.push_back(float(i));
            features.push_back(FaceExtractor::vectortomat(recordedFaceEmbedding));

            ++size;
        }
    }

    logisticRegression->train(features, 0, label);

    qDebug() << "Logistic regression trains" << size << "samples in" << timer.elapsed() << "ms";

    return size;
}


void FaceRecognizer::Private::onlineTrainKNN(const std::vector<float>& inputSample, int inputLabel) const
{
    cv::Mat feature, label;
    label.push_back(inputLabel);
    feature.push_back(FaceExtractor::vectortomat(inputSample));

    cv::Ptr<cv::ml::TrainData> trainingSample = cv::ml::TrainData::create(feature, 0, label);

    if (knn->train(trainingSample, cv::ml::StatModel::UPDATE_MODEL))
    {
        qDebug() << "online train knn";
    }
    else
    {
        qDebug() << "fail online train knn";
    }
}


Identity FaceRecognizer::Private::predictSVM(cv::Mat faceEmbedding) const
{
/*
    if (!svm->isTrained())
    {
        qDebug() << "svm is not trained";
        return Identity();
        //trainSVM();
    }
*/
    int     id    = int(svm->predict(faceEmbedding));
    QString label = db.queryLabel(id);

    Identity identity;
    identity.setId(id);
    identity.setAttribute(QLatin1String("fullName"), label);

    return identity;
}

Identity FaceRecognizer::Private::predictKNN(cv::Mat faceEmbedding) const
{
/*
    if (!knn->isTrained())
    {
        qDebug() << "knn is not trained";
        return Identity();
        //trainKNN();
    }
*/
    cv::Mat output;
    knn->findNearest(faceEmbedding, 3, output);

    int     id    = int(output.at<float>(0));
    QString label = db.queryLabel(id);

    Identity identity;
    identity.setId(id);
    identity.setAttribute(QLatin1String("fullName"), label);

    return identity;
}

Identity FaceRecognizer::Private::predictMLP(cv::Mat faceEmbedding, float threshold) const
{
    if (!mlp->isTrained())
    {
        qDebug() << "train mlp";
        trainMLP();
    }

    cv::Mat output;

    mlp->predict(faceEmbedding, output);

    int id = 0;

    for (int i = 0; i < 16; ++i)
    {
        if (output.at<float>(0, i) >= threshold)
        {
            id += int(pow(2, i));
        }
    }

    Identity identity;

    if (id < labels.size())
    {
        QString label = labels[id];

        identity.setId(id);
        identity.setAttribute(QLatin1String("fullName"), label);
    }

    return identity;
}

Identity FaceRecognizer::Private::predictLogisticRegression(cv::Mat faceEmbedding) const
{
    if (!logisticRegression->isTrained())
    {
        qDebug() << "train logistic regression";
        trainLogisticRegression();
    }

    cv::Mat output;
    int id = logisticRegression->predict(faceEmbedding);

    //int     id    = int(output.at<float>(0));
    QString label = labels[id];

    Identity identity;
    identity.setId(id);
    identity.setAttribute(QLatin1String("fullName"), label);

    return identity;
}

void FaceRecognizer::Private::addIndentityToTree(const Identity& id)
{
    QJsonArray jsonFaceEmbedding = QJsonDocument::fromJson(id.attribute(QLatin1String("faceEmbedding")).toLatin1()).array();
    cv::Mat recordedFaceEmbedding = FaceExtractor::vectortomat(FaceExtractor::decodeVector(jsonFaceEmbedding));

    tree.add(recordedFaceEmbedding, id);
}

Identity FaceRecognizer::Private::predictKDTree(const cv::Mat& faceEmbedding, int k) const
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

Identity FaceRecognizer::Private::predictDb(const cv::Mat& faceEmbedding, int k) const
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

    QString label = db.queryLabel(prediction);

    Identity identity;
    identity.setId(prediction);
    identity.setAttribute(QLatin1String("fullName"), label);

    return identity;
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
    Identity id;

    // Use ML to predict label
    if (metric == SupportVectorMachine)
    {
        id = d->predictSVM(d->extractor->getFaceDescriptor(preprocessedImage));
    }
    else if (metric == KNN)
    {
        id = d->predictKNN(d->extractor->getFaceDescriptor(preprocessedImage));
    }
    else if (metric == MLP)
    {
        id = d->predictMLP(d->extractor->getFaceDescriptor(preprocessedImage), float(threshold));
    }
    else if (metric == LogisticRegression)
    {
        id = d->predictLogisticRegression(d->extractor->getFaceDescriptor(preprocessedImage));
    }
    else if (metric == Tree)
    {
        id = d->predictKDTree(d->extractor->getFaceDescriptor(preprocessedImage), (int)threshold);
    }
    else if (metric == DB)
    {
        id = d->predictDb(d->extractor->getFaceDescriptor(preprocessedImage), (int)threshold);
    }
    else
    {
        std::vector<float> faceEmbedding = d->extractor->getFaceEmbedding(preprocessedImage);

        switch (metric)
        {
            case CosDistance:
                id = d->predictCosine(faceEmbedding, threshold);
                break;
            case L2Distance:
                id = d->predictL2(faceEmbedding, threshold);
                break;
            default:
                break;
        }
    }

    std::vector<float> faceEmbedding = d->extractor->getFaceEmbedding(preprocessedImage);

    QJsonArray jsonFaceEmbedding = FaceExtractor::encodeVector(faceEmbedding);
    id.setAttribute(QLatin1String("faceEmbedding"),
                    QString::fromLatin1(QJsonDocument(jsonFaceEmbedding).toJson(QJsonDocument::Compact)));

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

int FaceRecognizer::saveIdentity(Identity& id, bool newLabel)
{
/*
    QString label = id.attribute(QLatin1String("fullName"));

    if (label.isEmpty())
    {
        qWarning() << "idenitity is empty";

        return -1;
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

    d->addIndentityToTree(id);

    return d->labels.indexOf(label);
*/
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

    if (! d->treedb.insert(recordedFaceEmbedding, index))
    {
        qWarning() << "Error insert face embedding";
        return -1;
    }

    // Create a KD-Node for this identity
    //d->addIndentityToTree(id);
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
