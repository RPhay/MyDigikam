/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-31
 * Description : digiKam global static QNetworkAccessManager
 *
 * Copyright (C) 2022 by Maik Qualmann <metzpinguin at gmail dot com>
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

#include "networkmanager.h"

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN NetworkManager::Private
{
public:

    explicit Private()
      : networkManager(nullptr)
    {
    }

    QNetworkAccessManager* networkManager;
};

// -----------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN NetworkManagerCreator
{
public:

    NetworkManager object;
};

Q_GLOBAL_STATIC(NetworkManagerCreator, networkManagerCreator)

// -----------------------------------------------------------------------------------------------

NetworkManager::NetworkManager()
    : d(new Private)
{
    d->networkManager = new QNetworkAccessManager(this);
}

NetworkManager::~NetworkManager()
{
    delete d;
}

NetworkManager* NetworkManager::instance()
{
    return &networkManagerCreator->object;
}

QNetworkAccessManager* NetworkManager::getNetworkManager(QObject* object) const
{
    if (thread() == object->thread())
    {
        return d->networkManager;
    }

    qCDebug(DIGIKAM_GENERAL_LOG) << "New QNetworkAccessManager for" << object;

    return (new QNetworkAccessManager(object));
}

} // namespace Digikam
