/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-11-10
 * Description : Face classifier
 *
 * SPDX-FileCopyrightText: 2024-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
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

    // OpenCV variables

    cv::Ptr<cv::ml::KNearest>               knnClassifier;
    cv::Ptr<cv::ml::SVM>                    svmClassifier;

    // Classifier hyperparameters

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

Q_GLOBAL_STATIC(FaceClassifierCreator, faceClassifierCreator)

// -----------------------------------------------------------

FaceClassifier::FaceClassifier()
    : FaceClassifierBase()
{
    if (!d)
    {
        d                   = new Private;
        d->identityProvider = IdentityProvider::instance();

        // Use dynamic binding as retrain() function is virtual and called in constructor.

        this->retrain();
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
    return &faceClassifierCreator->object;
}

bool FaceClassifier::ready() const
{
    return d->ready;
}

void FaceClassifier::setParameters(const FaceScanSettings& parameters)
{
    d->recognizeModel = parameters.recognizeModel;

    // set the threshold for the model in case we can't get it from the model

    threshold         = 0.5F;

    try
    {
        // TODO: add other models if we decide to keep OpenFace

        if (d->recognizeModel == FaceScanSettings::FaceRecognitionModel::SFace)
        {
            // get a pointer to the model

            DNNModelBase* const model = DNNModelManager::instance()->getModel(QLatin1String("sface"),
                                                                              DNNModelUsage::DNNUsageFaceRecognition);

            // check if the model is loaded

            if (model)
            {
                // get the threshold from the model

                threshold = model->getThreshold(parameters.recognizeAccuracy);
            }
        }
    }

    catch (const std::exception& e)
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "FaceClassifier::setParameters: exception: " << e.what();
    }

    catch (...)
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "FaceClassifier::setParameters: exception:";
    }
}

cv::Ptr<cv::ml::KNearest> FaceClassifier::createKNearest()
{
    // create the KNN classifier

    cv::Ptr<cv::ml::KNearest> cvClassifier = cv::ml::KNearest::create();
    cvClassifier->setIsClassifier(true);
    cvClassifier->setAlgorithmType(d->knn_algorithm);
    cvClassifier->setDefaultK(d->knn_defaultK);

    return cvClassifier;
}

cv::Ptr<cv::ml::SVM> FaceClassifier::createSVM()
{
    // create the SVM classifier

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
    // called to retrain the classifier

    // lock the mutex

    QMutexLocker lock(&d->trainingMutex);

    // tell the training thread there is training to do

    d->trainingWaiting = true;

    // check if the training thread is already running

    if (d->trainingFuture.future().isRunning())
    {
        // training thread is already running

        return false;
    }

    // start the training thread

    d->trainingFuture.setFuture(QtConcurrent::run(
                                                  QThreadPool::globalInstance(),

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

                                                  &FaceClassifier::loadTrainingData,
                                                  this

#else

                                                  this,
                                                  &FaceClassifier::loadTrainingData

#endif

                                                 )
    );

/*
        connect(&d->trainingFuture, &QFutureWatcher<bool>::finished,
                this, &FaceClassifier::trainingComplete);
*/

    return true;
}

bool FaceClassifier::loadTrainingData()
{
    // training thread to load the training data

    // training thread should have a higher priority

    QThread::currentThread()->setPriority(QThread::Priority::HighPriority);

    QElapsedTimer timer;
    timer.start();

    // loop to rebuild the classifier components if there is training waiting
    // d->trainingWaiting can be modified outside the loop

    do
    {
        // clear the training waiting flag

        d->trainingWaiting            = false;

        // create new map and classifiers.  We'll swap them in at the end

        QMap<int, QList<cv::Mat> > identityFeatures;

        cv::Ptr<cv::ml::KNearest> knn = createKNearest();
        cv::Ptr<cv::ml::SVM> svm      = createSVM();

        try
        {
            // get the training data from the identity provider

            cv::Ptr<cv::ml::TrainData> trainData = d->identityProvider->getTrainingData();

            // if we have training data, split it into samples and labels

            if (nullptr != trainData)
            {
                cv::Mat samples = trainData->getSamples();
                cv::Mat labels  = trainData->getResponses();

                for (int i = 0 ; i < labels.rows ; ++i)
                {
                    int label = labels.at<int>(cv::Point(i, 0));
                    identityFeatures[label].append(samples.row(i));
                }
            }

            // if we have enough samples, train the classifiers

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
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "FaceClassifier::loadTrainingData: exception:";
        }

        // lock the classifiers and identity list

        d->trainingLock.lockForWrite();

        // swap the new classifiers and identity list with the currently running ones

        d->knnClassifier    = knn;
        d->svmClassifier    = svm;
        d->identityFeatures = identityFeatures;
        d->ready            = true;

        d->trainingLock.unlock();
    }
    while (d->trainingWaiting);

    qCDebug(DIGIKAM_FACESENGINE_LOG) << "FaceClassifier::loadTrainingData: training completed in "
                                     << timer.elapsed() << "ms";

    return true;
}

void FaceClassifier::trainingComplete()
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

int FaceClassifier::predictFullSearch(const cv::Mat& target)const
{
    QElapsedTimer timer;
    timer.start();

    int label = listSearch(target, d->identityFeatures);

    qCDebug(DIGIKAM_FACESENGINE_LOG) << "FaceClassifier::predictFullSearch: classifier prediction is: "
                                     << label << " completed in " << timer.elapsed();

    return label;
}

int FaceClassifier::predictClassifier(const cv::Mat& target) const
{
    int     label                      = -1;
    int     badLabel1                  = -1;
    int     badLabel2                  = -1;
    cv::Mat knn_resultMat;
    cv::Mat knn_neighbors;
    cv::Mat knn_distances;
    cv::Mat svm_prediction;
    IdentityProvider* const idProvider = IdentityProvider::instance();

    QElapsedTimer timer;
    timer.start();

    // stage 1 - compare SVM and KNN classifier results

    d->svmClassifier->predict(target, svm_prediction);
    int svm_result = int(svm_prediction.at<float>(0, 0));

    d->knnClassifier->findNearest(target,
                                  std::min<int>(d->identityFeatures.count(), d->knn_defaultK),
                                  knn_resultMat,
                                  knn_neighbors,
                                  knn_distances);

    int knn_result = int(knn_resultMat.at<float>(0, 0));

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

    if ((svm_result == knn_result) && (svm_result != -1))
    {
        if (validateKNNSVMResult(target, svm_result))
        {
            // we have a match

            qCDebug(DIGIKAM_FACESENGINE_LOG) << "FaceClassifier::predictClassifier: classifier stage 1 prediction is: "
                                             << svm_result << " completed in " << timer.elapsed();

            return svm_result;
        }

        else
        {
            // save the bad label so we don't check it again

            badLabel1 = svm_result;
        }
    }

    // Stage 2 - check the absolute nearest neighbor for a match to the SVM result

    if ((-1 != svm_result) && (-1 != knn_result))
    {
        // find the absolute nearest neighbor

        float distance = 10000.0F;

        for (int i = 0 ; i < knn_neighbors.cols ; ++i)
        {
            if (distance > abs(knn_distances.at<float>(0, i)))
            {
                label    = int(knn_neighbors.at<float>(0, i));
                distance = abs(knn_distances.at<float>(0, i));
            }
        }

        // verify we haven't checked this label before

        if ((label == svm_result) && (label != badLabel1))
        {
            // check the distance of the target to the samples for the label

            if (validateKNNSVMResult(target, label))
            {
                // we have a match

                qCDebug(DIGIKAM_FACESENGINE_LOG) << "FaceClassifier::predictClassifier: classifier stage 2 prediction is: "
                                                 << label << " completed in " << timer.elapsed();

                return label;
            }

            else
            {
                // save the bad label so we don't check it again

                badLabel2 = label;
            }
        }
    }

    // Stage 3 - do brute-force search on distinct set of SVM and KNN labels returned
    // from the internal classifiers.  Ignore labels we've already checked.

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
        float distance = 10000.0F;

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
            float distance = 10000.0F;

            // TODO: add a feature compare for OpenFace if we decide to keep it

            if (featureSFaceCompare(target, feature, distance))
            {
                votes.addVote(key, distance / value.count());
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

    float l1Threshold      = 1.0F + (threshold / 10.0F);

    // NOTE: both cosine distance and l1 distance can help to avoid errors with similarity prediction.

    distance = (1.0 - cosDistance);

    return (
            (distance        < threshold) &&
            (norm_l1Distance < l1Threshold)
           );
}

} // namespace Digikam
