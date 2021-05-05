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

#include "vkontakte_photoinfo.h"

// Local includes

#include "vkontakte_util.h"

namespace Vkontakte
{

class Q_DECL_HIDDEN PhotoInfo::Private : public QSharedData
{
public:

    QJsonObject jsonData;
};

PhotoInfo::PhotoInfo()
    : d(new Private)
{
}

PhotoInfo::PhotoInfo(const QJsonObject& jsonData)
    : d(new Private)
{
    d->jsonData = jsonData;
}

PhotoInfo::PhotoInfo(const PhotoInfo& other)
    : d(other.d)
{
}

PhotoInfo::~PhotoInfo()
{
}

PhotoInfo &PhotoInfo::operator=(const PhotoInfo& other)
{
    if (this != &other)
    {
        d = other.d;
    }

    return *this;
}

int PhotoInfo::pid() const
{
    return d->jsonData.value(QLatin1String("pid")).toInt(-1);
}

QUrl PhotoInfo::urlMaxResolution() const
{
    // Photo URLs in the order of decresing size
    QStringList srcKeys;
    srcKeys.append(QLatin1String("src_xxxbig"));
    srcKeys.append(QLatin1String("src_xxbig"));
    srcKeys.append(QLatin1String("src_xbig"));
    srcKeys.append(QLatin1String("src_big"));
    srcKeys.append(QLatin1String("src"));
    srcKeys.append(QLatin1String("src_small"));

    foreach (const QString& key, srcKeys)
    {
        if (!d->jsonData.contains(key))
        {
            continue;
        }

        QJsonValue value = d->jsonData.value(key);

        if (value.isString())
        {
            return QUrl(value.toString());
        }
    }

    return QUrl();
}

} // namespace Vkontakte
