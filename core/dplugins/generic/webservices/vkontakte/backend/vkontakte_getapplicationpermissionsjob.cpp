/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : a tool to export images to VKontakte web service
 *
 * Copyright (C) 2011-2015 by Alexander Potashev <aspotashev at gmail dot com>
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "vkontakte_getapplicationpermissionsjob.h"

// Qt includes

#include <QVariant>
#include <QJsonValue>

namespace Vkontakte
{

class Q_DECL_HIDDEN GetApplicationPermissionsJob::Private
{
public:

    int permissions;
};

// https://vk.com/dev/account.getAppPermissions
// https://vk.com/dev/permissions
GetApplicationPermissionsJob::GetApplicationPermissionsJob(const QString& accessToken)
    : VkontakteJob(accessToken, QLatin1String("account.getAppPermissions")),
      d(new Private)
{
}

GetApplicationPermissionsJob::~GetApplicationPermissionsJob()
{
    delete d;
}

void GetApplicationPermissionsJob::handleData(const QJsonValue& data)
{
    d->permissions = data.toInt(-1);

    // TODO: return error (call handleError?) when "data" is not an integer
}

int GetApplicationPermissionsJob::permissions() const
{
    return d->permissions;
}

} // namespace Vkontakte
