/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-06-01
 * Description : Face recognition using deep learning
 *               The internal DNN library interface
 *
 * Copyright (C) 2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "dnnfaceextractor.h"

// Qt includes

#include <QMutex>
#include <QString>
#include <QDataStream>
#include <QElapsedTimer>
#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"
#include "recognitionpreprocessor.h"

namespace Digikam
{

class Q_DECL_HIDDEN DNNFaceExtractor::Private
{
public:

    Private()
        : preprocessor      (nullptr),
          ref               (1),

          // As we use OpenFace, we need to set appropriate values for image color space and image size

          imageSize         (cv::Size(96, 96)),
          scaleFactor       (1.0F / 255.0F),
          meanValToSubtract (cv::Scalar(0.0, 0.0, 0.0))
    {
    }

    ~Private()
    {
        delete preprocessor;
    }

public:

    RecognitionPreprocessor* preprocessor;

    int                      ref;

    cv::dnn::Net             net;
    QMutex                   mutex;

    cv::Size                 imageSize;
    float                    scaleFactor;
    cv::Scalar               meanValToSubtract;
};

DNNFaceExtractor::DNNFaceExtractor()
    : d(new Private)
{
    loadModels();
}

DNNFaceExtractor::DNNFaceExtractor(const DNNFaceExtractor& other)
    : d(other.d)
{
    ++(d->ref);
}

DNNFaceExtractor::~DNNFaceExtractor()
{
    --(d->ref);

    if (d->ref == 0)
    {
        delete d;
    }
}

bool DNNFaceExtractor::loadModels()
{
/*
    QString proto   = QLatin1String("ResNet-50-deploy.prototxt");
    QString model   = QLatin1String("ResNet-50-model.caffemodel");

    QString nnproto = QStandardPaths::locate(QStandardPaths::AppDataLocation,
                                             QLatin1String("facesengine/%1").arg(proto));
    QString nnmodel = QStandardPaths::locate(QStandardPaths::AppDataLocation,
                                             QLatin1String("facesengine/%1").arg(model)));

    if (!nnproto.isEmpty() && !nnmodel.isEmpty())
    {
        qCDebug(DIGIKAM_FACEDB_LOG) << nnproto;
        qCDebug(DIGIKAM_FACEDB_LOG) << nnmodel;

        d->net = cv::dnn::readNetFromCaffe(nnproto.toStdString(), nnmodel.toStdString());
    }
    else
    {
        qCCritical(DIGIKAM_FACEDB_LOG) << "Cannot found faces engine DNN model" << proto << "or" << model;
        qCCritical(DIGIKAM_FACEDB_LOG) << "Faces recognition feature cannot be used!";

        return false;
    }
*/

    d->preprocessor = new RecognitionPreprocessor;
    d->preprocessor->init(PreprocessorSelection::OPENFACE);

    QString name    = QLatin1String("openface_nn4.small2.v1.t7");
    QString nnmodel = QStandardPaths::locate(QStandardPaths::AppDataLocation,
                                             QString::fromLatin1("facesengine/%1").arg(name));

    if (!nnmodel.isEmpty())
    {
        try
        {
            qCDebug(DIGIKAM_FACEDB_LOG) << "Extractor model:" << nnmodel;

#ifdef Q_OS_WIN

            d->net = cv::dnn::readNetFromTorch(nnmodel.toLocal8Bit().constData());

#else

            d->net = cv::dnn::readNetFromTorch(nnmodel.toStdString());

#endif

        }
        catch (cv::Exception& e)
        {
            qCWarning(DIGIKAM_FACEDB_LOG) << "cv::Exception:" << e.what();

            return false;
        }
        catch (...)
        {
           qCWarning(DIGIKAM_FACEDB_LOG) << "Default exception from OpenCV";

           return false;
        }
    }
    else
    {
        qCCritical(DIGIKAM_FACEDB_LOG) << "Cannot found faces engine DNN model" << name;
        qCCritical(DIGIKAM_FACEDB_LOG) << "Faces recognition feature cannot be used!";

        return false;
    }

    return true;
}

double DNNFaceExtractor::cosineDistance(std::vector<float> v1,
                                        std::vector<float> v2)
{
    Q_ASSERT(v1.size() == v2.size());

    double scalarProduct = std::inner_product(v1.begin(), v1.end(), v2.begin(), 0.0);
    double normV1        = sqrt(std::inner_product(v1.begin(), v1.end(), v1.begin(), 0.0));
    double normV2        = sqrt(std::inner_product(v2.begin(), v2.end(), v2.begin(), 0.0));

    return (scalarProduct / (normV1 * normV2));
}

double DNNFaceExtractor::L2squareDistance(std::vector<float> v1,
                                          std::vector<float> v2)
{
    Q_ASSERT(v1.size() == v2.size());

    double sqrDistance = 0.0;

    for (size_t i = 0 ; i < v1.size() ; ++i)
    {
        sqrDistance += pow((v1[i] - v2[i]), 2);
    }

    return sqrDistance;
}

double DNNFaceExtractor::L2squareNormDistance(std::vector<float> v1, std::vector<float> v2)
{
    Q_ASSERT(v1.size() == v2.size());

    double normV1      = sqrt(std::inner_product(v1.begin(), v1.end(), v1.begin(), 0.0));
    double normV2      = sqrt(std::inner_product(v2.begin(), v2.end(), v2.begin(), 0.0));
    double sqrDistance = 0.0;

    for (size_t i = 0 ; i < v1.size() ; ++i)
    {
        sqrDistance += pow((v1[i]/normV1 - v2[i]/normV2), 2);
    }

    return sqrDistance;
}

cv::Mat DNNFaceExtractor::vectortomat(const std::vector<float>& vector)
{
    cv::Mat mat(1, vector.size(), 5);

    memcpy(mat.data, vector.data(), vector.size()*sizeof(float));

    return mat;
}

QJsonArray DNNFaceExtractor::encodeVector(const std::vector<float>& vector)
{
    QJsonArray array;

    for (size_t i = 0 ; i < vector.size() ; ++i)
    {
        array << vector[i];
    }

    return array;
}

std::vector<float> DNNFaceExtractor::decodeVector(const QJsonArray& json)
{
    std::vector<float> vector;

    for (int i = 0 ; i < json.size() ; ++i)
    {
        vector.push_back(static_cast<float>(json[i].toDouble()));
    }

    return vector;
}


cv::Mat DNNFaceExtractor::alignFace(const cv::Mat& inputImage) const
{
    return d->preprocessor->preprocess(inputImage);
}

cv::Mat DNNFaceExtractor::getFaceEmbedding(const cv::Mat& faceImage)
{
    cv::Mat face_descriptors;
    cv::Mat alignedFace;
/*
    qCDebug(DIGIKAM_FACEDB_LOG) << "faceImage channels: " << faceImage.channels();
    qCDebug(DIGIKAM_FACEDB_LOG) << "faceImage size: (" << faceImage.rows << ", " << faceImage.cols << ")\n";
*/
    QElapsedTimer timer;

    timer.start();
/*
    alignedFace = faceImage;
*/
    alignedFace = d->preprocessor->preprocess(faceImage);

    qCDebug(DIGIKAM_FACEDB_LOG) << "Finish aligning face in " << timer.elapsed() << " ms";
    qCDebug(DIGIKAM_FACEDB_LOG) << "Start neural network";

    timer.start();

    cv::Mat blob = cv::dnn::blobFromImage(alignedFace, d->scaleFactor, d->imageSize, cv::Scalar(), true, false);

    d->mutex.lock();
    {
        d->net.setInput(blob);
        face_descriptors = d->net.forward();
    }
    d->mutex.unlock();

    qCDebug(DIGIKAM_FACEDB_LOG) << "Finish computing face embedding in "
                                << timer.elapsed() << " ms";

/*
    cv::Mat blob = cv::dnn::blobFromImage(faceImage, 1.0 / 255, cv::Size(96, 96), cv::Scalar(0,0,0), false, true, CV_32F); // work for openface.nn4
    cv::Mat blob = cv::dnn::blobFromImage(faceImage, 1.0 / 255, cv::Size(224,224), cv::Scalar(0,0,0), false, true, CV_32F);
    net.setInput(blob);
    cv::Mat face_descriptors = net.forward();
*/

    return face_descriptors;
}

} // namespace Digikam
