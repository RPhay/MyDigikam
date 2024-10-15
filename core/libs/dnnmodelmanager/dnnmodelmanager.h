/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-31
 * Description : digiKam global static QNetworkAccessManager
 *
 * SPDX-FileCopyrightText: 2022 by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QObject>
#include <QSettings>
// #include <QNetworkAccessManager>

// Local includes

#include "digikam_export.h"
#include "filesdownloader.h"
#include "dnnmodeldefinitions.h"

class DNNModelBase;
// #include "dnnmodelbase.h"

namespace Digikam
{

class DIGIKAM_EXPORT DNNModelManager : public QObject
{
    Q_OBJECT

public:

    /**
     * Global instance of internal network manager.
     * All accessor methods are thread-safe.
     */
    static DNNModelManager* instance();

    /**
     * used by the filesdownload to get a stream containing the files and information to download
     */
    const QList<DownloadInfo>& getDownloadInformation(DNNModelUsage usage);

    /**
     * retrieve a DNNModelBase pointer by name
     * This will load and create the model on first use
     * It will also find the best OpenCV Target and Backend for the model
     * based on computer capabilities
     */
    DNNModelBase* getModel(const QString& modelName, DNNModelUsage usage);

private:

    DNNModelManager();
    ~DNNModelManager();

    // Disable

    explicit DNNModelManager(QObject*) = delete;

private:
    const std::map<std::string, DNNLoaderType> str2loader
    {
        { "net",        DNNLoaderType::Net      },
        { "config",    DNNLoaderType::Config  },
        { "yunet",      DNNLoaderType::YuNet    },
        { "sface",      DNNLoaderType::SFace    }
    };

    /**
     * read the configuration file.  The configuration is a .csv hardcoded in dnnmodelmanager.cpp but
     * could be a csv, yaml, or json packed with digiKam or downloaded from the Internet
     */
    void loadConfig();
    void getSettings();     // called if no dnnmodelsettings.conf exists

private:

    class Private;
    Private* const d = nullptr;

    friend class DNNModelManagerCreator;
};

} // namespace Digikam
