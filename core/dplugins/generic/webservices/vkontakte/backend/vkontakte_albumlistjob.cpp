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

#include "vkontakte_albumlistjob.h"

// Qt includes

#include <QJsonArray>
#include <QJsonObject>

// Local includes

#include "vkontakte_util.h"

namespace Vkontakte
{

class Q_DECL_HIDDEN AlbumListJob::Private
{
public:

    QList<AlbumInfo> list;
};

AlbumListJob::AlbumListJob(const QString& accessToken, int uid, const QList<int>& aids)
    : VkontakteJob(accessToken, QLatin1String("photos.getAlbums")),
      d(new Private)
{
    if (uid != -1)
    {
        addQueryItem(QLatin1String("uid"), QString::number(uid));
    }

    if (!aids.empty())
    {
        addQueryItem(QLatin1String("aids"), joinIntegers(aids));
    }
}

AlbumListJob::~AlbumListJob()
{
    delete d;
}

void AlbumListJob::handleData(const QJsonValue& data)
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
            d->list.clear();
            return;
        }

        d->list.append(AlbumInfo(item.toObject()));
    }
}

QList<AlbumInfo> AlbumListJob::list() const
{
    return d->list;
}

} // namespace Vkontakte
