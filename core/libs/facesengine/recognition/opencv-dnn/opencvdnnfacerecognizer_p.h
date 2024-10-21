/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2020-05-22
 * Description : Wrapper of face recognition using OpenFace
 *
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2020-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2020      by Nghia Duong <minhnghiaduong997 at gmail dot com>
 * SPDX-FileCopyrightText: 2024      by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

#include "opencvdnnfacerecognizer.h"

// C++ includes

#include <iostream>

// Qt includes

#include <QElapsedTimer>

// Local includes

#include "digikam_debug.h"
#include "dnnopenfaceextractor.h"
#include "dnnsfaceextractor.h"
#include "facedbaccess.h"
#include "facedb.h"
#include "kd_treebase.h"

namespace Digikam
{

class Q_DECL_HIDDEN OpenCVDNNFaceRecognizer::Private
{
public:

    Private(Classifier mthd, FaceScanSettings::FaceRecognitionModel recModel)
        : method        (mthd),
          recognizeModel(recModel)
    {
        ref = 1;

        for (int i = 0 ; i < 1 ; ++i)
        {
            switch (recognizeModel)
            {
                case FaceScanSettings::FaceRecognitionModel::OpenFace:
                {
                    extractors << new DNNOpenFaceExtractor;
                    break;
                }

                case FaceScanSettings::FaceRecognitionModel::SFace:
                {
                    extractors << new DNNSFaceExtractor;
                    break;
                }

                default:
                {
                    qCritical(DIGIKAM_DPLUGIN_GENERIC_LOG) << "OpenCVDNNFaceRecognizer::Private() Unknown recognition model specified" << Qt::endl;
                    break;
                }
            }
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
                if (tree)
                {
                    delete tree;
                    tree = nullptr; // safety in case reconstructTree fails
                }

                tree = FaceDbAccess().db()->reconstructTree(recognizeModel);
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
        QVector<DNNFaceExtractorBase*>::iterator extractor = extractors.begin();

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
    int predictDb(const cv::Mat& faceEmbedding)     const;
    int predictSFace(const cv::Mat& faceEmbedding)  const;

    bool insertData(const cv::Mat& position, const int label, const QString& context = QString());

public:

    int                             ref                   = 1;

    Classifier                      method;

    QVector<DNNFaceExtractorBase*>  extractors;
    cv::Ptr<cv::ml::SVM>            svm;
    cv::Ptr<cv::ml::KNearest>       knn;

    KDTreeBase*                     tree                  = nullptr;
    int                             kNeighbors            = 5;
/*
    float                           threshold             = 0.4F;
*/
    int                             uiThreshold           = DNN_MODEL_THRESHOLD_NOT_SET;

    bool                            newDataAdded          = true;

    FaceScanSettings::FaceRecognitionModel recognizeModel = FaceScanSettings::FaceRecognitionModel::SFace;

public:

    class ParallelRecognizer;
    class ParallelTrainer;
};

class OpenCVDNNFaceRecognizer::Private::ParallelRecognizer : public cv::ParallelLoopBody
{
public:

    ParallelRecognizer(OpenCVDNNFaceRecognizer::Private* const d,
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
                    break;
                }
            }

            ids[i] = id;
        }
    }

private:

    const QList<QImage*>&                   images;
    QVector<int>&                           ids;

    OpenCVDNNFaceRecognizer::Private* const d = nullptr;

private:

    Q_DISABLE_COPY(ParallelRecognizer)
};

class OpenCVDNNFaceRecognizer::Private::ParallelTrainer: public cv::ParallelLoopBody
{
public:

    ParallelTrainer(OpenCVDNNFaceRecognizer::Private* const d,
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

    OpenCVDNNFaceRecognizer::Private* const d = nullptr;

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

    double threshold;

    if (FaceScanSettings::FaceRecognitionModel::SFace == recognizeModel)
    {
        threshold = DNNModelManager::instance()->getModel(QLatin1String("SFace"), DNNModelUsage::DNNUsageFaceRecognition)->getThreshold(uiThreshold);
    }
    else
    {
        threshold = DNNModelManager::instance()->getModel(QLatin1String("OpenFace"), DNNModelUsage::DNNUsageFaceRecognition)->getThreshold(uiThreshold);
    }

    // Look for K-nearest neighbor which have the cosine distance greater than the threshold.

    QMap<double, QVector<int> > closestNeighbors = tree->getClosestNeighbors(faceEmbedding, threshold, kNeighbors);

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
    double threshold;

    if (FaceScanSettings::FaceRecognitionModel::SFace == recognizeModel)
    {
        threshold = DNNModelManager::instance()->getModel(QLatin1String("SFace"),
                                                          DNNModelUsage::DNNUsageFaceRecognition)->getThreshold(uiThreshold);
    }
    else
    {
        threshold = DNNModelManager::instance()->getModel(QLatin1String("OpenFace"),
                                                          DNNModelUsage::DNNUsageFaceRecognition)->getThreshold(uiThreshold);
    }

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
    if (nodePos.rows != 1)
    {
        qCWarning(DIGIKAM_FACEDB_LOG) << "Error face embedding not valid";

        return false;
    }

    int nodeId = FaceDbAccess().db()->insertFaceVector(nodePos, label, context);

    if (nodeId <= 0)
    {
        qCWarning(DIGIKAM_FACEDB_LOG) << "Error inserting face embedding to database";
    }

    if      (method == DB)
    {
        if (!FaceDbAccess().db()->insertToTreeDb(nodeId, nodePos))
        {
            qCWarning(DIGIKAM_FACEDB_LOG) << "Error insert face embedding";

            return false;
        }
    }
    else if (method == Tree)
    {
        KDNodeBase* const newNode = tree->add(nodePos, label);

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
