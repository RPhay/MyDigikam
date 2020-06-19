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
          svm(cv::ml::SVM::create()),
          knn(cv::ml::KNearest::create()),
          tree(128)
    {
        // use linear mapping
        svm->setKernel(cv::ml::SVM::LINEAR);

        knn->setAlgorithmType(cv::ml::KNearest::BRUTE_FORCE);
        knn->setIsClassifier(true);
    }

    ~Private()
    {
        delete extractor;
    }

public:

    int trainSVM()
    {
        cv::Mat features, label;

        int size = 0;

        QElapsedTimer timer;
        timer.start();

        for (int i = 0; i < labels.size(); ++i)
        {
            for (QVector<Identity>::iterator iter  = faceLibrary[labels[i]].begin();
                                             iter != faceLibrary[labels[i]].end();
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

    int trainKNN()
    {
        cv::Mat features, label;

        int size = 0;

        QElapsedTimer timer;
        timer.start();

        for (int i = 0; i < labels.size(); ++i)
        {
            for (QVector<Identity>::iterator iter  = faceLibrary[labels[i]].begin();
                                             iter != faceLibrary[labels[i]].end();
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

    Identity predictSVM(cv::Mat faceEmbedding)
    {
        if (!svm->isTrained())
        {
            trainSVM();
        }

        // perdict
        float id = svm->predict(faceEmbedding);

        return faceLibrary[labels[int(id)]][0];
    }

    Identity predictKNN(cv::Mat faceEmbedding)
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

    void addIndentityToTree(const Identity& id)
    {
        QJsonArray jsonFaceEmbedding = QJsonDocument::fromJson(id.attribute(QLatin1String("faceEmbedding")).toLatin1()).array();
        std::vector<float> recordedFaceEmbedding = FaceExtractor::decodeVector(jsonFaceEmbedding);

        tree.add(recordedFaceEmbedding, id);
    }

    Identity predictKDTree(const std::vector<float>& faceEmbedding, int k)
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

public:

    bool debugMode;
    int identityCounter;

    FaceExtractor* extractor;
    cv::Ptr<cv::ml::SVM> svm;
    cv::Ptr<cv::ml::KNearest> knn;

    QHash<QString, QVector<Identity> > faceLibrary;
    QVector<QString> labels;

    KDTree tree;
};

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
    // TODO: verify why have to enum this
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
    // Use support vector machine to predict label
    if (metric == SupportVectorMachine)
    {
        return d->predictSVM(d->extractor->getFaceDescriptor(preprocessedImage));
    }
    else if (metric == KNN)
    {
        return d->predictKNN(d->extractor->getFaceDescriptor(preprocessedImage));
    }

    std::vector<float> faceEmbedding = d->extractor->getFaceEmbedding(preprocessedImage);

    if (metric == Tree)
    {
        return d->predictKDTree(faceEmbedding, (int)threshold);
    }

    // TODO: scan database for face

    double bestDistance;

    switch (metric)
    {
    case CosDistance:
        bestDistance = -1;
        break;
    case L2Distance:
        bestDistance = 1;
        break;
    case L2NormDistance:
        bestDistance = 1;
        break;
    default:
        bestDistance = 1;
        break;
    }

    QVector<Identity>::iterator prediction;

    for (QHash<QString, QVector<Identity> >::iterator group  = d->faceLibrary.begin();
                                                      group != d->faceLibrary.end();
                                                    ++group)
    {
        for (QVector<Identity>::iterator iter  = group.value().begin();
                                         iter != group.value().end();
                                       ++iter)
        {
            QJsonArray jsonFaceEmbedding = QJsonDocument::fromJson(iter->attribute(QLatin1String("faceEmbedding")).toLatin1()).array();
            std::vector<float> recordedFaceEmbedding = FaceExtractor::decodeVector(jsonFaceEmbedding);

            double distance;

            switch (metric)
            {
            case CosDistance:
                distance = FaceExtractor::cosineDistance(recordedFaceEmbedding, faceEmbedding);

                if (distance > bestDistance)
                {
                    bestDistance  = distance;
                    prediction    = iter;
                }

                break;
            case L2Distance:
                distance = FaceExtractor::L2squareDistance(recordedFaceEmbedding, faceEmbedding);

                if (distance < bestDistance)
                {
                    bestDistance = distance;
                    prediction   = iter;
                }

                break;
            case L2NormDistance:
                distance = FaceExtractor::L2squareNormDistance(recordedFaceEmbedding, faceEmbedding);

                if (distance < bestDistance)
                {
                    bestDistance = distance;
                    prediction   = iter;
                }

                break;
            default:
                break;
            }
        }
    }

    switch (metric)
    {
    case CosDistance:
        if (bestDistance > threshold)
        {
            return *prediction;
        }
        break;
    case L2Distance:
        if (bestDistance < threshold)
        {
            return *prediction;
        }
        break;
    case L2NormDistance:
        if (bestDistance < threshold)
        {
            return *prediction;
        }
        break;
    default:
        break;
    }

    // new identity
    Identity id;
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

void FaceRecognizer::saveIdentity(Identity& id)
{
    QString label = id.attribute(QLatin1String("fullName"));
    // TODO save identity to database
    if (label.isEmpty())
    {
        qWarning() << "idenitity is empty";

        return;
    }

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
