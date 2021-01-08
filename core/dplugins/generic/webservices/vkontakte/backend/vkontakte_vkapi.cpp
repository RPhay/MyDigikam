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

#include "vkontakte_vkapi.h"

// Qt includes

#include <QPointer>

// Local includes

#include "vkontakte_authenticationdialog.h"
#include "vkontakte_getapplicationpermissionsjob.h"

namespace Vkontakte
{

class Q_DECL_HIDDEN VkApi::Private
{
public:

    QWidget*                         parent;
    QString                          appId;
    Vkontakte::AppPermissions::Value requiredPermissions;
    QString                          accessToken;
    bool                             authenticated;
};

VkApi::VkApi(QWidget* const parent)
    : d(new Private)
{
    d->parent              = parent;
    d->appId               = QString();
    d->requiredPermissions = Vkontakte::AppPermissions::NoPermissions;
    d->accessToken         = QString();
    d->authenticated       = false;
}

VkApi::~VkApi()
{
}

void VkApi::setAppId(const QString& appId)
{
    d->appId = appId;
}

void VkApi::setRequiredPermissions(Vkontakte::AppPermissions::Value permissions)
{
    d->requiredPermissions = permissions;
}

void VkApi::setInitialAccessToken(const QString& accessToken)
{
    // Does nothing if m_accessToken is already set, because this function
    // is only for parameter initialization from a configuration file.
    
    if (d->accessToken.isEmpty())
    {
        d->accessToken = accessToken;
    }
}

QString VkApi::accessToken() const
{
    return d->accessToken;
}

void VkApi::startAuthentication(bool forceLogout)
{
    if (forceLogout)
    {
        d->accessToken.clear();
    }

    if (!d->accessToken.isEmpty())
    {
        Vkontakte::GetApplicationPermissionsJob* const job = new Vkontakte::GetApplicationPermissionsJob(d->accessToken);

        connect(job, SIGNAL(result(KJob*)),
                this, SLOT(slotApplicationPermissionCheckDone(KJob*)));

        job->start();
    }
    else
    {
        QPointer<Vkontakte::AuthenticationDialog> authDialog = new Vkontakte::AuthenticationDialog(d->parent);
        authDialog->setAppId(d->appId);
        authDialog->setPermissions(d->requiredPermissions);

        connect(authDialog, SIGNAL(authenticated(QString)),
                this, SLOT(slotAuthenticationDialogDone(QString)));

        connect(authDialog, SIGNAL(canceled()),
                this, SIGNAL(canceled()));

        authDialog->start();
    }
}

void VkApi::slotApplicationPermissionCheckDone(KJob* kjob)
{
    Vkontakte::GetApplicationPermissionsJob* const job = dynamic_cast<Vkontakte::GetApplicationPermissionsJob *>(kjob);
    Q_ASSERT(job);

    bool havePermissions = true;

    if (!job || job->error())
    {
        // There was some error enumerating permissions, need to start over for sure
        havePermissions = false;
    }
    else
    {
        Vkontakte::AppPermissions::Value availablePermissions = static_cast<Vkontakte::AppPermissions::Value>(job->permissions());

        if ((availablePermissions & d->requiredPermissions) != d->requiredPermissions)
        {
            // Existing permissions are not enough, need to request more permissions
            havePermissions = false;
        }
    }

    if (havePermissions)
    {
        d->authenticated = true;
        emit authenticated();
    }
    else
    {
        startAuthentication(true);
    }
}

void VkApi::slotAuthenticationDialogDone(const QString& accessToken)
{
    d->accessToken   = accessToken;
    d->authenticated = true;
    emit authenticated();
}

bool VkApi::isAuthenticated()
{
    return d->authenticated;
}

} // namespace Vkontakte
