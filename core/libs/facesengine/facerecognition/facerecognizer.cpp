#include "facerecognizer.h"


// Qt includes

#include <QElapsedTimer>
#include <QJsonArray>
#include <QJsonDocument>

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
          identityCounter(0)
    {
    }

    ~Private()
    {
        delete extractor;
    }

public:

    bool           debugMode;
    // TODO verify recognition threshold
    float          threshold = 15000.0;

    FaceExtractor* extractor;

    int            identityCounter;

    QHash<int, Identity> faceLibrary;
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

Identity FaceRecognizer::findIdenity(const cv::Mat& preprocessedImage)
{
    std::vector<float> faceEmbedding = d->extractor->getFaceEmbedding(preprocessedImage);
    qDebug() << "look for identity of" << faceEmbedding;

    // TODO: scan database for face

    for (QHash<int, Identity>::iterator iter  = d->faceLibrary.begin();
                                        iter != d->faceLibrary.end();
                                      ++iter)
    {
        QJsonArray jsonFaceEmbedding = QJsonDocument::fromJson(iter.value().attribute(QLatin1String("faceEmbedding")).toLatin1()).array();

        qDebug() << "face embedding of" << iter.value().attribute(QLatin1String("fullName")) << ":" << jsonFaceEmbedding;

        std::vector<float> recordedFaceEmbedding;

        for (int i = 0; i < jsonFaceEmbedding.size(); ++i)
        {
            recordedFaceEmbedding.push_back(static_cast<float>(jsonFaceEmbedding[i].toDouble()));
        }

        double cosDistance = FaceExtractor::cosineDistance(recordedFaceEmbedding, faceEmbedding);
        double l2Distance  = FaceExtractor::L2Distance(recordedFaceEmbedding, faceEmbedding);

        qDebug() << "cosine distance with" << iter.value().attribute(QLatin1String("fullName")) << ":" << cosDistance;
        qDebug() << "L2     distance with" << iter.value().attribute(QLatin1String("fullName")) << ":" << l2Distance;

        if (cosDistance > 0.7)
        {
            return iter.value();
        }
    }

    // new identity
    QJsonArray jsonFaceEmbedding;

    for (size_t i = 0; i < faceEmbedding.size(); ++i)
    {
        jsonFaceEmbedding << faceEmbedding[i];
    }

    Identity id;

    qDebug() << "Cannot find identity";

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
    // TODO save identity
    if (id.attribute(QLatin1String("fullName")).isEmpty())
    {
        qWarning() << "idenitity is empty";

        return;
    }

    if (id.isNull())
    {
        id.setId(++d->identityCounter);
    }

    d->faceLibrary[id.id()] = id;
}

}
