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

    for (auto model : keys)
    {
        if (d->modelMap[model]->usage.contains(usage))
        {
            d->downloadInfo << d->modelMap[model]->getDownloadInformation();
        }
    }

    return d->downloadInfo;
}

DNNModelBase* DNNModelManager::getModel(const QString& modelName, DNNModelUsage usage) const
{
    Q_UNUSED(usage);       // for future reference

    return d->modelMap[modelName.toLower()];
}

// --------------- private ---------------------

void DNNModelManager::loadConfig()
{
    // get the current application name

    QString appName = qApp->applicationName().toLower();

    // load the group from the config file

    const auto groups = d->settings->childGroups();

    for (const auto& model : groups)
    {

        // load the keys and values for this group

        d->settings->beginGroup(model);

        // check if model is used with this application

        if (d->settings->value(QLatin1String("Application")).toString().toLower().contains(appName))
        {
            // Create usage

            DNNModelUsageList usage;
            QString usageStr =  d->settings->value(QLatin1String("Usage")).toString();

            if (usageStr.contains(QLatin1String("face_detection")))
            {
                usage.append(DNNModelUsage::FaceDetection); 
            }

            if (usageStr.contains(QLatin1String("face_recognition")))
            {
                usage.append(DNNModelUsage::FaceRecognition);
            }

            if (usageStr.contains(QLatin1String("redeye_detection")))
            {
                usage.append(DNNModelUsage::RedeyeDetection);
            }

            if (usageStr.contains(QLatin1String("object_detection")))
            {
                usage.append(DNNModelUsage::ObjectDetection);
            }

            if (usageStr.contains(QLatin1String("aesthetic")))
            {
                usage.append(DNNModelUsage::Aesthetic);
            }

            // Create version

            QVersionNumber version;
            QString strVersion       = d->settings->value(QLatin1String("MinVersion")).toString();
            QStringList versionParts = d->settings->value(QLatin1String("MinVersion")).toString().toLower().split(QLatin1String("."));

            if (3 == versionParts.size())
            {
                version = QVersionNumber(versionParts[0].toInt(), versionParts[1].toInt(), versionParts[2].toInt());
            }

            // loader type

            QString loadertypeStr    = d->settings->value(QLatin1String("LoaderType")).toString();
            DNNLoaderType loaderType = str2loader.at(d->settings->value(QLatin1String("LoaderType")).toString().toLower().toUtf8().data());

            // create version

            cv::Scalar meanValToSubtract;
            QString mvtsStr       = d->settings->value(QLatin1String("MeanValueToSubtract")).toString().toLower();
            QStringList mvtsParts = d->settings->value(QLatin1String("MeanValueToSubtract")).toString().toLower().split(QLatin1String(","));

            if (3 == mvtsParts.size())
            {
                meanValToSubtract = cv::Scalar(mvtsParts[0].toFloat(), mvtsParts[1].toFloat(), mvtsParts[2].toFloat());
            }

            // create the model

            DNNModelBase* modelPtr = nullptr;

            switch (loaderType)
            {
                case DNNLoaderType::Net:
                {
                    modelPtr = new DNNModelNet(
                                               d->settings->value(QString::fromUtf8("DisplayName")).toString(),
                                               d->settings->value(QString::fromUtf8("FileName")).toString(),
                                               usage,
                                               version,
                                               d->settings->value(QString::fromUtf8("DownloadPath")).toString(),
                                               d->settings->value(QString::fromUtf8("SHA256")).toString(),
                                               d->settings->value(QString::fromUtf8("FileSize")).toInt(),
                                               d->settings->value(QString::fromUtf8("MinUsageThreshold")).toInt(),
                                               d->settings->value(QString::fromUtf8("MaxUsageThreshold")).toInt(),
                                               loaderType,
                                               d->settings->value(QString::fromUtf8("ConfigName")).toString(),
                                               meanValToSubtract,
                                               d->settings->value(QString::fromUtf8("ImageSize")).toInt()
                                              );
                    break;
                }

                case DNNLoaderType::Config:
                {
                    modelPtr = new DNNModelConfig(
                                                  d->settings->value(QString::fromUtf8("DisplayName")).toString(),
                                                  d->settings->value(QString::fromUtf8("FileName")).toString(),
                                                  usage,
                                                  version,
                                                  d->settings->value(QString::fromUtf8("DownloadPath")).toString(),
                                                  d->settings->value(QString::fromUtf8("SHA256")).toString(),
                                                  d->settings->value(QString::fromUtf8("FileSize")).toInt(),
                                                  d->settings->value(QString::fromUtf8("MinUsageThreshold")).toInt(),
                                                  d->settings->value(QString::fromUtf8("MaxUsageThreshold")).toInt(),
                                                  loaderType,
                                                  d->settings->value(QString::fromUtf8("ConfigName")).toString(),
                                                  meanValToSubtract,
                                                  d->settings->value(QString::fromUtf8("ImageSize")).toInt()
                                            );
                    break;
                }

                case DNNLoaderType::YuNet:
                {
                    modelPtr = new DNNModelYuNet(
                                                 d->settings->value(QString::fromUtf8("DisplayName")).toString(),
                                                 d->settings->value(QString::fromUtf8("FileName")).toString(),
                                                 usage,
                                                 version,
                                                 d->settings->value(QString::fromUtf8("DownloadPath")).toString(),
                                                 d->settings->value(QString::fromUtf8("SHA256")).toString(),
                                                 d->settings->value(QString::fromUtf8("FileSize")).toInt(),
                                                 d->settings->value(QString::fromUtf8("MinUsageThreshold")).toInt(),
                                                 d->settings->value(QString::fromUtf8("MaxUsageThreshold")).toInt(),
                                                 loaderType,
                                                 d->settings->value(QString::fromUtf8("ConfigName")).toString(),
                                                 meanValToSubtract,
                                                 d->settings->value(QString::fromUtf8("ImageSize")).toInt()
                                                );
                    break;
                }

                case DNNLoaderType::SFace:
                {
                    modelPtr = new DNNModelSFace(
                                                 d->settings->value(QString::fromUtf8("DisplayName")).toString(),
                                                 d->settings->value(QString::fromUtf8("FileName")).toString(),
                                                 usage,
                                                 version,
                                                 d->settings->value(QString::fromUtf8("DownloadPath")).toString(),
                                                 d->settings->value(QString::fromUtf8("SHA256")).toString(),
                                                 d->settings->value(QString::fromUtf8("FileSize")).toInt(),
                                                 d->settings->value(QString::fromUtf8("MinUsageThreshold")).toInt(),
                                                 d->settings->value(QString::fromUtf8("MaxUsageThreshold")).toInt(),
                                                 loaderType,
                                                 d->settings->value(QString::fromUtf8("ConfigName")).toString(),
                                                 meanValToSubtract,
                                                 d->settings->value(QString::fromUtf8("ImageSize")).toInt()
                                                );
                    break;
                }
            }

            // add the model to map

            d->modelMap.insert(model.toLower(), modelPtr);
        }

        // done with this group

        d->settings->endGroup();
    }
}

void DNNModelManager::getSettings()
{
    if (!d->settings)
    {
        // check potential download location
        // TODO: Maybe download the config file from the web

        // get from bundle

        QString appPath = QStandardPaths::locate(QStandardPaths::AppLocalDataLocation,
                                                 QLatin1String("digikam/dnnmodels/dnnmodels.conf"),
                                                 QStandardPaths::LocateFile);
        d->settings = new QSettings(appPath, QSettings::IniFormat);
    }
}

} // namespace Digikam

#include "moc_dnnmodelmanager.cpp"
