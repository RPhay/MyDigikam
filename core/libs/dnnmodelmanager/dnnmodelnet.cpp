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

// Qt includes

#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"
#include "dnnmodelbase.h"
#include "dnnmodelnet.h"
#include "dnnmodelconfig.h"

namespace Digikam
{

DNNModelNet::DNNModelNet(
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

DNNModelNet::~DNNModelNet()
{
}

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
    // load the model from disk and create cv::dnn:Net instance

    QString configPath;

    // verify filename

    if (checkFilename())
    {

        // load config model if needed

        if (0 < configName.size())
        {
            DNNModelConfig* configModel = static_cast<DNNModelConfig*>(DNNModelManager::instance()->getModel(configName, usage[0]));
            configPath = configModel->getModelPath();
        }

        // create the cv::dnn::Net instance with the config

        return callLoader(configPath);
    }

    return false;
}

bool DNNModelNet::callLoader(const QString& configPath)
{

    QString appPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                    QLatin1String("digikam/") + (downloadPath.split(QLatin1String("/"))[0]),
                                    QStandardPaths::LocateDirectory);

    QString modelPath = appPath + QLatin1Char('/') + fileName;

    if (0 < configPath.size())
    {

#ifdef Q_OS_WIN
            net = cv::dnn::readNet(modelPath.toLocal8Bit().constData(),
                                   configPath.toLocal8Bit().constData());

#else

            net = cv::dnn::readNet(modelPath.toStdString(),
                                   configPath.toStdString());

#endif
    }
    else
    {

#ifdef Q_OS_WIN

            net = cv::dnn::readNet(modelPath.toLocal8Bit().constData());

#else

            net = cv::dnn::readNet(modelPath.toStdString());

#endif

    }

    return true;
}

} // namespace Digikam

// #include "moc_DNNModelBase.cpp"
