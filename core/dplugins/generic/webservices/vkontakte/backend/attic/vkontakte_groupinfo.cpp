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

#include "vkontakte_groupinfo.h"

namespace Vkontakte
{

class Q_DECL_HIDDEN GroupInfo::Private
{
public:

    int         gid;
    QString     name;
    QString     screenName;
    bool        isClosed;
    bool        isAdmin;
    QString     photo;
    QString     photoMedium;
    QString     photoBig;
};

GroupInfo::GroupInfo()
    : d(new Private)
{
    d->gid      = -1;
    d->isClosed = true;
    d->isAdmin  = false;
}

GroupInfo::~GroupInfo()
{
    delete d;
}

void GroupInfo::setGid(int gid)
{
    d->gid = gid;
}

int GroupInfo::gid() const
{
    return d->gid;
}

void GroupInfo::setName(const QString& name)
{
    d->name = name;
}

QString GroupInfo::name() const
{
    return d->name;
}

void GroupInfo::setScreenName(const QString& screenName)
{
    d->screenName = screenName;
}

QString GroupInfo::screenName() const
{
    return d->screenName;
}

void GroupInfo::setClosed(bool closed)
{
    d->isClosed = closed;
}

bool GroupInfo::isClosed() const
{
    return d->isClosed;
}

void GroupInfo::setAdmin(bool admin)
{
    d->isAdmin = admin;
}

bool GroupInfo::isAdmin() const
{
    return d->isAdmin;
}

void GroupInfo::setPhoto(const QString& photo)
{
    d->photo = photo;
}

QString GroupInfo::photo() const
{
    return d->photo;
}

void GroupInfo::setPhotoMedium(const QString& photoMedium)
{
    d->photoMedium = photoMedium;
}

QString GroupInfo::photoMedium() const
{
    return d->photoMedium;
}

void GroupInfo::setPhotoBig(const QString& photoBig)
{
    d->photoBig = photoBig;
}

QString GroupInfo::photoBig() const
{
    return d->photoBig;
}

} // namespace Vkontakte
