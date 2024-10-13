/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-06-01
 * Description : Face recognition using deep learning
 *               The internal DNN library interface
 *
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2020-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024      by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dnnsfaceextractor.h"

// Qt includes

#include <QMutex>
#include <QString>
#include <QFileInfo>
#include <QMutexLocker>
#include <QElapsedTimer>
#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"
#include "recognitionpreprocessor.h"

namespace Digikam
{

class Q_DECL_HIDDEN DNNSFaceExtractor::Private
{
public:

    Private() = default;

    ~Private()
    {
        if (net)
        {
            // net->release();
        }
    }

public:

    int                             ref         = 1;

    cv::Ptr<cv::FaceRecognizerSF>   net         = nullptr;
    cv::Ptr<cv::FaceDetectorYN>     cv_model    = nullptr;
    QMutex                          mutex;
};

DNNSFaceExtractor::DNNSFaceExtractor()
    : DNNFaceExtractorBase(),
      d                   (new Private)
{
    // Virtual call in contructor: use dynamic binding.

    this->loadModels();
}

DNNSFaceExtractor::DNNSFaceExtractor(const DNNSFaceExtractor& other)
    : DNNFaceExtractorBase(),
      d                   (other.d)
{
    ++(d->ref);
}

DNNSFaceExtractor::~DNNSFaceExtractor()
{
    --(d->ref);

    if (d->ref == 0)
    {
        delete d;
    }
}

bool DNNSFaceExtractor::loadModels()
{
    QString appPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                             QLatin1String("digikam/facesengine"),
                                             QStandardPaths::LocateDirectory);

    QString model   = QLatin1String("face_recognition_sface_2021dec.onnx");
    QString nnmodel = appPath + QLatin1Char('/') + model;

    if (QFileInfo::exists(nnmodel))
    {
        try
        {
            qCDebug(DIGIKAM_FACEDB_LOG) << "Extractor model:" << nnmodel;

            // TODO set these based on installed capabilities

            int backend_id       = cv::dnn::DNN_BACKEND_DEFAULT;
            int target_id        = cv::dnn::DNN_TARGET_CPU;

            float conf_threshold = 0.3F;
            float nms_threshold  = 0.3F;
            int top_k            = 5000;

#ifdef Q_OS_WIN

            d->net = cv::FaceRecognizerSF::create(
                                                  nnmodel.toLocal8Bit().constData(),
                                                  "",
                                                  backend_id,
                                                  target_id
                                                 );

            model = QLatin1String("face_detection_yunet_2023mar.onnx");
            nnmodel = appPath + QLatin1Char('/') + model;
            d->cv_model = cv::FaceDetectorYN::create(
                                                     nnmodel.toStdString(),
                                                     "",
                                                     cv::Size(112, 112),
                                                     conf_threshold,
                                                     nms_threshold,
                                                     top_k,
                                                     backend_id,
                                                     target_id
                                                    );

#else

            // we need both YuNet for basic landmark detection and SFace for full feature extraction

            d->net = cv::FaceRecognizerSF::create(
                                                  nnmodel.toStdString(),
                                                  "",
                                                  backend_id,
                                                  target_id
                                                 );


            model       = QLatin1String("face_detection_yunet_2023mar.onnx");
            nnmodel     = appPath + QLatin1Char('/') + model;
            d->cv_model = cv::FaceDetectorYN::create(
                                                     nnmodel.toStdString(),
                                                     "",
                                                     cv::Size(112, 112),
                                                     conf_threshold,
                                                     nms_threshold,
                                                     top_k,
                                                     backend_id,
                                                     target_id
                                                    );

#endif

#if (OPENCV_VERSION == QT_VERSION_CHECK(4, 7, 0))

            d->net.enableWinograd(false);

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
        qCCritical(DIGIKAM_FACEDB_LOG) << "Cannot found faces engine DNN model" << model;
        qCCritical(DIGIKAM_FACEDB_LOG) << "Faces recognition feature cannot be used!";

        return false;
    }

    return true;
}

cv::Mat DNNSFaceExtractor::alignFace(const cv::Mat& inputImage) const
{
    cv::Mat alignedFace;
    d->net->alignCrop(inputImage, inputImage, alignedFace);

    return alignedFace;
}

cv::Mat DNNSFaceExtractor::getFaceEmbedding(const cv::Mat& faceImage)
{
    cv::Mat face_descriptors;
    cv::Mat paddedFace;
    cv::Mat alignedFace;

    QElapsedTimer timer;

    // start the timer for profiling

    timer.start();

    // resize the thumbnail if necessary to match SFace detection
    // SFace wants 112x112px images.  Resize so 112 is the smallest dimension

    if (std::min(faceImage.cols, faceImage.rows) > 112)
    {
        // Image should be resized.  YuNet image sizes are much more flexible than SSD and YOLO
        // so we just need to make sure no one bound exceeds the max. No padding needed

        float resizeFactor      = std::min(static_cast<float>(112) / static_cast<float>(faceImage.cols),
                                           static_cast<float>(112) / static_cast<float>(faceImage.rows));

        int newWidth            = (int)(resizeFactor * faceImage.cols);
        int newHeight           = (int)(resizeFactor * faceImage.rows);
        cv::resize(faceImage, paddedFace, cv::Size(newWidth, newHeight));
    }
    else
    {
        paddedFace = faceImage.clone();
    }

    // add a border so there is room to rotate the image during alignment

    cv::copyMakeBorder(paddedFace, paddedFace,
                       60, 60,
                       60, 60,
                       cv::BORDER_CONSTANT,
                       cv::Scalar(0, 0, 0));

    try 
    {
        QMutexLocker lock(&d->mutex);

        // redetect face using YuNet to get landmarks

        cv::Mat faceLandmark;

        d->cv_model->setInputSize(paddedFace.size());
        d->cv_model->detect(paddedFace, faceLandmark);

        if (0 < faceLandmark.rows)
        {
            // align and crop the face to standard size

            d->net->alignCrop(paddedFace, faceLandmark, alignedFace);

            qCDebug(DIGIKAM_FACEDB_LOG) << "Finish aligning face in " << timer.elapsed() << " ms";
            qCDebug(DIGIKAM_FACEDB_LOG) << "Start neural network";

            timer.start();

            d->net->feature(alignedFace, face_descriptors);
            normalize(face_descriptors, face_descriptors);
        }
    }
    catch (cv::Exception& e)
    {
        qCritical(DIGIKAM_FACEDB_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        // qCDebug(DIGIKAM_FACEDB_LOG) << e.what();
    }

    qCDebug(DIGIKAM_FACEDB_LOG) << "Finish computing face embedding in "
                                << timer.elapsed() << " ms";


    return face_descriptors;
}

} // namespace Digikam
