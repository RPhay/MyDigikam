/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : a tool to export images to VKontakte web service
 *
 * Copyright (C) 2011-2015 by Alexander Potashev <aspotashev at gmail dot com>
 * Copyright (C) 2011-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "vkontakte_userinfojob.h"
#include "vkontakte_util.h"

// Qt includes

#include <QJsonArray>

namespace Vkontakte
{

class Q_DECL_HIDDEN UserInfoJob::Private
{
public:
    QList<UserInfo> userInfo;
    QStringList fields;
};

// http://vk.com/dev/users.get
UserInfoJob::UserInfoJob(const QString& accessToken)
    : VkontakteJob(accessToken, QStringLiteral("users.get"))
    , d(new Private)
{
    // The complete list of fields
    setFields(UserInfo::allQueryFields()); // TODO: do not pull extra fields by default

    // TODO: support "counters" request (probably in another KJob)
}

UserInfoJob::UserInfoJob(const QString& accessToken, int uid)
    : VkontakteJob(accessToken, QStringLiteral("users.get"))
    , d(new Private)
{
    setFields(UserInfo::allQueryFields()); // TODO: do not pull extra fields by default
    addQueryItem(QStringLiteral("user_ids"), QString::number(uid));
}

UserInfoJob::UserInfoJob(const QString& accessToken, const QList<int>& uids)
    : VkontakteJob(accessToken, QStringLiteral("users.get"))
    , d(new Private)
{
    setFields(UserInfo::allQueryFields()); // TODO: do not pull extra fields by default
    addQueryItem(QStringLiteral("user_ids"), joinIntegers(uids));

    // TODO: make this working for more than 1000 uids
    // ("users.get" allows requesting only 1000 users at once)
}

UserInfoJob::~UserInfoJob()
{
    delete d;
}

QList<UserInfo> UserInfoJob::userInfo() const
{
    return d->userInfo;
}

void UserInfoJob::setFields(const QStringList& fields)
{
    d->fields = fields;
}

void UserInfoJob::prepareQueryItems()
{
    if (!d->fields.isEmpty())
    {
        addQueryItem(QStringLiteral("fields"), d->fields.join(QStringLiteral(",")));
    }
}

void UserInfoJob::handleData(const QJsonValue& data)
{
    if (!data.isArray())
    {
        // TODO: report error!!!
        return;
    }

    foreach (const QJsonValue& item, data.toArray())
    {
        if (!item.isObject())
        {
            // TODO: report error!!!
            d->userInfo.clear();
            return;
        }

        d->userInfo.append(UserInfo(item.toObject()));
    }
}

} // namespace Vkontakte
