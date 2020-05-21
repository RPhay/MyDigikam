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
          extractor(new FaceExtractor)
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

Identity FaceRecognizer::findIdenity(const cv::Mat& inputImage)
{
    std::vector<float> faceEmbedding = d->extractor->getFaceEmbedding(inputImage);

    // TODO: scan database for face

    Identity id;

    QJsonArray jsonFaceEmbedding;

    for (size_t i = 0; i < faceEmbedding.size(); ++i)
    {
        jsonFaceEmbedding << faceEmbedding[i];
    }

    // TODO add face embedding to identity
    id.setAttribute(QLatin1String("faceEmbedding"), QString::fromLatin1(QJsonDocument(jsonFaceEmbedding).toJson(QJsonDocument::Compact)));

    return id;
}

void FaceRecognizer::saveIdentity(const Identity& id)
{
    // TODO save identity
}

}
