/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-10-13
 * Description : digiKam DNNModelSFace specific for create FaceRecognizerSF instances
 *
 * SPDX-FileCopyrightText: 2024 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dnnmodelsface.h"

// Qt includes

#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"
#include "dnnmodelconfig.h"

namespace Digikam
{

DNNModelSFace::DNNModelSFace(
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

cv::Ptr<cv::FaceRecognizerSF>& DNNModelSFace::getNet()
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

bool DNNModelSFace::loadModel()
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

bool DNNModelSFace::callLoader()
{
    int backend_id    = cv::dnn::DNN_BACKEND_DEFAULT;
    int target_id     = cv::dnn::DNN_TARGET_CPU;

    QString modelPath = getModelPath();

#ifdef Q_OS_WIN

    net = cv::FaceRecognizerSF::create(
                                       modelPath.toLocal8Bit().constData(),
                                       "",
                                       backend_id,
                                       target_id
                                      );

#else

    net = cv::FaceRecognizerSF::create(
                                       modelPath.toStdString(),
                                       "",
                                       backend_id,
                                       target_id
                                      );

#endif

    return true;
}

} // namespace Digikam
