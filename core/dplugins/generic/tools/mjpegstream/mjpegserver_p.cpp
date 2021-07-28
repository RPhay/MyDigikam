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

#include "mjpegserver_p.h"

// Must be placed in first to not break Windows Compilation with WinSock API

#ifdef Q_OS_WIN
#   ifdef WIN32_LEAN_AND_MEAN        // krazy:exclude=cpp
#       undef WIN32_LEAN_AND_MEAN
#   endif
#endif

// C ANSI includes

#ifndef Q_OS_WIN
#   include <sys/socket.h>
#else
#   include <windows.h>
#   define MSG_NOSIGNAL 0
#endif

// Qt includes

#include <QByteArray>
#include <QString>
#include <QBuffer>
#include <QtConcurrent>              // krazy:exclude=includes

// Local includes

#include "digikam_debug.h"
#include "actionthreadbase.h"

namespace DigikamGenericMjpegStreamPlugin
{

MjpegServer::Private::Private(QObject* const parent)
    : QObject(parent),
      server (nullptr),
      rate   (15),
      delay  (40000)
{
}

MjpegServer::Private::~Private()
{
}

void MjpegServer::Private::setMaxClients(int max)
{
    if (isOpened())
    {
        server->setMaxPendingConnections(max);
        qCDebug(DIGIKAM_GENERAL_LOG) << "MJPEG server max clients:" << maxClients();
    }
}

int MjpegServer::Private::maxClients() const
{
    if (isOpened())
    {
        return server->maxPendingConnections();
    }

    return (-1);
}

int MjpegServer::Private::writeInSocket(int sock, const QByteArray& data) const
{
    if (!data.isEmpty())
    {
        try
        {
            return (::send(sock, data.constData(), data.size(), MSG_NOSIGNAL));
        }
        catch (int e)
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "Socket::send() exception occurred:" << e;
        }
    }

    return (-1);
}

QString MjpegServer::Private::clientDescription(QTcpSocket* const client) const
{
    return (QString::fromLatin1("%1:%2").arg(client->peerAddress().toString())
                                        .arg(client->peerPort()));
}

bool MjpegServer::Private::isOpened() const
{
    if (!server)
    {
        return false;
    }

    return (server->isListening());
}

bool MjpegServer::Private::open(const QString& address, int port)
{
    server = new QTcpServer(parent());

    connect(server, SIGNAL(newConnection()),
            SLOT(slotNewConnection()));

    if (!server->listen(address.isEmpty() ? QHostAddress::Any
                                          : QHostAddress(address),
                        port))
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Error : couldn't listen with server"
                                       << server->serverAddress()
                                       << "to port" << server->serverPort() << "!";
        close();
        return false;
    }

    qCDebug(DIGIKAM_GENERAL_LOG) << "MJPEG server address    :" << server->serverAddress();
    qCDebug(DIGIKAM_GENERAL_LOG) << "MJPEG server port       :" << server->serverPort();

    return true;
}

void MjpegServer::Private::close()
{
    if (isOpened())
    {
        server->close();
    }

    server->deleteLater();
}

void MjpegServer::Private::start()
{
    srvTask = QtConcurrent::run(this, &MjpegServer::Private::writerThread);
    qCDebug(DIGIKAM_GENERAL_LOG) << "MJPEG server started...";
}

void MjpegServer::Private::stop()
{
    close();
    srvTask.waitForFinished();
    qCDebug(DIGIKAM_GENERAL_LOG) << "MJPEG server stopped...";
}

void MjpegServer::Private::slotNewConnection()
{
    while (server->hasPendingConnections())
    {
        QTcpSocket* const client = server->nextPendingConnection();

        if (client)
        {
            if (!blackList.contains(client->peerAddress().toString()))
            {
                connect(client, SIGNAL(disconnected()),
                        this, SLOT(slotClientDisconnected()));

                mutexClients.lock();
                {
                    client->write(QByteArray("HTTP/1.0 200 OK\r\n"));

                    client->write(QByteArray("Server: digiKamMjpeg/1.0\r\n"
                                             "Accept-Range: bytes\r\n"
                                             "Connection: close\r\n"
                                             "Max-Age: 0\r\n"
                                             "Expires: 0\r\n"
                                             "Cache-Control: no-cache, private\r\n"
                                             "Pragma: no-cache\r\n"
                                             "Content-Type: multipart/x-mixed-replace; boundary=--mjpegstream\r\n"
                                             "\r\n"));

                    clients.push_back(client);

                    qCDebug(DIGIKAM_GENERAL_LOG) << "MJPEG server new client    :" << clientDescription(client);
                    qCDebug(DIGIKAM_GENERAL_LOG) << "MJPEG server total clients :" << clients.count();
                }
                mutexClients.unlock();
            }
            else
            {
                client->close();
            }
        }
    }
}

void MjpegServer::Private::slotClientDisconnected()
{
    QTcpSocket* const client = dynamic_cast<QTcpSocket*>(sender());

    if (!client)
    {
        return;
    }

    mutexClients.lock();
    {
        int index = clients.indexOf(client);

        if (index != -1)
        {
            clients.removeAt(index);

            qCDebug(DIGIKAM_GENERAL_LOG) << "MJPEG server client disconnected :" << clientDescription(client);
            qCDebug(DIGIKAM_GENERAL_LOG) << "MJPEG server total clients       :" << clients.count();

            client->deleteLater();
        }
    }
    mutexClients.unlock();
}

// -----------------------------------------------------
// Multi-threaded methods.

void MjpegServer::Private::writerThread()
{
    while (isOpened())
    {
        QList <QFuture<void> > sockTasks;

        mutexFrame.lock();
        {
            mutexClients.lock();
            {
                foreach(QTcpSocket* const client, clients)
                {
                    sockTasks.append(QtConcurrent::run(
                                     this,
                                     &MjpegServer::Private::clientWriteMultithreaded,
                                     client->socketDescriptor(),
                                     lastFrame)
                                    );
                }
            }
            mutexClients.unlock();

            foreach(QFuture<void> t, sockTasks)
            {
                t.waitForFinished();
            }
        }
        mutexFrame.unlock();

        QThread::usleep(delay);
    }
}

void MjpegServer::Private::clientWriteMultithreaded(int client, const QByteArray& data)
{
    QString head;
    head.append(QLatin1String("--mjpegstream\r\n"
                              "Content-type: image/jpeg\r\n"
                              "Content-length: "));
    head.append(QString::number(data.size()));
    head.append(QLatin1String("\r\n\r\n"));

    // Write header

    writeInSocket(client, head.toLatin1());

    // Write image data

    writeInSocket(client, data);
    writeInSocket(client, QByteArray("\r\n\r\n"));
}

} // namespace DigikamGenericMjpegStreamPlugin
