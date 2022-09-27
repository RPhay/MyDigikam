/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-01-24
 * Description : Web Service settings container.
 *
 * SPDX-FileCopyrightText: 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2018      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_WS_SETTINGS_H
#define DIGIKAM_WS_SETTINGS_H

// Qt includes

#include <QObject>
#include <QtGlobal>
#include <QList>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QMap>
#include <QSettings>

// Local includes

#include "digikam_export.h"
#include "o0settingsstore.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT WSSettings : public QObject
{
    Q_OBJECT

public:

    /// Images selection mode
    enum Selection
    {
        EXPORT = 0,
        IMPORT
    };

    enum WebService
    {
        FLICKR = 0,
        DROPBOX,
        IMGUR,
        FACEBOOK,
        SMUGMUG,
        GDRIVE,
        GPHOTO
    };

    enum ImageFormat
    {
        JPEG = 0,
        PNG
    };

public:

    explicit WSSettings(QObject* const parent = nullptr);
    ~WSSettings() override;

    ///@{
    /// Read and write settings in config file between sessions.
    void  readSettings(KConfigGroup& group);
    void  writeSettings(KConfigGroup& group);
    ///@}

    QString format() const;

    ///@{
    /// Helper methods to fill settings from GUI.
    static QMap<WebService,  QString> webServiceNames();
    static QMap<ImageFormat, QString> imageFormatNames();
    ///@}

    /// Helper method to list all user accounts (of all web service) that user logged in before.
    QStringList allUserNames(const QString& serviceName);

public:

    Selection           selMode;             ///< Items selection mode

    QList<QUrl>         inputImages;         ///< Selected items to upload.

    bool                addFileProperties;
    bool                imagesChangeProp;

    bool                removeMetadata;

    int                 imageCompression;

    qint64              attLimitInMbytes;

    WebService          webService;

    QString             userName;

    QSettings*          oauthSettings;
    O0SettingsStore*    oauthSettingsStore;

    QString             currentAlbumId;     ///< Selected album to upload to

    int                 imageSize;

    ImageFormat         imageFormat;

    QMap<QUrl, QUrl>    itemsList;          ///< Map of original item and attached item (can be resized).
};

} // namespace Digikam

#endif // DIGIKAM_WS_SETTINGS_H
