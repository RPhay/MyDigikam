#include "facerecognizer.h"


// Qt includes

#include <QElapsedTimer>
#include <QJsonArray>
#include <QJsonDocument>

// TODO officilally include opencv_ml to CMake
// cv include
#include <opencv2/ml.hpp>

// Local includes
//#include "facedbaccess.h"
//#include "facedb.h"
#include "dnnfacemodel.h"
#include "digikam_debug.h"
#include "faceextractor.h"
//#include "dnndbscan.h"

using namespace Digikam;

namespace RecognitionTest
{

class Q_DECL_HIDDEN FaceRecognizer::Private
{
public:

    Private(bool debug)
        : debugMode(debug),
          extractor(new FaceExtractor),
          svm(cv::ml::SVM::create()),
          knn(cv::ml::KNearest::create()),
          identityCounter(0)
    {
        // use linear mapping
        svm->setKernel(cv::ml::SVM::LINEAR);

        // parameterize KNN
        // NOTE KD TREE Of OpenCV not working, causing dead loop
        //knn->setAlgorithmType(cv::ml::KNearest::KDTREE);
        knn->setDefaultK(17);
        knn->setIsClassifier(true);
    }

    ~Private()
    {
        delete extractor;
    }

public:

    int trainSVM();
    int trainKNN();
    Identity predictSVM(cv::Mat faceEmbedding);
    Identity predictKNN(cv::Mat faceEmbedding, int k);
    Identity predictL2(const std::vector<float>& faceEmbedding, double confidenceThreshold, bool normalized = false);
    Identity predictCosine(const std::vector<float>& faceEmbedding, double confidenceThreshold, bool mean = false);

public:

    bool debugMode;
    // TODO verify recognition threshold
    float threshold = 15000.0;

    FaceExtractor* extractor;
    cv::Ptr<cv::ml::SVM>      svm;
    cv::Ptr<cv::ml::KNearest> knn;

    int identityCounter;

    QHash<QString, QVector<Identity> > faceLibrary;
    QVector<QString> labels;
};

int FaceRecognizer::Private::trainSVM()
{
    cv::Mat features, groups;

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

            //qDebug() << "face embedding of" << iter.value().attribute(QLatin1String("fullName")) << ":" << jsonFaceEmbedding;

            std::vector<float> recordedFaceEmbedding;

            for (int i = 0; i < jsonFaceEmbedding.size(); ++i)
            {
                recordedFaceEmbedding.push_back(static_cast<float>(jsonFaceEmbedding[i].toDouble()));
            }

            groups.push_back(i);
            features.push_back(FaceExtractor::vectortomat(recordedFaceEmbedding));

            ++size;
        }

    }

    svm->train(features, 0, groups);

    qDebug() << "Support vector machine trains" << size << "samples in" << timer.elapsed() << "ms";

    return size;
}

int FaceRecognizer::Private::trainKNN()
{
    cv::Mat features, groups;

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

            //qDebug() << "face embedding of" << iter.value().attribute(QLatin1String("fullName")) << ":" << jsonFaceEmbedding;

            std::vector<float> recordedFaceEmbedding;

            for (int i = 0; i < jsonFaceEmbedding.size(); ++i)
            {
                recordedFaceEmbedding.push_back(static_cast<float>(jsonFaceEmbedding[i].toDouble()));
            }

            groups.push_back(i);
            features.push_back(FaceExtractor::vectortomat(recordedFaceEmbedding));

            ++size;
        }

    }

    qDebug() << "Start training KNN";
    knn->train(features, 0, groups);

    qDebug() << "KNN trains" << size << "samples in" << timer.elapsed() << "ms";

    return size;
}

Identity FaceRecognizer::Private::predictSVM(cv::Mat faceEmbedding)
{
    if (!svm->isTrained())
    {
        trainSVM();
    }

    // perdict
    float id = svm->predict(faceEmbedding);

    return faceLibrary[labels[int(id)]][0];
}

Identity FaceRecognizer::Private::predictKNN(cv::Mat faceEmbedding, int k)
{
    if (!knn->isTrained())
    {
        trainKNN();
    }

    cv::Mat input, output;
    //input.push_back(faceEmbedding);

    knn->findNearest(faceEmbedding, k, output);

    float id = output.at<float>(0);

    return faceLibrary[labels[int(id)]][0];
}

Identity FaceRecognizer::Private::predictL2(const std::vector<float>& faceEmbedding, double confidenceThreshold, bool normalized)
{
    double bestDistance = 2;

    QVector<Identity>::iterator prediction;

    for (QHash<QString, QVector<Identity> >::iterator group  = faceLibrary.begin();
                                                      group != faceLibrary.end();
                                                    ++group)
    {
        for (QVector<Identity>::iterator iter  = group.value().begin();
                                         iter != group.value().end();
                                       ++iter)
        {
            QJsonArray jsonFaceEmbedding = QJsonDocument::fromJson(iter->attribute(QLatin1String("faceEmbedding")).toLatin1()).array();

            //qDebug() << "face embedding of" << iter->attribute(QLatin1String("faceEmbedding")) << ":" << jsonFaceEmbedding;

            std::vector<float> recordedFaceEmbedding;

            for (int i = 0; i < jsonFaceEmbedding.size(); ++i)
            {
                recordedFaceEmbedding.push_back(static_cast<float>(jsonFaceEmbedding[i].toDouble()));
            }

            double distance;

            if (normalized)
            {
                distance = FaceExtractor::L2squareNormDistance(recordedFaceEmbedding, faceEmbedding);
            }
            else
            {
                distance = FaceExtractor::L2squareDistance(recordedFaceEmbedding, faceEmbedding);
            }

            if (distance < bestDistance)
            {
                bestDistance = distance;
                prediction   = iter;
            }
        }
    }

    if (bestDistance < confidenceThreshold)
    {
        return *prediction;
    }

    // new identity
    QJsonArray jsonFaceEmbedding;

    for (size_t i = 0; i < faceEmbedding.size(); ++i)
    {
        jsonFaceEmbedding << faceEmbedding[i];
    }

    Identity id;
    id.setAttribute(QLatin1String("faceEmbedding"), QString::fromLatin1(QJsonDocument(jsonFaceEmbedding).toJson(QJsonDocument::Compact)));

    return id;
}

Identity FaceRecognizer::Private::predictCosine(const std::vector<float>& faceEmbedding, double confidenceThreshold, bool mean)
{
    double bestDistance = -1;
    QVector<Identity>::iterator prediction;

    // For mean cosdistance prediction
    QHash<QString, QVector<double> > cosDistances;

    for (QHash<QString, QVector<Identity> >::iterator group  = faceLibrary.begin();
                                                      group != faceLibrary.end();
                                                    ++group)
    {
        for (QVector<Identity>::iterator iter  = group.value().begin();
                                         iter != group.value().end();
                                       ++iter)
        {
            QJsonArray jsonFaceEmbedding = QJsonDocument::fromJson(iter->attribute(QLatin1String("faceEmbedding")).toLatin1()).array();

            std::vector<float> recordedFaceEmbedding;

            for (int i = 0; i < jsonFaceEmbedding.size(); ++i)
            {
                recordedFaceEmbedding.push_back(static_cast<float>(jsonFaceEmbedding[i].toDouble()));
            }

            double distance = FaceExtractor::cosineDistance(recordedFaceEmbedding, faceEmbedding);

            if (mean)
            {
                cosDistances[group.key()].append(distance);
            }
            else
            {
                if (distance > bestDistance)
                {
                    bestDistance  = distance;
                    prediction    = iter;
                }
            }
        }
    }

    if (mean)
    {
        for (QHash<QString, QVector<double> >::iterator iter  = cosDistances.begin();
                                                        iter != cosDistances.end();
                                                      ++iter)
        {
            double meanCosDistance = 0;

            for (int i = 0; i < iter.value().size(); ++i)
            {
                meanCosDistance += iter.value()[i];
            }

            meanCosDistance /= iter.value().size();

            if (meanCosDistance > bestDistance)
            {
                bestDistance = meanCosDistance;
                prediction   = faceLibrary[iter.key()].begin();
            }
        }
    }

    if (bestDistance > confidenceThreshold)
    {
        return *prediction;
    }

    // new identity
    QJsonArray jsonFaceEmbedding;

    for (size_t i = 0; i < faceEmbedding.size(); ++i)
    {
        jsonFaceEmbedding << faceEmbedding[i];
    }

    Identity id;
    id.setAttribute(QLatin1String("faceEmbedding"), QString::fromLatin1(QJsonDocument(jsonFaceEmbedding).toJson(QJsonDocument::Compact)));

    return id;
}

/* -----------------------------------------------------------------------------------------------------------------------------------------------------*/

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

int FaceRecognizer::recognize(const cv::Mat& inputImage)
{
    int predictedLabel = -1;
    double confidence  = 0;

    // TODO predict label
    //d->dnn()->predict(inputImage, predictedLabel, confidence, d->m_extractor);

    qCDebug(DIGIKAM_FACESENGINE_LOG) << "predictedLabel: " << predictedLabel << ", confidence: " << confidence;

    /**
     * confidence must be greater than threshold, because distance used is cosine distance
     * in case that we use euclidean distance, confidence must be less than threshold
     */
    if (confidence < d->threshold)
    {
        return -1;
    }

    return predictedLabel;
}

Identity FaceRecognizer::findIdenity(const cv::Mat& preprocessedImage, ComparisonMetric metric, double threshold)
{
    std::vector<float> faceEmbedding = d->extractor->getFaceEmbedding(preprocessedImage);

    switch (metric)
    {
        case CosDistance:
            return d->predictCosine(faceEmbedding, threshold);
            break;
        case MeanCosDistance:
            return d->predictCosine(faceEmbedding, threshold, false);
            break;
        case L2Distance:
            return d->predictL2(faceEmbedding, threshold);
            break;
        case L2NormDistance:
            return d->predictL2(faceEmbedding, threshold, true);
            break;
        case SupportVectorMachine:
            return d->predictSVM(d->extractor->getFaceDescriptor(preprocessedImage));
            break;
        case KNN:
            if (int(threshold) < 1)
            {
                threshold = 5;
            }

            return d->predictKNN(d->extractor->getFaceDescriptor(preprocessedImage), int(threshold));
            break;
        default:
            break;
    }

    return Identity();
}

Identity FaceRecognizer::newIdentity(const cv::Mat& preprocessedImage)
{
    std::vector<float> faceEmbedding = d->extractor->getFaceEmbedding(preprocessedImage);

    // new identity
    QJsonArray jsonFaceEmbedding;

    for (size_t i = 0; i < faceEmbedding.size(); ++i)
    {
        jsonFaceEmbedding << faceEmbedding[i];
    }

    Identity id;
    id.setAttribute(QLatin1String("faceEmbedding"),
                    QString::fromLatin1(QJsonDocument(jsonFaceEmbedding).toJson(QJsonDocument::Compact)));

    return id;
}

void FaceRecognizer::saveIdentity(Identity& id)
{
    QString label = id.attribute(QLatin1String("fullName"));
    // TODO save identity in database
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
}
}
