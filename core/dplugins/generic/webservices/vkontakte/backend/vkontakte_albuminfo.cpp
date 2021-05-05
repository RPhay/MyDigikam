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

#include "vkontakte_albuminfo.h"

// Local includes

#include "vkontakte_util.h"

namespace Vkontakte
{

class Q_DECL_HIDDEN AlbumInfo::Private : public QSharedData
{
public:

    QJsonObject jsonData;
};

// --------------------------------------------------------------------

AlbumInfo::AlbumInfo()
    : d()
{
}

AlbumInfo::AlbumInfo(const QJsonObject& jsonData)
    : d(new Private)
{
    d->jsonData = jsonData;
}

AlbumInfo::AlbumInfo(const AlbumInfo &other)
    : d(other.d)
{
}

AlbumInfo::~AlbumInfo()
{
}

AlbumInfo& AlbumInfo::operator=(const AlbumInfo& other)
{
    if (this != &other)
    {
        d = other.d;
    }

    return *this;
}

bool AlbumInfo::isEmpty() const
{
    return !d;
}

int AlbumInfo::aid() const
{
    return d->jsonData.value(QLatin1String("aid")).toInt(-1);
}

QString AlbumInfo::title() const
{
    return d->jsonData.value(QLatin1String("title")).toString();
}

QString AlbumInfo::description() const
{
    return d->jsonData.value(QLatin1String("description")).toString();
}

int AlbumInfo::size() const
{
    return d->jsonData.value(QLatin1String("size")).toInt(-1);
}

int AlbumInfo::privacy() const
{
    return d->jsonData.value(QLatin1String("privacy")).toInt(PRIVACY_UNKNOWN);
}

int AlbumInfo::commentPrivacy() const
{
    return d->jsonData.value(QLatin1String("comment_privacy")).toInt(PRIVACY_UNKNOWN);
}

} // namespace Vkontakte
