/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-08-08
 * Description : Derived class to perform YuNet neural network inference
 *               for face detection. Credit: Ayoosh Kathuria (for YuNet blog post),
 *               sthanhng (for example of face detection with YuNet).
 *               More information with YuNetv3:
 *               https://github.com/opencv/opencv_zoo/tree/main/models/face_detection_yunet
 *
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2020-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024      by Michael Miller <michael underscore miller at msn dot com>
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

const std::map<std::string, int> str2backend
{
    { "default", cv::dnn::DNN_BACKEND_DEFAULT          },
    { "halide",  cv::dnn::DNN_BACKEND_HALIDE           },
    { "ie",      cv::dnn::DNN_BACKEND_INFERENCE_ENGINE },
    { "opencv",  cv::dnn::DNN_BACKEND_OPENCV           },
    { "vkcom",   cv::dnn::DNN_BACKEND_VKCOM            }
};

const std::map<std::string, int> str2target
{
    { "cpu",         cv::dnn::DNN_TARGET_CPU           },
    { "opencl",      cv::dnn::DNN_TARGET_OPENCL        },
    { "myriad",      cv::dnn::DNN_TARGET_MYRIAD        },
    { "vulkan",      cv::dnn::DNN_TARGET_VULKAN        },
    { "opencl_fp16", cv::dnn::DNN_TARGET_OPENCL_FP16   }
};

const std::map<FaceScanSettings::FaceDetectionSize, int> faceenum2size
{
    { FaceScanSettings::FaceDetectionSize::ExtraLarge, 420   },
    { FaceScanSettings::FaceDetectionSize::Large,      620   },
    { FaceScanSettings::FaceDetectionSize::Medium,     800   },
    { FaceScanSettings::FaceDetectionSize::Small,      1200  },
    { FaceScanSettings::FaceDetectionSize::ExtraSmall, 2000  }
};

DNNFaceDetectorYuNet::DNNFaceDetectorYuNet()
    : DNNFaceDetectorBase(1.0F / 255.0F,
                          cv::Scalar(0.0, 0.0, 0.0),
                          cv::Size(800, 800))
{
    qCDebug(DIGIKAM_FACESENGINE_LOG) << "Creating new instance of DNNFaceDetectorYuNet";

    loadModels();
}

DNNFaceDetectorYuNet::~DNNFaceDetectorYuNet()
{
}

bool DNNFaceDetectorYuNet::loadModels()
{
    QString appPath      = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                  QLatin1String("digikam/facesengine"),
                                                  QStandardPaths::LocateDirectory);
    float conf_threshold = 0.3F;
    float nms_threshold  = 0.3F;
    int top_k            = 5000;
    int backend_id       = cv::dnn::DNN_BACKEND_DEFAULT;
    int target_id        = cv::dnn::DNN_TARGET_CPU;

    // TODO: detect backends and targets. Pick the best one.

    QString cvBackend    = QString::fromLocal8Bit(qgetenv("DIGIKAM_YUNET_BACKEND"));
    QString cvTarget     = QString::fromLocal8Bit(qgetenv("DIGIKAM_YUNET_TARGET"));

    if (cvBackend.length() > 0)
    {
        try
        {
            backend_id = str2backend.at(cvBackend.toLower().toUtf8().data());
            qCDebug(DIGIKAM_FACESENGINE_LOG) << "YuNet using OpenCV backend:" << cvBackend;
        }
        catch (...)
        {
            qCDebug(DIGIKAM_FACESENGINE_LOG) << "Invalid YuNet OpenCV backend:" << cvBackend;
        }
    }

    if (cvTarget.length() > 0)
    {
        try
        {
            target_id = str2target.at(cvTarget.toLower().toUtf8().data());
            qCDebug(DIGIKAM_FACESENGINE_LOG) << "YuNet using OpenCV target:" << cvTarget;
        }
        catch (...)
        {
            qCDebug(DIGIKAM_FACESENGINE_LOG) << "Invalid YuNet OpenCV target:" << cvTarget;
        }
    }

    QString model   = QLatin1String("face_detection_yunet_2023mar.onnx");

    QString nnmodel = appPath + QLatin1Char('/') + model;

    if (QFileInfo::exists(nnmodel))
    {
        try
        {
            qCDebug(DIGIKAM_FACESENGINE_LOG) << "YuNet model:" << model;

            cv_model = cv::FaceDetectorYN::create(
                                                  nnmodel.toStdString(),
                                                  "",
                                                  inputImageSize,
                                                  conf_threshold,
                                                  nms_threshold,
                                                  top_k,
                                                  backend_id,
                                                  target_id
                                                 );
        }
        catch (cv::Exception& e)
        {
            qCWarning(DIGIKAM_FACESENGINE_LOG) << "cv::Exception:" << e.what();

            return false;
        }
        catch (...)
        {
           qCWarning(DIGIKAM_FACESENGINE_LOG) << "Default exception from OpenCV";

           return false;
        }
    }
    else
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "Cannot find faces engine DNN model" << model;
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "Faces detection feature cannot be used!";

        return false;
    }

    return true;
}

cv::Mat DNNFaceDetectorYuNet::callModel(const cv::Mat& inputImage)
{
    QElapsedTimer timer;
    cv::Mat faces;

    qCDebug(DIGIKAM_FACESENGINE_LOG) << "starting YuNet face detection";

    // Lock the model for single threading

    QMutexLocker lock(&lockModel);

    try
    {
        if (!cv_model.empty())
        {
            // Start the timer so we know how long we're locking for

            timer.start();

            // Set up the detector with new params

            cv_model->setInputSize(inputImage.size());
            cv_model->setScoreThreshold(confidenceThreshold);
            cv_model->setNMSThreshold(nmsThreshold);

            // Detect faces

            cv_model->detect(inputImage, faces);

            qCDebug(DIGIKAM_FACESENGINE_LOG) << "YuNet detected" << faces.rows << "faces in" << timer.elapsed() << "ms";
        }
    }

    catch (const std::exception& ex)
    {
        // ...
    }
    catch (const std::string& ex)
    {
        // ...
    }
    catch (...)
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "Face detection encountered a critical error. Reloading model...";
        loadModels();
    }

    return faces;
}

void DNNFaceDetectorYuNet::detectFaces(const cv::Mat& inputImage,
                                       const cv::Size& paddedSize,
                                       std::vector<cv::Rect>& detectedBboxes)
{
    Q_UNUSED(paddedSize);

    std::vector<float> confidences;

    // Safety check

    if (inputImage.empty())
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Invalid image given to YuNet, not detecting faces.";
        return;
    }

    // All calls to the model need to be in this method

    cv::Mat faces = callModel(inputImage);

    // Process faces found

    if ( faces.rows > 0)
    {
        // Loop through the faces found

        for (int i = 0 ; i < faces.rows ; ++i)
        {
            double confidence = faces.at<float>(i, 14);

            // Add the confidence to the result list

            confidences.push_back(confidence);

            // Create the rect of the face

            int X       = static_cast<int>(faces.at<float>(i, 0));
            int Y       = static_cast<int>(faces.at<float>(i, 1));
            int width   = static_cast<int>(faces.at<float>(i, 2));
            int height  = static_cast<int>(faces.at<float>(i, 3));

            // Add the rect to result list

            detectedBboxes.push_back(cv::Rect(X, Y, width, height));
        }
    }
}

void DNNFaceDetectorYuNet::setFaceDetectionSize(FaceScanSettings::FaceDetectionSize faceSize)
{
    try
    {
        inputImageSize = cv::Size(
                                  faceenum2size.at(faceSize),
                                  faceenum2size.at(faceSize)
                                 );
    }
    catch (const std::exception& e)
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "YuNet face size error:" << e.what() << '\n';
    }
}

} // namespace Digikam
