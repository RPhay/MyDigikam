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

#include "vkontakte_userinfo.h"

// Qt includes

#include <QStringList>

// Local includes

#include "vkontakte_util.h"

namespace Vkontakte
{

class Q_DECL_HIDDEN UserInfo::Private : public QSharedData
{
public:

    QJsonObject jsonData;
};

UserInfo::UserInfo()
    : d(new Private)
{
}

UserInfo::UserInfo(const QJsonObject& jsonData)
    : d(new Private)
{
    d->jsonData = jsonData;
}

UserInfo::UserInfo(const UserInfo& other)
    : d(other.d)
{
}

UserInfo::~UserInfo()
{
}

UserInfo &UserInfo::operator=(const UserInfo& other)
{
    if (this != &other)
    {
        d = other.d;
    }

    return *this;
}

int UserInfo::userId() const
{
    return d->jsonData.value(QLatin1String("uid")).toInt(-1);
}

QString UserInfo::firstName() const
{
    return d->jsonData.value(QLatin1String("first_name")).toString();
}

QString UserInfo::lastName() const
{
    return d->jsonData.value(QLatin1String("last_name")).toString();
}

QString UserInfo::nickName() const
{
    return d->jsonData.value(QLatin1String("nick_name")).toString();
}

QString UserInfo::domain() const
{
    return d->jsonData.value(QLatin1String("domain")).toString();
}

int UserInfo::sex() const
{
    return d->jsonData.value(QLatin1String("sex")).toInt(-1);
}

bool UserInfo::online() const
{
    return d->jsonData.value(QLatin1String("online")).toBool(false);
}

// static
QStringList UserInfo::allQueryFields()
{
    QStringList fields;

    fields << QLatin1String("sex")
           << QLatin1String("bdate")
           << QLatin1String("city")
           << QLatin1String("country")
           << QLatin1String("photo_50")
           << QLatin1String("photo_100")
           << QLatin1String("photo_200_orig")
           << QLatin1String("photo_200")
           << QLatin1String("photo_400_orig")
           << QLatin1String("photo_max")
           << QLatin1String("photo_max_orig")
           << QLatin1String("photo_id")
           << QLatin1String("online")
           << QLatin1String("online_mobile")
           << QLatin1String("domain")
           << QLatin1String("has_mobile")
           << QLatin1String("contacts")
           << QLatin1String("connections")
           << QLatin1String("site")
           << QLatin1String("education")
           << QLatin1String("universities")
           << QLatin1String("schools")
           << QLatin1String("can_post")
           << QLatin1String("can_see_all_posts")
           << QLatin1String("can_see_audio")
           << QLatin1String("can_write_private_message")
           << QLatin1String("status")
           << QLatin1String("last_seen")
           << QLatin1String("common_count")
           << QLatin1String("relation")
           << QLatin1String("relatives")
           << QLatin1String("counters")
           << QLatin1String("screen_name")
           << QLatin1String("maiden_name")
           << QLatin1String("timezone")
           << QLatin1String("occupation")
           << QLatin1String("activities")
           << QLatin1String("interests")
           << QLatin1String("music")
           << QLatin1String("movies")
           << QLatin1String("tv")
           << QLatin1String("books")
           << QLatin1String("games")
           << QLatin1String("about")
           << QLatin1String("quotes")
           << QLatin1String("personal")
           << QLatin1String("friends_status");

    return fields;
}

} // namespace Vkontakte
