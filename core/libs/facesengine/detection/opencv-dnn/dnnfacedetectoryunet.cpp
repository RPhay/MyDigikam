/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-08-08
 * Description : Derived class to perform YuNet neural network inference
 *               for face detection. Credit: Ayoosh Kathuria (for YuNet blog post),
 *               sthanhng (for example of face detection with YuNet).
 *               More information with YuNetv3:
 *               https://towardsdatascience.com/yolo-v3-object-detection-53fb7d3bfe6b
 *               sthanhng github on face detection with Yolov3:
 *               https://github.com/sthanhng/yoloface
 *
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2020-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dnnfacedetectoryunet.h"

// Qt includes

#include <QList>
#include <QRect>
#include <QString>
#include <QFileInfo>
#include <QMutexLocker>
#include <QElapsedTimer>
#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"

namespace Digikam
{

DNNFaceDetectorYuNet::DNNFaceDetectorYuNet()
    : DNNFaceDetectorBase(1.0F / 255.0F,
                          cv::Scalar(0.0, 0.0, 0.0),
                          cv::Size(1024, 1024))
{
    qCDebug(DIGIKAM_FACESENGINE_LOG) << "Creating new instance of DNNFaceDetectorYuNet";

    loadModels();
}

bool DNNFaceDetectorYuNet::loadModels()
{
    QString appPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                             QLatin1String("digikam/facesengine"),
                                             QStandardPaths::LocateDirectory);
    float conf_threshold = 0.3f;
    float nms_threshold = 0.3f;
    int top_k = 5000;
    int backend_id = 0;
    int target_id = 0;

    // TODO: detect backends and targets.  Pick the best one.

    // net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
    // net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);


     QString model   = QLatin1String("face_detection_yunet_2023mar.onnx");

     QString nnmodel = appPath + QLatin1Char('/') + model;

    if (QFileInfo::exists(nnmodel))
    {
        try
        {
            qCDebug(DIGIKAM_FACEDB_LOG) << "YuNet model:" << model;

#ifdef Q_OS_WIN

            cv_model = cv::FaceDetectorYN::create(nnmodel.toLocal8Bit(), "", inputImageSize, conf_threshold, nms_threshold, top_k, backend_id, target_id);

#else

            // cv_model = cv::FaceDetectorYN::create("/Users/michmill/Downloads/face_detection_yunet_2023mar.onnx", "", inputImageSize, conf_threshold, nms_threshold, top_k, backend_id, target_id);
            cv_model = cv::FaceDetectorYN::create(nnmodel.toStdString(), "", inputImageSize, conf_threshold, nms_threshold, top_k, backend_id, target_id);

#endif

#if (OPENCV_VERSION == QT_VERSION_CHECK(4, 7, 0))

            net.enableWinograd(false);

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
        qCCritical(DIGIKAM_FACEDB_LOG) << "Cannot find faces engine DNN model" << model;
        qCCritical(DIGIKAM_FACEDB_LOG) << "Faces detection feature cannot be used!";

        return false;
    }

    return true;
}
cv::Mat DNNFaceDetectorYuNet::callModel(const cv::Mat& inputImage)
{
    QElapsedTimer timer;

    // lock the model for single threading
    std::lock_guard<std::mutex> lock(lockModel);

    // set up the detector with new params
    cv_model->setInputSize(inputImage.size());
    cv_model->setScoreThreshold(confidenceThreshold);
    cv_model->setNMSThreshold(nmsThreshold);

    // detect faces
    cv::Mat faces;
    
    timer.start();
    
    cv_model->detect(inputImage, faces);

    qCDebug(DIGIKAM_FACESENGINE_LOG) << "YuNet detected" << faces.rows << "faces in" << timer.elapsed() << "ms";

    return faces;
}

void DNNFaceDetectorYuNet::detectFaces(const cv::Mat& inputImage,
                                      const cv::Size& paddedSize,
                                      std::vector<cv::Rect>& detectedBboxes)
{
    std::vector<float> confidences;

    // safety check
    if (inputImage.empty())
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Invalid image given, not detecting faces.";
        return;
    }

    // all calls to the model need to be in this method
    cv::Mat faces = callModel(inputImage);
    
    // process faces found
    if ( faces.rows > 0)
    {

        // loop through the faces found
        for (int i = 0 ; i < faces.rows ; ++i)
        {
            double confidence = faces.at<float>(i, 14);

            // add the confidence to the result list
            confidences.push_back(confidence);

            // create the rect of the face
            int X = static_cast<int>(faces.at<float>(i, 0));
            int Y = static_cast<int>(faces.at<float>(i, 1));
            int width   = static_cast<int>(faces.at<float>(i, 2));
            int height  = static_cast<int>(faces.at<float>(i, 3));

            // add the rect to result list
            detectedBboxes.push_back(cv::Rect(X, Y, width, height));
        }
    }
}

} // namespace Digikam
