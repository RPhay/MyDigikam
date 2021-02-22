/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : a tool to export images to VKontakte web service
 *
 * Copyright (C) 2010      by Thomas McGuire <mcguire at kde dot org>
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

#include "vkontakte_util.h"

// Qt includes

#include <QString>
#include <QMap>

namespace Vkontakte
{

QDateTime unixTimeToQDateTime(const QString& unixTime)
{
    return QDateTime::fromSecsSinceEpoch(unixTime.toLongLong());
}

static QMap<Vkontakte::AppPermissions::Value, const char*> initAppPermissionStrings()
{
    QMap<Vkontakte::AppPermissions::Value, const char*> map;

    // https://vk.com/dev/permissions

    map.insert(Vkontakte::AppPermissions::Notify,        "notify");
    map.insert(Vkontakte::AppPermissions::Friends,       "friends");
    map.insert(Vkontakte::AppPermissions::Photos,        "photos");
    map.insert(Vkontakte::AppPermissions::Audio,         "audio");
    map.insert(Vkontakte::AppPermissions::Video,         "video");
    map.insert(Vkontakte::AppPermissions::Offers,        "offers");
    map.insert(Vkontakte::AppPermissions::Questions,     "questions");
    map.insert(Vkontakte::AppPermissions::Pages,         "pages");

    // "applink" permission cannot be requested, I invented
    // the name "applink" myself to somehow identify this permission flag only.

    map.insert(Vkontakte::AppPermissions::AppLink,       "applink");
    map.insert(Vkontakte::AppPermissions::Status,        "status");
    map.insert(Vkontakte::AppPermissions::Notes,         "notes");
    map.insert(Vkontakte::AppPermissions::Messages,      "messages");
    map.insert(Vkontakte::AppPermissions::Wall,          "wall");
    map.insert(Vkontakte::AppPermissions::Ads,           "ads");
    map.insert(Vkontakte::AppPermissions::Offline,       "offline");
    map.insert(Vkontakte::AppPermissions::Docs,          "docs");
    map.insert(Vkontakte::AppPermissions::Groups,        "groups");
    map.insert(Vkontakte::AppPermissions::Notifications, "notifications");
    map.insert(Vkontakte::AppPermissions::Stats,         "stats");
    map.insert(Vkontakte::AppPermissions::Email,         "email");

    return map;
}

// NOTE: This could be a QList<QPair<A, B> >, but with QMap we can write this shorter
static QMap<Vkontakte::AppPermissions::Value, const char*> appPermissionToString = initAppPermissionStrings();

QStringList appPermissionsToStringList(Vkontakte::AppPermissions::Value permissions)
{
    QStringList res;

    foreach (Vkontakte::AppPermissions::Value key, appPermissionToString.keys())
    {
        if (permissions & key)
        {
            res << QString::fromUtf8(appPermissionToString.value(key));
        }
    }

    return res;
}

QString joinIntegers(const QList<int>& list)
{
    QString res;

    foreach (int x, list)
    {
        if (!res.isEmpty())
        {
            res += QLatin1Char(',');
        }

        res += QString::number(x);
    }

    return res;
}

} // namespace Vkontakte
