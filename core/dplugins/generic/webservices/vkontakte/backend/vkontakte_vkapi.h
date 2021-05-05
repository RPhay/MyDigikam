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

#ifndef DIGIKAM_VKONTAKTE_VKAPI_H
#define DIGIKAM_VKONTAKTE_VKAPI_H

// Qt includes

#include <QObject>

// Local includes

#include "vkontakte_apppermissions.h"

class QWidget;
class KJob;

namespace Vkontakte
{

/**
 * @brief Authentication manager for libkvkontakte
 *
 * This is a handy interface on top of Vkontakte::AuthenticationDialog
 * to request authentication on demand and store the API token for later use.
 */
class VkApi : public QObject        // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

public:

    explicit VkApi(QWidget* const parent);
    ~VkApi() override;

    /**
     * @brief Initialize app ID.
     *
     * @param appId VK application ID for authentication requests.
     */
    void setAppId(const QString& appId);

    /**
     * @brief Set app permissions to request during authentication.
     *
     * @param permissions Any combination of AppPermissions::*, use bitwise OR to combine them
     */
    void setRequiredPermissions(Vkontakte::AppPermissions::Value permissions);

    /**
     * @brief Set initial access token.
     *
     * If an initial access token was set with this method, then before the
     * normal authentication process VkApi::startAuthentication() will first try
     * to reuse the given token in order to cut down the number of times the
     * authentication dialog is opened.
     */
    void setInitialAccessToken(const QString& accessToken);

    QString accessToken() const;

    void startAuthentication(bool forceLogout);
    bool isAuthenticated();

Q_SIGNALS:

    void authenticated();
    void canceled();

protected Q_SLOTS:

    void slotApplicationPermissionCheckDone(KJob* kjob);
    void slotAuthenticationDialogDone(const QString& accessToken);

private:

    class Private;
    Private* const d;
};

} // namespace Vkontakte

#endif // DIGIKAM_VKONTAKTE_VKAPI_H
