/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-11-10
 * Description : Face classifier
 *
 * SPDX-FileCopyrightText: 2024      by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024      by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "faceclassifier.h"

// Qt includes

#include <QException>
#include <QReadWriteLock>
#include <QtConcurrent>
#include <QFuture>
#include <QMap>
#include <QList>
#include <QTimer>
#include <QFile>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "identityprovider.h"
#include "dnnmodelmanager.h"

namespace Digikam
{

FaceClassifier::Private* FaceClassifier::d = nullptr;

class Q_DECL_HIDDEN FaceClassifier::Private
{
public:

    int                                     ref                     = 0;
    bool                                    ready                   = false;
    bool                                    trainingWaiting         = false;
    FaceScanSettings::FaceRecognitionModel  recognizeModel          = FaceScanSettings::FaceRecognitionModel::SFace;
    IdentityProvider*                       identityProvider        = nullptr;

    // Qt variables

    QReadWriteLock                          trainingLock;
    QFutureWatcher<bool>                    trainingFuture;
    QMutex                                  trainingMutex;
    QMap<int, QList<cv::Mat> >              identityFeatures;

    // openCV variables

    cv::Ptr<cv::ml::KNearest>               knnClassifier;
    cv::Ptr<cv::ml::SVM>                    svmClassifier;

    // classifier hyperparameters

    // KNN hyperparameters
    int                                     knn_defaultK            = 8;
    cv::ml::KNearest::Types                 knn_algorithm           = cv::ml::KNearest::Types::BRUTE_FORCE;

    // SVM hyperparameeters
    cv::ml::SVM::Types                      svm_type                = cv::ml::SVM::Types::C_SVC;
    cv::ml::SVM::KernelTypes                svm_kernel              = cv::ml::SVM::KernelTypes::RBF;
    double                                  svm_gamma               = 0.38;
    double                                  svm_C                   = 10;
};

class Q_DECL_HIDDEN FaceClassifierCreator
{
public:

    FaceClassifier object;
};

Q_GLOBAL_STATIC(FaceClassifierCreator, FaceClassifierCreator)

FaceClassifier::FaceClassifier()
    : FaceClassifierBase()
{
    if (!d)
    {
        d = new Private;
        d->identityProvider = IdentityProvider::instance();
        retrain();
    }
    else
    {
        ++(d->ref);
    }
}

FaceClassifier::~FaceClassifier()
{
    --(d->ref);

    if (d->ref == 0)
    {
        delete d;
        d = nullptr;
    }
}

FaceClassifier* FaceClassifier::instance()
{
    return &FaceClassifierCreator->object;
}

bool FaceClassifier::ready() const
{
    return d->ready;
}

void FaceClassifier::setParameters(const FaceScanSettings& parameters)
{
    d->recognizeModel = parameters.recognizeModel;
    threshold         = 0.5;

    try
    {
        if (d->recognizeModel == FaceScanSettings::FaceRecognitionModel::SFace)
        {
            DNNModelBase* const model = DNNModelManager::instance()->getModel(QLatin1String("sface"),
                                                                              DNNModelUsage::DNNUsageFaceRecognition);

            if (model)
            {
                threshold = model->getThreshold(parameters.recognizeAccuracy);
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    catch (...)
    {
        // do nothing
    }
}

cv::Ptr<cv::ml::KNearest> FaceClassifier::createKNearest()
{
    cv::Ptr<cv::ml::KNearest> cvClassifier = cv::ml::KNearest::create();
    cvClassifier->setIsClassifier(true);
    cvClassifier->setAlgorithmType(d->knn_algorithm);
    cvClassifier->setDefaultK(d->knn_defaultK);

    return cvClassifier;
}

cv::Ptr<cv::ml::SVM> FaceClassifier::createSVM()
{
    cv::Ptr<cv::ml::SVM> cvClassifier = cv::ml::SVM::create();
    cvClassifier->setType(d->svm_type);
    cvClassifier->setKernel(d->svm_kernel);
    cvClassifier->setGamma(d->svm_gamma);
    cvClassifier->setC(d->svm_C);
    cvClassifier->setTermCriteria(cvTermCriteria(cv::TermCriteria::Type::MAX_ITER + cv::TermCriteria::Type::EPS, 500, 1e-6));

    return cvClassifier;
}

bool FaceClassifier::retrain()
{
    QMutexLocker lock(&d->trainingMutex);

    d->trainingWaiting = true;

    if (d->trainingFuture.future().isRunning())
    {
        // training thread is already running

        return false;
    }

    d->trainingFuture.setFuture(QtConcurrent::run(QThreadPool::globalInstance(), 

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

        &FaceClassifier::loadTrainingData,
        this

#else

        this,
        &FaceClassifier::loadTrainingData

#endif

        ));
/*
    connect(&d->trainingFuture, &QFutureWatcher<bool>::finished,
            this, &FaceClassifier::slotTrainingComplete);
*/
    return true;
}

bool FaceClassifier::loadTrainingData()
{
    qCDebug(DIGIKAM_FACESENGINE_LOG) << "FaceClassifier::loadTrainingData: training started";

    QThread::currentThread()->setPriority(QThread::Priority::HighPriority);

    QElapsedTimer timer;
    timer.start();

    // loop to rebuild the classifier components
    // d->trainingWaiting can be modified outside the loop

    do
    {
        d->trainingWaiting = false;

        QMap<int, QList<cv::Mat> > identityFeatures;

        cv::Ptr<cv::ml::KNearest> knn = createKNearest();
        cv::Ptr<cv::ml::SVM> svm      = createSVM();

        try
        {
            cv::Ptr<cv::ml::TrainData> trainData = d->identityProvider->getTrainingData();

            if (nullptr != trainData)
            {
                cv::Mat samples = trainData->getSamples();
                cv::Mat labels  = trainData->getResponses();

/* TODO: Remove this.

                QFile file(QLatin1String("/Users/michmill/Downloads/trainingData.csv"));
                file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate | QIODevice::Unbuffered);
*/
                for (int i = 0 ; i < labels.rows ; ++i)
                {
                    int label = labels.at<int>(cv::Point(i,0));
                    identityFeatures[label].append(samples.row(i));

                    // file.write(QString::number(label).toUtf8());
                    // for (int j = 0; j < samples.cols; ++j)
                    // {
                    //     file.write(",");
                    //     file.write(QString::number(samples.at<float>(i,j)).toUtf8());
                    // }
                    // file.write("\n");
                }

                // file.close();
            }

            if (identityFeatures.count() > d->knn_defaultK)
            {
                knn->train(trainData);
                svm->train(trainData);
            }

        }
        catch (cv::Exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "FaceClassifier::loadTrainingData: exception: " << e.what();
        }
        catch (const QException& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "FaceClassifier::loadTrainingData: exception: " << e.what();
        }
        catch (const std::exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "FaceClassifier::loadTrainingData: exception: " << e.what();
        }
        catch (...)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "Default exception";
        }

        d->trainingLock.lockForWrite();

        d->knnClassifier    = knn;
        d->svmClassifier    = svm;
        d->identityFeatures = identityFeatures;

        d->ready            = true;

        d->trainingLock.unlock();
    }
    while (d->trainingWaiting);

    qCDebug(DIGIKAM_FACESENGINE_LOG) << "FaceClassifier::loadTrainingData: training completed in " << timer.elapsed() << "ms";

    return true;
}

void FaceClassifier::slotTrainingComplete()
{
    if (d->trainingWaiting)
    {
        d->trainingWaiting = false;
        retrain();
    }
}

int FaceClassifier::predict(const cv::Mat& target) const
{
    int label = -1;

    if (!d->ready)
    {
        return -1;
    }

    d->trainingLock.lockForRead();

    if (!d->knnClassifier->isTrained() || !d->svmClassifier->isTrained())
    {
        // we don't have enough identites and samples yet to use the knn and svm classifiers
        // so we perform a full brute-force search on all the known faces

        label = predictFullSearch(target);
    }
    else
    {
        // use the classifier algorithm

        label = predictClassifier(target);
    }

    d->trainingLock.unlock();

    return label;
}

int FaceClassifier::predict(const cv::UMat& target) const
{
    cv::Mat matTarget = target.getMat(cv::ACCESS_READ);

    return predict(matTarget);
}

int FaceClassifier::predictFullSearch(const cv::Mat& target)    const
{
    QElapsedTimer timer;
    timer.start();

    int label = listSearch(target, d->identityFeatures);
    
    qCDebug(DIGIKAM_FACESENGINE_LOG) << "FaceClassifier::predictFullSearch: classifier prediction is: "  << label << "    completed in " << timer.elapsed();

    return label;
}

int FaceClassifier::predictClassifier(const cv::Mat& target)    const
{
    int     label           = -1;
    int     badLabel1       = -1;
    int     badLabel2       = -1;
    cv::Mat knn_resultMat;
    cv::Mat knn_neighbors;
    cv::Mat knn_distances;
    cv::Mat svm_prediction;
    IdentityProvider* const idProvider = IdentityProvider::instance();

    QElapsedTimer timer;
    timer.start();

    // stage 1 - compare SVM and KNN classifier results

    d->svmClassifier->predict(target, svm_prediction);
    int svm_result = int(svm_prediction.at<float>(0,0));

    d->knnClassifier->findNearest(target,
                                  std::min<int>(d->identityFeatures.count(), d->knn_defaultK),
                                  //1,
                                  knn_resultMat,
                                  knn_neighbors,
                                  knn_distances);

    int knn_result = int(knn_resultMat.at<float>(0,0));

    // check if results exist in recognition DB

    if (!idProvider->isValidId(svm_result))
    {
        svm_result = -1;
    }
    if (!idProvider->isValidId(knn_result))
    {
        knn_result = -1;
    }

    // if the  SVM and KNN classifiers agree it's usually over 98% correct unless
    // we have massively unbalanced data so we need to validate the results

    if (svm_result == knn_result && svm_result != -1)
    {
        if (validateKNNSVMResult(target, svm_result))
        {
            qCDebug(DIGIKAM_FACESENGINE_LOG) << "FaceClassifier::predictClassifier: classifier stage 1 prediction is: "  << svm_result << "    completed in " << timer.elapsed();
            return svm_result;
        }
        else
        {
            badLabel1 = svm_result;
        }
    }

    // Stage 2 - check the absolute nearest neighbor for a match to the SVM result

    if ((-1 != svm_result) && (-1 != knn_result))
    {
        // find the absolute nearest neighbor

        float distance = 10000.0;

        for (int i = 0 ; i < knn_neighbors.cols ; ++i)
        {
            if (distance > abs(knn_distances.at<float>(0,i)))
            {
                label    = int(knn_neighbors.at<float>(0,i));
                distance = abs(knn_distances.at<float>(0,i));
            }
        }

        // verify we haven't checked this label before

        if ((label == svm_result) && (label != badLabel1))
        {
            // check the distance of of the target to the samples for the label

            if (validateKNNSVMResult(target, label))
            {
                qCDebug(DIGIKAM_FACESENGINE_LOG) << "FaceClassifier::predictClassifier: classifier stage 2 prediction is: "  << label << "    completed in " << timer.elapsed();
                return label;
            }
            else
            {
                badLabel2 = label;
            }
        }

    }

    // Stage 3 - do brute-force search on distinct set of SVM and KNN labels returned 
    // from the internal classifiers that weren't checked before

    {
        d->knnClassifier->findNearest(target,
                                      std::min<int>(d->identityFeatures.count(), d->knn_defaultK),
                                      knn_resultMat,
                                      knn_neighbors,
                                      knn_distances);

        QMap<int, QList<cv::Mat> > featureSet;

        if ((svm_result != badLabel1) && (svm_result != badLabel2) && idProvider->isValidId(svm_result))
        {
            featureSet[svm_result] = d->identityFeatures[svm_result];
        }

        for (int i = 0 ; i < knn_neighbors.cols ; ++i)
        {
            int tmpLabel = int(knn_neighbors.at<float>(0, i));

            if ((tmpLabel != badLabel1) && (tmpLabel != badLabel2) && idProvider->isValidId(tmpLabel))
            {
                featureSet[tmpLabel] = d->identityFeatures[tmpLabel];
            }
        }

        label = listSearch(target, featureSet);
    }

    qCDebug(DIGIKAM_FACESENGINE_LOG) << "FaceClassifier::predictClassifier: classifier stage 3 prediction is: "
                                     << label << " completed in " << timer.elapsed();

    return label;
}

bool FaceClassifier::validateKNNSVMResult(const cv::Mat& target, int label) const
{
    bool result = false;

    for (const cv::Mat& feature : std::as_const(d->identityFeatures[label]))
    {
        float distance = 10000.0;

        // TODO: add a feature compare for OpenFace if we decide to keep it

        if (featureSFaceCompare(target, feature, distance))
        {
            result = true;
            break;
        }
    }

    return result;
}

int FaceClassifier::listSearch(const cv::Mat& target, const QMap<int, QList<cv::Mat> >& identityFeatures) const
{
    VotingGroups votes;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

    for (auto [key, value] : identityFeatures.asKeyValueRange())
    {

#else

    for (const auto& key : identityFeatures.keys())
    {
        const auto& value = identityFeatures[key];

#endif

        for (const cv::Mat& feature : std::as_const(value))
        {
            float distance = 10000.0;

            // TODO: add a feature compare for OpenFace if we decide to keep it

            if (featureSFaceCompare(target, feature, distance))
            {
                votes.addVote(key, distance/value.count());
            }
        }
    }

    return votes.winner(VotingGroups::VotesLowScore);
}

// TODO: add a feature compare for OpenFace if we decide to keep it

bool FaceClassifier::featureSFaceCompare(const cv::Mat& target, const cv::Mat& sample, float& distance) const
{
    double cosDistance     = sum(target.mul(sample))[0];
    double norm_l1Distance = norm(target, sample);

    // Recompute l1Threshold for SFace based on passed in sqRange.
    // d->recognizeThreshold is controlled via the UI.
    // We use the inverse of the cos result to compare to sqRange
    // and norm_l1 distance compared to recomputed l1Threshold.
    // norm_l1 is almost always < 1 for a good match. We add .1 of d->recognizeThreshold for extra verification
    // to avoid false negatives.

    float l1Threshold = 1.0 + (threshold / 10.0);

    // NOTE: both cosine distance and l1 distance can help to avoid errors with similarity prediction.

    distance = (1.0 - cosDistance);

    return (distance < threshold) &&
           (norm_l1Distance < l1Threshold);
}

// int FaceClassifier::predictClassifier(const cv::Mat& target)    const
// {
//     cv::Mat knn_resultMat, knn_neighbors, knn_distances;
//     int     label                   = -1;
//     IdentityProvider* idProvider    = IdentityProvider::instance();
//     QMap<int, QList<cv::Mat> > featureSet;

//     QElapsedTimer timer;
//     timer.start();

//     // get the K nearest from the KNN classifier

//     if (d->knnClassifier->isTrained())
//     {
//         d->knnClassifier->findNearest(target,
//                                     std::min<int>(d->identityFeatures.count(), d->knn_defaultK),
//                                     knn_resultMat,
//                                     knn_neighbors,
//                                     knn_distances);

//         // create a map of the nearest neighbors face features lists

//         for (int i = 0; i < knn_neighbors.cols; ++i)
//         {
//             if (idProvider->isValidId(knn_neighbors.at<float>(0,i)))
//             {
//                 featureSet[knn_neighbors.at<float>(0,i)] = d->identityFeatures[knn_neighbors.at<float>(0,i)];
//             }
//         }
//     }
//     else
//     {
//         featureSet = d->identityFeatures;
//     }

//     // search through the lists for the closest match 

//     label = listSearch(target, featureSet);

//     qCDebug(DIGIKAM_FACESENGINE_LOG) << "FaceClassifier::predictClassifier: search prediction is: "  << label << "    completed in " << timer.elapsed();

//     return label;
// }

// int FaceClassifier::predictClassifier(const cv::Mat& target)    const
// {
//     int     label           = -1;
//     cv::Mat knn_resultMat;
//     cv::Mat knn_neighbors;
//     cv::Mat knn_distances;
//     cv::Mat svm_prediction;
//     IdentityProvider* idProvider = IdentityProvider::instance();

//     QElapsedTimer timer;
//     timer.start();

//     // stage 1 - compare SVM and KNN classifier results

//     d->svmClassifier->predict(target,
//                               svm_prediction);
//     int svm_result = int(svm_prediction.at<float>(0,0));

//     d->knnClassifier->findNearest(target,
//                                                     //    std::min<int>(d->identityFeatures.count(), d->knn_defaultK),
//                                                        5,
//                                                        knn_resultMat,
//                                                        knn_neighbors,
//                                                        knn_distances);

//     int knn_result = int(knn_resultMat.at<float>(0,0));
//     // check if results exist in recognition DB

//     if (!idProvider->isValidId(svm_result))
//     {
//         svm_result = -1;
//     }
//     if (!idProvider->isValidId(knn_result))
//     {
//         knn_result = -1;
//     }

//     // if the  SVM and KNN classifiers agree it's over 98% correct

//     if (svm_result == knn_result)
//     {
//         qCDebug(DIGIKAM_FACESENGINE_LOG) << "FaceClassifier::predictClassifier: classifier prediction is: "  << svm_result << "    completed in " << timer.elapsed();
//         return svm_result;
//     }

//     // Stage 2 - SVM and KNN didn't match

//     // next step is to look at the count and distances returned from the KNN
//     // we use counts first, if there is a tie for the number of counts, then
//     // use the average distances. Count only votes within the threshold distance.

//     knn_result = d->knnClassifier->findNearest(target,
//                                                std::min<int>(d->identityFeatures.count(), d->knn_defaultK),
//                                                knn_resultMat,
//                                                knn_neighbors,
//                                                knn_distances);

//     {
//         QMap<int, QList<float> > voteDistances;

//         for (int i = 0; i < knn_distances.cols; ++i)
//         {
//             float flabel = knn_neighbors.at<float>(0,i);
//             float dist = knn_distances.at<float>(0,i);
//             voteDistances[int(flabel)].append(dist);
//             // qCDebug(DIGIKAM_FACESENGINE_LOG) << "knn_distance " << i << " is " << dist;
//         }

//         VotingGroups votes;

//         for(auto [key, value] : voteDistances.asKeyValueRange())
//         {
//             // verify the key (label) exists in the identity set

//             if (idProvider->isValidId(key))
//             {
//                 for(const float& dist : std::as_const(value) )
//                 {
//                     // only count votes within the threshold
//                     if (dist < threshold)
//                     {
//                         // divide the distance by the count so the sum is the average
//                         votes.addVote(key, dist/value.count());
//                     }
//                 }
//             }
//         }

//         label = votes.winner(MLClassifierFoundation::VotingGroups::VotesLowScore);
//         // label |= votes.winner(MLClassifierFoundation::VotingGroups::VotesHighScore);
//         // label |= votes.winner(MLClassifierFoundation::VotingGroups::LowScore);
//         // label |= votes.winner(MLClassifierFoundation::VotingGroups::HighScore);

//         if (-1 != label)
//         {
//             // label = votes.winner(MLClassifierFoundation::VotingGroups::VotesLowScore);
//             // label = votes.winner(MLClassifierFoundation::VotingGroups::VotesHighScore);
//             // label = votes.winner(MLClassifierFoundation::VotingGroups::LowScore);
//             // label = votes.winner(MLClassifierFoundation::VotingGroups::HighScore);

//             qCDebug(DIGIKAM_FACESENGINE_LOG) << "FaceClassifier::predictClassifier: knearest prediction is: "  << label << "    completed in " << timer.elapsed();

//             return label;
//         }

//     }

//     // Stage 3 - do brute-force search on set of SVM and KNN labels

//     {
//         QMap<int, QList<cv::Mat> > featureSet;
//         if (idProvider->isValidId(svm_result))
//         {
//             featureSet[svm_result] = d->identityFeatures[svm_result];
//         }

//         for (int i = 0; i < knn_neighbors.cols; ++i)
//         {
//             if (idProvider->isValidId(knn_neighbors.at<float>(0,i)))
//             {
//                 featureSet[knn_neighbors.at<float>(0,i)] = d->identityFeatures[knn_neighbors.at<float>(0,i)];
//             }
//         }

//         label = listSearch(target, featureSet);
//     }

//     qCDebug(DIGIKAM_FACESENGINE_LOG) << "FaceClassifier::predictClassifier: search prediction is: "  << label << "    completed in " << timer.elapsed();

//     return label;
// }

} // namespace Digikam

#include "moc_faceclassifier.cpp"
