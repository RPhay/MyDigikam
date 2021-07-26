/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-24
 * Description : a MJPEG Stream server to export items on the network.
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_MJPEG_SERVER_H
#define DIGIKAM_MJPEG_SERVER_H

// Qt includes

#include <QDebug>
#include <QImage>
#include <QObject>

namespace DigikamGenericMjpegStreamPlugin
{

/// A kind of map of albums with urls contents to share with MJPEG server.
typedef QMap<QString, QList<QUrl> > MjpegServerMap;

class MjpegServer : public QObject
{
    Q_OBJECT

public:

    /**
     * Create an instance of MJPEG server listening on 'address'
     * and 'port'. Default address is 'any' (an empty string), as server listen on all
     * network interfaces. You can pass a literal address as "localhost"
     * or an IPV4 based one as "192.168.1.1".
     */
    explicit MjpegServer(const QString& address = QString(),
                         int port = 8080,
                         QObject* const parent = nullptr);
    ~MjpegServer();

public:

    /**
     * Handle rate of frame per seconds dispatched to the network [1 - 30].
     * A low value reduce network bandwith use.
     * Default = 15 img/s.
     */
    bool setRate(int);
    int  rate() const;

    /**
     * Limit the number of clients connected to the server [1 - 30].
     * Default = 10 clients.
     */
    bool setMaxClients(int);
    int  maxClients() const;

    /**
     * Handle the list of clients IP address to ban from the stream server.
     */
    void setBlackList(const QStringList& lst);
    QStringList blackList() const;

    /**
     * Start and stop the server to listen on the network.
     * for a new client connection.
     */
    void start();
    void stop();

public Q_SLOTS:

    /**
     * Slot to push an update of JPEG frame to
     * the remote client connected on server.
     * Use a direct signal/slot connection to optimize
     * time latency.
     */
    void slotWriteFrame(const QByteArray& frame);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericMjpegStreamPlugin

#endif // DIGIKAM_MJPEG_SERVER_H
