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

#ifndef DIGIKAM_VKONTAKTE_USERINFOJOB_H
#define DIGIKAM_VKONTAKTE_USERINFOJOB_H

// Qt includes

#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>

// Local includes

#include "vkontakte_jobs.h"
#include "vkontakte_userinfo.h"

namespace Vkontakte
{

/**
 * Get information about multiple users
 *
 * Uses VKontakte API method "users.get".
 */
// TODO: support working with more than 1000 users
class UserInfoJob : public VkontakteJob
{
    Q_OBJECT

public:

    /**
     * @brief Construct a job to retrieve the data of the currently authenticaed user.
     * @param accessToken The access token to access data on vkontakte.
    */
    explicit UserInfoJob(const QString& accessToken);

    /**
    * @brief Construct a job to retrieve the data of user with the given uid.
    * @param accessToken The access token to access data on vkontakte.
    * @param uid The user ID
    */
    UserInfoJob(const QString& accessToken, int uid);

    UserInfoJob(const QString& accessToken, const QList<int>& uids);
    ~UserInfoJob() override;

    /**
     * @return A pointer to a userInfo object about the currently authenticated user.
     */
    QList<UserInfo> userInfo() const;

    void setFields(const QStringList& fields);

protected:

    /**
     * @brief Handles the data returned by the VkontakteGetJob
     * @param data A JSON string containing the data.
     */
    void handleData(const QJsonValue& data) override;

    /**
     * @brief Prepares m_queryItems by several addQueryItem calls
     */
    void prepareQueryItems()                override;

private:

    // Disable
    UserInfoJob(QObject*);

    class Private;
    Private* const d;
};

} // namespace Vkontakte

#endif // DIGIKAM_VKONTAKTE_USERINFOJOB_H
