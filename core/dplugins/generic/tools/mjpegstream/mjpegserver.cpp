/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-24
 * Description : a MJPEG Stream server to export items on the network.
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes

#include <QBuffer>
#include <QByteArray>
#include <QIODevice>
#include <QString>
#include <QMutexLocker>

// Local includes

#include "digikam_debug.h"

namespace DigikamGenericMjpegStreamPlugin
{

MjpegServer::MjpegServer(const QString& address,
                         int port,
                         QObject* const parent)
    : QObject(parent),
      d      (new Private(parent))
{
    d->open(address, port);
    d->setMaxClients(10);
}

MjpegServer::~MjpegServer()
{
    d->close();

    // NOTE: QObject based => self deletion.
}

bool MjpegServer::setRate(int ra)
{
    if ((ra < 1) || (ra > 100))
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Error: rate value is out of range: " << ra;
        return false;
    }

    d->rate  = ra;
    d->delay = (int)(1000000.0 / ra);
    qCDebug(DIGIKAM_GENERAL_LOG) << "MJPEG Server rate       :" << d->rate;

    return true;
}

int MjpegServer::rate() const
{
    return d->rate;
}

bool MjpegServer::setMaxClients(int max)
{
    if ((max < 1) || (max > 30))
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Error: max clients value is out of range: " << max;
        return false;
    }

    d->setMaxClients(max);

    return true;
}

int MjpegServer::maxClients() const
{
    return d->maxClients();
}

void MjpegServer::setBlackList(const QStringList& lst)
{
    d->blackList = lst;
}

QStringList MjpegServer::blackList() const
{
    return d->blackList;
}

void MjpegServer::slotWriteFrame(const QByteArray& frame)
{
    d->mutexFrame.lock();
    {
        if (!frame.isNull())
        {
            // Perform a deep-copy of image data.

            d->lastFrame = QByteArray(frame.data(), frame.size());
        }
    }
    d->mutexFrame.unlock();
}

void MjpegServer::start()
{
    d->start();
}

void MjpegServer::stop()
{
    d->stop();
}

} // namespace DigikamGenericMjpegStreamPlugin
