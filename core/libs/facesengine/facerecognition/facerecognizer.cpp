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
          identityCounter(0)
    {
        // use linear mapping
        svm->setKernel(cv::ml::SVM::LINEAR);
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

                //qDebug() << "face embedding of" << iter.value().attribute(QLatin1String("fullName")) << ":" << jsonFaceEmbedding;

                std::vector<float> recordedFaceEmbedding;

                for (int i = 0; i < jsonFaceEmbedding.size(); ++i)
                {
                    recordedFaceEmbedding.push_back(static_cast<float>(jsonFaceEmbedding[i].toDouble()));
                }

                label.push_back(i);
                features.push_back(FaceExtractor::vectortomat(recordedFaceEmbedding));

                ++size;
            }

        }

        svm->train(features, 0, label);

        qDebug() << "Support vector machine trains" << size << "samples in" << timer.elapsed() << "ms";

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

public:

    bool           debugMode;
    // TODO verify recognition threshold
    float          threshold = 15000.0;

    FaceExtractor* extractor;
    cv::Ptr<cv::ml::SVM> svm;

    int            identityCounter;

    QHash<QString, QVector<Identity> > faceLibrary;
    QVector<QString> labels;
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
    // Use support vector machine to predict label
    if (metric == SupportVectorMachine)
    {
        return d->predictSVM(d->extractor->getFaceDescriptor(preprocessedImage));
    }

    std::vector<float> faceEmbedding = d->extractor->getFaceEmbedding(preprocessedImage);
    //qDebug() << "look for identity of" << faceEmbedding;

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

            //qDebug() << "face embedding of" << iter->attribute(QLatin1String("faceEmbedding")) << ":" << jsonFaceEmbedding;

            std::vector<float> recordedFaceEmbedding;

            for (int i = 0; i < jsonFaceEmbedding.size(); ++i)
            {
                recordedFaceEmbedding.push_back(static_cast<float>(jsonFaceEmbedding[i].toDouble()));
            }

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
                distance = FaceExtractor::L2Distance(recordedFaceEmbedding, faceEmbedding);

                if (distance < bestDistance)
                {
                    bestDistance = distance;
                    prediction   = iter;
                }

                break;
            case L2NormDistance:
                distance = FaceExtractor::L2NormDistance(recordedFaceEmbedding, faceEmbedding);

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
    QJsonArray jsonFaceEmbedding;

    for (size_t i = 0; i < faceEmbedding.size(); ++i)
    {
        jsonFaceEmbedding << faceEmbedding[i];
    }

    Identity id;

    //qDebug() << "Cannot find identity";

    id.setAttribute(QLatin1String("faceEmbedding"), QString::fromLatin1(QJsonDocument(jsonFaceEmbedding).toJson(QJsonDocument::Compact)));

    return id;
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
    // TODO save identity
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
