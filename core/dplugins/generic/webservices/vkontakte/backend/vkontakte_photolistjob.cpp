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

#include "vkontakte_photolistjob.h"

// Qt includes

#include <QJsonArray>

// Local includes

#include "vkontakte_util.h"

namespace Vkontakte
{

class Q_DECL_HIDDEN PhotoListJob::Private
{
public:

    QList<PhotoInfo> list;
};

// https://vk.com/dev/photos.get
//
// The API also allows to set "limit" and "offset", but that
// does not seem to be useful.
PhotoListJob::PhotoListJob(const QString& accessToken,
                           int uid, int aid, const QList<int>& pids)
    : VkontakteJob(accessToken, QLatin1String("photos.get")),
      d(new Private)
{
    addQueryItem(QLatin1String("uid"), QString::number(uid));
    addQueryItem(QLatin1String("aid"), QString::number(aid));

    if (!pids.empty())
    {
        addQueryItem(QLatin1String("pids"), joinIntegers(pids));
    }
}

PhotoListJob::~PhotoListJob()
{
    delete d;
}

void PhotoListJob::handleData(const QJsonValue& data)
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

        d->list.append(PhotoInfo(item.toObject()));
    }
}

QList<PhotoInfo> PhotoListJob::list() const
{
    return d->list;
}

} // namespace Vkontakte
