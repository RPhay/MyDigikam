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

#include "vkontakte_friendlistjob.h"

// Qt includes

#include <qjson/qobjecthelper.h>

namespace Vkontakte
{

class Q_DECL_HIDDEN FriendListJob::Private
{
public:

    QList<UserInfoPtr> list;
};

// https://vk.com/dev/friends.get
FriendListJob::FriendListJob(const QString& accessToken, int uid)
    : VkontakteJob(accessToken, "friends.get"),
      d(new Private)
{
    if (uid != -1)
    {
        addQueryItem("uid", QString::number(uid));
    }

    addQueryItem("fields", UserInfo::allQueryFields().join(","));
}

FriendListJob::~FriendListJob()
{
    delete d;
}

QList<UserInfoPtr> FriendListJob::list() const
{
    return d->list;
}

void FriendListJob::handleData(const QVariant& data)
{
    foreach (const QVariant& user, data.toList())
    {
        UserInfoPtr userInfo(new UserInfo());
        QJson::QObjectHelper::qvariant2qobject(user.toMap(), userInfo.data());
        d->list.append(userInfo);
    }
}

} // namespace Vkontakte
