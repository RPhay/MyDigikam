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

#include "faceextractor.h"

// Qt includes

#include <QString>
#include <QFile>
#include <QDataStream>
#include <QStandardPaths>
#include <QElapsedTimer>

// Local includes

#include "digikam_debug.h"
#include "recognitionpreprocessor.h"

using namespace Digikam;

namespace RecognitionTest
{

class Q_DECL_HIDDEN FaceExtractor::Private
{
public:

    Private()
        : preprocessor(nullptr)
    {
    }

    ~Private()
    {
        delete preprocessor;
    }

public:

    RecognitionPreprocessor* preprocessor;

    cv::dnn::Net  net;

    cv::Size      imageSize;
    float         scaleFactor;
    cv::Scalar    meanValToSubtract;
};

double FaceExtractor::cosineDistance(std::vector<float> v1,
                                     std::vector<float> v2)
{
    assert(v1.size() == v2.size());

    double scalarProduct = std::inner_product(v1.begin(), v1.end(), v2.begin(), 0.0);
    double normV1        = sqrt(std::inner_product(v1.begin(), v1.end(), v1.begin(), 0.0));
    double normV2        = sqrt(std::inner_product(v2.begin(), v2.end(), v2.begin(), 0.0));

    return (scalarProduct / (normV1 * normV2));
}

double FaceExtractor::L2Distance(std::vector<float> v1,
                                 std::vector<float> v2)
{
    assert(v1.size() == v2.size());

    double sqrDistance = 0;

    for (size_t i = 0; i < v1.size(); ++i)
    {
        sqrDistance += pow((v1[i] - v2[i]), 2);
    }

    return sqrt(sqrDistance);
}

double FaceExtractor::L2NormDistance(std::vector<float> v1, std::vector<float> v2)
{
    assert(v1.size() == v2.size());

    double normV1        = sqrt(std::inner_product(v1.begin(), v1.end(), v1.begin(), 0.0));
    double normV2        = sqrt(std::inner_product(v2.begin(), v2.end(), v2.begin(), 0.0));

    double sqrDistance = 0;

    for (size_t i = 0; i < v1.size(); ++i)
    {
        sqrDistance += pow((v1[i]/normV1 - v2[i]/normV2), 2);
    }

    return sqrt(sqrDistance);
}

cv::Mat FaceExtractor::vectortomat(const std::vector<float>& vector)
{
    cv::Mat mat(1, vector.size(), 5);

    memcpy(mat.data, vector.data(), vector.size()*sizeof(float));

    return mat;
}

FaceExtractor::FaceExtractor()
    : d(new Private)
{
    // Read pretrained neural network for face recognition

/*
    QString nnproto = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                             QLatin1String("digikam/facesengine/dnnface/deep-residual-networks/ResNet-50-deploy.prototxt"));
    QString nnmodel = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                             QLatin1String("digikam/facesengine/dnnface/deep-residual-networks/ResNet-50-model.caffemodel"));
    net = cv::dnn::readNetFromCaffe(nnproto, nnmodel);
*/
    d->preprocessor = new RecognitionPreprocessor;
    d->preprocessor->init(PreprocessorSelection::OPENFACE);

    QString nnmodel = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                             QLatin1String("digikam/facesengine/openface_nn4.small2.v1.t7"));
    qCDebug(DIGIKAM_FACEDB_LOG) << nnmodel;

    d->net               = cv::dnn::readNetFromTorch(nnmodel.toStdString());
    // As we use OpenFace, we need to set appropriate values for image color space and image size

    d->imageSize         = cv::Size(96, 96);
    d->scaleFactor       = 1.0F / 255.0F;
    d->meanValToSubtract = cv::Scalar(0.0, 0.0, 0.0);
}

FaceExtractor::~FaceExtractor()
{
    delete d;
}

cv::Mat FaceExtractor::alignFace(const cv::Mat& inputImage)
{
    return d->preprocessor->preprocess(inputImage);
}

std::vector<float> FaceExtractor::getFaceEmbedding(const cv::Mat& faceImage)
{
    cv::Mat face_descriptors = getFaceDescriptor(faceImage);

    qCDebug(DIGIKAM_FACEDB_LOG) << "Face descriptors size: (" << face_descriptors.rows
                                << ", " << face_descriptors.cols << ")";

    std::vector<float> faceEmbedding;

    for (int i = 0 ; i < face_descriptors.rows ; ++i)
    {
        for (int j = 0 ; j < face_descriptors.cols ; ++j)
        {
            faceEmbedding.push_back(face_descriptors.at<float>(i,j));
        }
    }

    return faceEmbedding;
}

cv::Mat FaceExtractor::getFaceDescriptor(const cv::Mat& faceImage)
{
    qCDebug(DIGIKAM_FACEDB_LOG) << "faceImage channels: " << faceImage.channels();
    qCDebug(DIGIKAM_FACEDB_LOG) << "faceImage size: (" << faceImage.rows << ", " << faceImage.cols << ")\n";

    QElapsedTimer timer;
    timer.start();
/*
    cv::Mat alignedFace = faceImage;
*/
    cv::Mat alignedFace = d->preprocessor->preprocess(faceImage);

    qCDebug(DIGIKAM_FACEDB_LOG) << "Finish aligning face in " << timer.elapsed() << " ms";
    qCDebug(DIGIKAM_FACEDB_LOG) << "Start neural network";

    timer.start();
    cv::Mat face_descriptors;
    cv::Mat blob     = cv::dnn::blobFromImage(alignedFace, d->scaleFactor, d->imageSize, cv::Scalar(), true, false);

    d->net.setInput(blob);
    face_descriptors = d->net.forward();

    qCDebug(DIGIKAM_FACEDB_LOG) << "Finish computing face embedding in "
                                << timer.elapsed() << " ms";
/*
    cv::Mat blob = cv::dnn::blobFromImage(faceImage, 1.0 / 255, cv::Size(96, 96), cv::Scalar(0,0,0), false, true, CV_32F); // work for openface.nn4
    cv::Mat blob = cv::dnn::blobFromImage(faceImage, 1.0 / 255, cv::Size(224,224), cv::Scalar(0,0,0), false, true, CV_32F);
    net.setInput(blob);
    cv::Mat face_descriptors = net.forward();
*/

    //qDebug() << "faceEmbedded channels: " << face_descriptors.channels() << "type" << face_descriptors.type();
    //qDebug() << "faceEmbedded size: (" << face_descriptors.rows << ", " << face_descriptors.cols << ")\n";

    return face_descriptors;
}

} // namespace RecognitionTest
