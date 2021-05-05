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

#include "vkontakte_cidsnamesjob.h"

// Qt includes

#include <QMap>
#include <QVariant>

namespace Vkontakte
{

class Q_DECL_HIDDEN CidsNamesJob::Private
{
public:

    QIntList           cids;
    QMap<int, QString> names; // cid -> name
};

// https://vk.com/dev/database.getCountriesById
// TODO: access token is not needed for this call

CidsNamesJob::CidsNamesJob(const QString& method,
                           const QString& accessToken,
                           const QIntList& cids)
    : VkontakteJob(accessToken, method),
      d(new Private)
{
    d->cids = cids;
    addQueryItem("cids", cids.join());
}

CidsNamesJob::~CidsNamesJob()
{
    delete d;
}

void CidsNamesJob::handleData(const QVariant& data)
{
    // We need QMap, because VK server might reorder results
    // (I did not check it, but they do not guarantee the same order as in "cids")

    foreach (const QVariant& item, data.toList())
    {
        QVariantMap item_map              = item.toMap();
        d->names[item_map["cid"].toInt()] = item_map["name"].toString();
    }
}

QMap<int, QString> CidsNamesJob::names() const
{
    return d->names;
}

} // namespace Vkontakte
