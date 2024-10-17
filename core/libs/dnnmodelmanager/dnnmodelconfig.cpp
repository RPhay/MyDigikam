/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-10-13
 * Description : digiKam DNNModelConfig for generic OpenCV config
 *
 * SPDX-FileCopyrightText: 2024 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dnnmodelconfig.h"

// Qt includes

#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

/*
DNNModelConfig::DNNModelConfig(
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
*/

/*
const QString DNNModelConfig::getConfig()
{
    if (!modelLoaded)
    {
        loadModel();
        QString appPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                 QLatin1String("digikam/") + (downloadPath.split(QLatin1String("/"))[0]),
                                                 QStandardPaths::LocateDirectory);

        QString model = appPath + QLatin1Char('/') + fileName;

        return model;
    }

    return QLatin1String();
}
*/

bool DNNModelConfig::loadModel()
{
    if (checkFilename())
    {
        // do nothing.  Config don't have a cv::dnn::Net pointer

        return modelLoaded = true;  // set modelLoaded and return true
    }

    return false;
}

} // namespace Digikam
