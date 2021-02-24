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

#ifndef OPENCV_DNN_FACERECOGNIZER_P_H
#define OPENCV_DNN_FACERECOGNIZER_P_H

#include "opencvdnnfacerecognizer.h"

// C++ includes

#include <iostream>

// Qt includes

#include <QElapsedTimer>

// Local includes

#include "digikam_debug.h"
#include "dnnfaceextractor.h"
#include "facedbaccess.h"
#include "facedb.h"
#include "kd_tree.h"

namespace Digikam
{

class Q_DECL_HIDDEN OpenCVDNNFaceRecognizer::Private
{
public:

    Private(Classifier method)
        : method        (method),
          tree          (nullptr),
          kNeighbors    (5),
          threshold     (0.4),
          newDataAdded  (true)
    {
        for (int i = 0 ; i < 1 ; ++i)
        {
            extractors << new DNNFaceExtractor;
        }

        switch (method)
        {
            case SVM:
            {
                svm = cv::ml::SVM::create();
                svm->setKernel(cv::ml::SVM::LINEAR);
                break;
            }

            case OpenCV_KNN:
            {
                knn = cv::ml::KNearest::create();
                knn->setAlgorithmType(cv::ml::KNearest::BRUTE_FORCE);
                knn->setIsClassifier(true);
                break;
            }

            case Tree:
            {
                tree = FaceDbAccess().db()->reconstructTree();
                break;
            }

            case DB:
            {
                break;
            }

            default:
            {
                qFatal("Invalid classifier");
            }
        }
    }

    ~Private()
    {
        QVector<DNNFaceExtractor*>::iterator extractor = extractors.begin();

        while (extractor != extractors.end())
        {
            delete *extractor;
            extractor = extractors.erase(extractor);
        }

        delete tree;
    }

public:

    bool trainSVM();
    bool trainKNN();

    int predictSVM(const cv::Mat& faceEmbedding);
    int predictKNN(const cv::Mat& faceEmbedding);

    int predictKDTree(const cv::Mat& faceEmbedding) const;
    int predictDb(const cv::Mat& faceEmbedding) const;

    bool insertData(const cv::Mat& position, const int label, const QString& context = QString());

public:

    Classifier                 method;

    QVector<DNNFaceExtractor*> extractors;
    cv::Ptr<cv::ml::SVM>       svm;
    cv::Ptr<cv::ml::KNearest>  knn;

    KDTree*                    tree;
    int                        kNeighbors;
    float                      threshold;

    bool                       newDataAdded;

public:

    class ParallelRecognizer;
    class ParallelTrainer;
};

class OpenCVDNNFaceRecognizer::Private::ParallelRecognizer : public cv::ParallelLoopBody
{
public:

    ParallelRecognizer(OpenCVDNNFaceRecognizer::Private* d,
                       const QList<QImage*>& images,
                       QVector<int>& ids)
        : images    (images),
          ids       (ids),
          d         (d)
    {
        ids.resize(images.size());
    }

    void operator()(const cv::Range& range) const override
    {
        for(int i = range.start ; i < range.end ; ++i)
        {
            int id = -1;

            cv::Mat faceEmbedding = d->extractors[i%(d->extractors.size())]->getFaceEmbedding(OpenCVDNNFaceRecognizer::prepareForRecognition(*images[i]));

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

            ids[i] = id;
        }
    }

private:

    const QList<QImage*>&                   images;
    QVector<int>&                           ids;

    OpenCVDNNFaceRecognizer::Private* const d;

private:

    Q_DISABLE_COPY(ParallelRecognizer)
};

class OpenCVDNNFaceRecognizer::Private::ParallelTrainer: public cv::ParallelLoopBody
{
public:

    ParallelTrainer(OpenCVDNNFaceRecognizer::Private* d,
                    const QList<QImage*>& images,
                    const int& id,
                    const QString& context)
        : images    (images),
          id        (id),
          context   (context),
          d         (d)
    {
    }

    void operator()(const cv::Range& range) const override
    {
        for(int i = range.start ; i < range.end ; ++i)
        {
            cv::Mat faceEmbedding = d->extractors[i%(d->extractors.size())]->
                getFaceEmbedding(OpenCVDNNFaceRecognizer::prepareForRecognition(*images[i]));

            if (!d->insertData(faceEmbedding, id, context))
            {
                qCWarning(DIGIKAM_FACEDB_LOG) << "Fail to register a face of identity" << id;
            }
        }
    }

private:

    const QList<QImage*>&                   images;
    const int&                              id;
    const QString&                          context;

    OpenCVDNNFaceRecognizer::Private* const d;

private:

    Q_DISABLE_COPY(ParallelTrainer)
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

int OpenCVDNNFaceRecognizer::Private::predictSVM(const cv::Mat& faceEmbedding)
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

int OpenCVDNNFaceRecognizer::Private::predictKNN(const cv::Mat& faceEmbedding)
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
    knn->findNearest(faceEmbedding, kNeighbors, output);

    return (int(output.at<float>(0)));
}

int OpenCVDNNFaceRecognizer::Private::predictKDTree(const cv::Mat& faceEmbedding) const
{
    if (!tree)
    {
        return -1;
    }

    // Look for K-nearest neighbor which have the cosine distance greater than the threshold.

    QMap<double, QVector<int> > closestNeighbors = tree->getClosestNeighbors(faceEmbedding, threshold, 0.8, kNeighbors);

    QMap<int, QVector<double> > votingGroups;

    for (QMap<double, QVector<int> >::const_iterator iter  = closestNeighbors.cbegin();
                                                     iter != closestNeighbors.cend();
                                                     ++iter)
    {
        for (QVector<int>::const_iterator node  = iter.value().cbegin();
                                          node != iter.value().cend();
                                          ++node)
        {
            int label = (*node);

            votingGroups[label].append(iter.key());
        }
    }

    double maxScore = 0.0;
    int prediction  = -1;

    for (QMap<int, QVector<double> >::const_iterator group  = votingGroups.cbegin();
                                                     group != votingGroups.cend();
                                                     ++group)
    {
        double score = 0.0;

        for (int i = 0 ; i < group.value().size() ; ++i)
        {
            score += (threshold - group.value()[i]);
        }

        if (score > maxScore)
        {
            maxScore   = score;
            prediction = group.key();
        }
    }

    return prediction;
}

int OpenCVDNNFaceRecognizer::Private::predictDb(const cv::Mat& faceEmbedding) const
{
    QMap<double, QVector<int> > closestNeighbors = FaceDbAccess().db()->getClosestNeighborsTreeDb(faceEmbedding, threshold, 0.8, kNeighbors);

    QMap<int, QVector<double> > votingGroups;

    for (QMap<double, QVector<int> >::const_iterator iter  = closestNeighbors.cbegin();
                                                     iter != closestNeighbors.cend();
                                                     ++iter)
    {
        for (int i = 0 ; i < iter.value().size() ; ++i)
        {
            votingGroups[iter.value()[i]].append(iter.key());
        }
    }

    double maxScore = 0.0;
    int prediction  = -1;

    for (QMap<int, QVector<double> >::const_iterator group  = votingGroups.cbegin();
                                                     group != votingGroups.cend();
                                                     ++group)
    {
        double score = 0.0;

        for (int i = 0 ; i < group.value().size() ; ++i)
        {
            score += (threshold - group.value()[i]);
        }

        if (score > maxScore)
        {
            maxScore   = score;
            prediction = group.key();
        }
    }

    return prediction;
}

bool OpenCVDNNFaceRecognizer::Private::insertData(const cv::Mat& nodePos, const int label, const QString& context)
{
    int nodeId = FaceDbAccess().db()->insertFaceVector(nodePos, label, context);

    if (nodeId <= 0)
    {
        qCWarning(DIGIKAM_FACEDB_LOG) << "error inserting face embedding to database";
    }

    if      (method == DB)
    {
        if (! FaceDbAccess().db()->insertToTreeDb(nodeId, nodePos))
        {
            qCWarning(DIGIKAM_FACEDB_LOG) << "Error insert face embedding";

            return false;
        }
    }
    else if (method == Tree)
    {
        KDNode* const newNode = tree->add(nodePos, label);

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

} // namespace Digikam

#endif // OPENCV_DNN_FACERECOGNIZER_P_H
