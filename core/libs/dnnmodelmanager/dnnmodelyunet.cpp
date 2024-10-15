/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-10-13
 * Description : digiKam DNNModelYuNet specific for create FaceDetectorYN instances
 *
 * SPDX-FileCopyrightText: 2024 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dnnmodelyunet.h"

// Qt includes

#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"
#include "dnnmodelconfig.h"

namespace Digikam
{

DNNModelYuNet::DNNModelYuNet(
                             const QString&                 _displayName,
                             const QString&                 _fileName,
                             const DNNModelUsageList&       _usage,
                             const QVersionNumber&          _minVersion,
                             const QString&                 _downloadPath,
                             const QString&                 _sha256,
                             const qint64&                  _fileSize,
                             int                            _minUsableThreshold,
                             int                            _maxUsableThreshold,
                             DNNLoaderType                  _loaderType,
                             const QString&                 _configName,
                             const cv::Scalar&              _meanValToSubtract,
                             int                            _imageSize
                            )
    : DNNModelBase(
                    _displayName,
                    _fileName,
                    _usage,
                    _minVersion,
                    _downloadPath,
                    _sha256,
                    _fileSize,
                    _minUsableThreshold,
                    _maxUsableThreshold,
                    _loaderType,
                    _configName,
                    _meanValToSubtract,
                    _imageSize
                   )
{
}

cv::Ptr<cv::FaceDetectorYN>& DNNModelYuNet::getNet()
{
    if (!modelLoaded)
    {
        QMutexLocker lock(&mutex);

        if (loadModel())
        {
            modelLoaded = true;
        }
    }

    return net;
}

bool DNNModelYuNet::loadModel()
{
    // load the model from disk and create cv::dnn:Net instance

    // verify filename

    if (checkFilename())
    {
        // create the cv::dnn::Net instance with the config

        return callLoader();

    }

    return false;
}

bool DNNModelYuNet::callLoader()
{
    float conf_threshold = 0.3F;
    float nms_threshold  = 0.3F;
    int top_k            = 5000;
    int backend_id       = cv::dnn::DNN_BACKEND_DEFAULT;
    int target_id        = cv::dnn::DNN_TARGET_CPU;

    QString modelPath    = getModelPath();

#ifdef Q_OS_WIN

    net = cv::FaceDetectorYN::create(
                                     modelPath.toLocal8Bit().constData(),
                                     "",
                                     inputImageSize,
                                     conf_threshold,
                                     nms_threshold,
                                     top_k,
                                     backend_id,
                                     target_id
                                    );
#else

    net = cv::FaceDetectorYN::create(
                                     modelPath.toStdString(),
                                     "",
                                     cv::Size(imageSize, imageSize),
                                     conf_threshold,
                                     nms_threshold,
                                     top_k,
                                     backend_id,
                                     target_id
                                    );
#endif

    return true;
}

} // namespace Digikam
