/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-24
 * Description : MJPEG server manager
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_MJPEG_SERVER_MNGR_H
#define DIGIKAM_MJPEG_SERVER_MNGR_H

// Qt includes

#include <QObject>
#include <QMap>
#include <QList>
#include <QString>
#include <QUrl>

// Local includes

#include "mjpegserver.h"
#include "mjpegstreamsettings.h"

namespace DigikamGenericMjpegStreamPlugin
{

class MjpegServerMngr : public QObject
{
    Q_OBJECT

public:

    /**
     * Setup the list of items to share with the MJPEG server into a single album.
     */
    void setItemsList(const QString& aname, const QList<QUrl>& urls);

    /**
     * Return a flat list of items shared.
     */
    QList<QUrl> itemsList()                  const;

    /**
     * Setup the list of albums to share with MJPEG server.
     */
    void setCollectionMap(const MjpegServerMap&);

    /**
     * Return the current album map shared.
     */
    MjpegServerMap collectionMap()           const;

    /**
     * Setup the MJPEGstream settings.
     */
    void setSettings(const MjpegStreamSettings& set);

    /**
     * Return the MJPEG stream settings.
     */
    MjpegStreamSettings settings()           const;

    /**
     * Start the DLNA server and share the contents. Return true is all is on-line.
     */
    bool startMjpegServer();

    /**
     * Stop the DLNA server and clean-up.
     */
    void cleanUp();

    /**
     * Low level methods to save and load from xml data file.
     */
    bool save();
    bool load();

    /**
     * Wrapper to check if server configuration must be saved and restored between application sessions.
     */
    bool loadAtStartup();
    void saveAtShutdown();

    /**
     * Return true if server is running in background.
     */
    bool isRunning()                         const;

    /**
     * Return some stats about total albums and total items shared on the network.
     */
    int  albumsShared()                      const;
    int  itemsShared()                       const;

    /**
     * Config properties methods.
     */
    QString configGroupName()                 const;
    QString configStartServerOnStartupEntry() const;

    /**
     * Send a notification message if MediaServer have been started or not.
     */
    void mjpegServerNotification(bool started);

public:

    /**
     * This manager is a singleton. Use this method to control the MJPEG server instance.
     */
    static MjpegServerMngr* instance();

private:

    // Disable
    MjpegServerMngr();
    explicit MjpegServerMngr(QObject*) = delete;
    ~MjpegServerMngr() override;

private:

    friend class MjpegServerMngrCreator;

    class Private;
    Private* const d;
};

} // namespace DigikamGenericMjpegStreamPlugin

#endif // DIGIKAM_MJPEG_SERVER_MNGR_H
