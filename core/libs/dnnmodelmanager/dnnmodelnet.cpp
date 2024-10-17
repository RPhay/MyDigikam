/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-10-13
 * Description : digiKam DNNModelNet for generic OpenCV nets
 *
 * SPDX-FileCopyrightText: 2024 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dnnmodelnet.h"

// Qt includes

#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"
#include "dnnmodelconfig.h"

namespace Digikam
{

/*
DNNModelNet::DNNModelNet(
                         const QString&           _displayName,
                         const QString&           _fileName,
                         const DNNModelUsageList& _usage,
                         const QVersionNumber&    _minVersion,
                         const QString&           _downloadPath,
                         const QString&           _sha256,
                         const qint64&            _fileSize,
                         int                      _minUsableThreshold,
                         int                      _maxUsableThreshold,
                         DNNLoaderType            _loaderType,
                         const QString&           _configName,
                         const cv::Scalar&        _meanValToSubtract,
                         int                      _imageSize
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
*/

cv::dnn::Net& DNNModelNet::getNet()
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

bool DNNModelNet::loadModel()
{
    // Load the model from disk and create cv::dnn:Net instance

    QString configPath;

    // Verify filename

    if (checkFilename())
    {
        // Load config model if needed

        if (0 < info.configName.size())
        {
            const DNNModelConfig* const configModel = static_cast<DNNModelConfig*>(DNNModelManager::instance()->getModel(info.configName, info.usage[0]));
            configPath                              = configModel->getModelPath();
        }

        // Create the cv::dnn::Net instance with the config

        return callLoader(configPath);
    }

    return false;
}

bool DNNModelNet::callLoader(const QString& configPath)
{
    QString modelPath = getModelPath();

    if (0 < configPath.size())
    {

        net = cv::dnn::readNet(modelPath.toStdString(),
                               configPath.toStdString());

    }
    else
    {

        net = cv::dnn::readNet(modelPath.toStdString());

    }

    return true;
}

} // namespace Digikam
