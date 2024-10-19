/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-10-13
 * Description : digiKam DNN Model Manager
 *
 * SPDX-FileCopyrightText: 2024 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dnnmodelmanager.h"

// Qt includes

#include <QStandardPaths>
#include <QVersionNumber>
#include <QApplication>

// Local includes

#include "digikam_debug.h"
#include "dnnmodelnet.h"
#include "dnnmodelconfig.h"
#include "dnnmodelyunet.h"
#include "dnnmodelsface.h"
#include "dnnmodelinfocontainer.h"

namespace Digikam
{

class Q_DECL_HIDDEN DNNModelManager::Private
{
public:

    Private() = default;

public:

    QMap<QString, DNNModelBase*>    modelMap;
    QSettings*                      settings        = nullptr;
    QList<DownloadInfo>             downloadInfo;
};

// -----------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN DNNModelManagerCreator
{
public:

    DNNModelManager object;
};

Q_GLOBAL_STATIC(DNNModelManagerCreator, DNNModelManagerCreator)

// -----------------------------------------------------------------------------------------------

DNNModelManager::DNNModelManager()
    : QObject(),
      d      (new Private)
{
    if (0 == d->modelMap.size())
    {
        getSettings();
        loadConfig();
    }
}

DNNModelManager::~DNNModelManager()
{
    delete d;
}

DNNModelManager* DNNModelManager::instance()
{
    return &DNNModelManagerCreator->object;
}

const QList<DownloadInfo>& DNNModelManager::getDownloadInformation(DNNModelUsage usage)
{
    d->downloadInfo.clear();

    const auto keys = d->modelMap.keys();

    for (auto& model : keys)
    {
        if (d->modelMap[model]->info.usage.contains(usage))
        {
            d->downloadInfo << d->modelMap[model]->getDownloadInformation();
        }
    }

    return d->downloadInfo;
}

DNNModelBase* DNNModelManager::getModel(const QString& modelName, DNNModelUsage usage) const
{
    Q_UNUSED(usage);       // For future reference

    if (d->modelMap.contains(modelName.toLower()))
    {
        return d->modelMap[modelName.toLower()];
    }
    else
    {
        qCCritical(DIGIKAM_DNNMODELMNGR_LOG) << "Cannot found model:" << modelName;

        return nullptr;
    }
}

// --------------- private ---------------------

void DNNModelManager::loadConfig()
{
    // Get the current application name

    QString appName   = qApp->applicationName().toLower();

    // Load the group from the config file

    const auto groups = d->settings->childGroups();

    for (const auto& modelName : groups)
    {
        DNNModelInfoContainer info;

        // Load the keys and values for this group

        d->settings->beginGroup(modelName);

        // Check if model is used with this application

        if (d->settings->value(QLatin1String("Application")).toString().toLower().contains(appName))
        {
            // Set basic values

            info.displayName        = d->settings->value(QString::fromUtf8("DisplayName")).toString();
            info.fileName           = d->settings->value(QString::fromUtf8("FileName")).toString();
            info.downloadPath       = d->settings->value(QString::fromUtf8("DownloadPath")).toString();
            info.sha256             = d->settings->value(QString::fromUtf8("SHA256")).toString();
            info.fileSize           = d->settings->value(QString::fromUtf8("FileSize")).toInt();
            info.baseThreshold      = d->settings->value(QString::fromUtf8("BaseThreshold")).toInt();
            info.minUsableThreshold = d->settings->value(QString::fromUtf8("MinUsableThreshold")).toInt();
            info.minUsableThreshold = d->settings->value(QString::fromUtf8("MaxUsableThreshold")).toInt();
            info.classList          = d->settings->value(QString::fromUtf8("ClassList")).toString();
            info.configName         = d->settings->value(QString::fromUtf8("ConfigName")).toString();
            info.imageSize          = d->settings->value(QString::fromUtf8("ImageSize")).toInt();

            // Create usage

            QString usageStr =  d->settings->value(QLatin1String("Usage")).toString();

            if (usageStr.contains(QLatin1String("face_detection")))
            {
                info.usage.append(DNNModelUsage::DNNUsageFaceDetection); 
            }

            if (usageStr.contains(QLatin1String("face_recognition")))
            {
                info.usage.append(DNNModelUsage::DNNUsageFaceRecognition);
            }

            if (usageStr.contains(QLatin1String("redeye_detection")))
            {
                info.usage.append(DNNModelUsage::DNNUsageRedeyeDetection);
            }

            if (usageStr.contains(QLatin1String("object_detection")))
            {
                info.usage.append(DNNModelUsage::DNNUsageObjectDetection);
            }

            if (usageStr.contains(QLatin1String("aesthetics")))
            {
                info.usage.append(DNNModelUsage::DNNUsageAesthetics);
            }

            // Create version

            QStringList versionParts = d->settings->value(QLatin1String("MinVersion")).toString().toLower().split(QLatin1String("."));

            if (3 == versionParts.size())
            {
                info.minVersion = QVersionNumber(versionParts[0].toInt(), versionParts[1].toInt(), versionParts[2].toInt());
            }

            // Loader type

            info.loaderType          = str2loader.at(d->settings->value(QLatin1String("LoaderType")).toString().toLower().toUtf8().data());

            // Create version

            QStringList mvtsParts    = d->settings->value(QLatin1String("MeanValueToSubtract")).toStringList();

            if (3 == mvtsParts.size())
            {
                info.meanValToSubtract = cv::Scalar(mvtsParts[0].toFloat(), mvtsParts[1].toFloat(), mvtsParts[2].toFloat());
            }

            // create the model

            DNNModelBase* modelPtr = nullptr;

            switch (info.loaderType)
            {
                case DNNLoaderType::DNNLoaderNet:
                {
                    modelPtr = new DNNModelNet(info);
                    break;
                }

                case DNNLoaderType::DNNLoaderConfig:
                {
                    modelPtr = new DNNModelConfig(info);
                    break;
                }

                case DNNLoaderType::DNNLoaderYuNet:
                {
                    modelPtr = new DNNModelYuNet(info);
                    break;
                }

                case DNNLoaderType::DNNLoaderSFace:
                {
                    modelPtr = new DNNModelSFace(info);
                    break;
                }
            }

            // Add the model to map

            if (modelPtr)
            {
                d->modelMap.insert(modelName.toLower(), modelPtr);
            }
        }

        // Done with this group

        d->settings->endGroup();
    }
}

void DNNModelManager::getSettings()
{
    if (!d->settings)
    {
        // Check potential download location
        // TODO: Maybe download the config file from the web

        // Get from bundle

        QString appPath = QStandardPaths::locate(QStandardPaths::AppDataLocation,
                                                 QLatin1String("digikam/dnnmodels/dnnmodels.conf"),
                                                 QStandardPaths::LocateFile);

        if (!appPath.isEmpty())
        {
            d->settings = new QSettings(appPath, QSettings::IniFormat);
        }
        else
        {
            qCCritical(DIGIKAM_DNNMODELMNGR_LOG) << "Cannot find configuration file dnnmodels.conf";
        }
    }
}

} // namespace Digikam

#include "moc_dnnmodelmanager.cpp"
