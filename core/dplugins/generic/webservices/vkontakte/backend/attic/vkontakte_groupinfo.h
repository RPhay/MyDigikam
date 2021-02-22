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

#ifndef DIGIKAM_VKONTAKTE_GROUPINFO_H
#define DIGIKAM_VKONTAKTE_GROUPINFO_H

// Qt includes

#include <QSharedPointer>

namespace Vkontakte
{

// https://vk.com/dev/groups.get
class GroupInfo : public QObject
{
    Q_OBJECT

    // gid
    Q_PROPERTY(int gid WRITE setGid READ gid)

    // If "extended" was requested, then these fields will be also available:
    // name, screen_name, is_closed, is_admin, photo, photo_medium, photo_big
    Q_PROPERTY(QString name WRITE setName READ name)
    Q_PROPERTY(QString screen_name WRITE setScreenName READ screenName)
    Q_PROPERTY(bool is_closed WRITE setClosed READ isClosed)
    Q_PROPERTY(bool is_admin WRITE setAdmin READ isAdmin)
    Q_PROPERTY(QString photo WRITE setPhoto READ photo)
    Q_PROPERTY(QString photo_medium WRITE setPhotoMedium READ photoMedium)
    Q_PROPERTY(QString photo_big WRITE setPhotoBig READ photoBig)

public:

    GroupInfo();
    ~GroupInfo();

    void setGid(int gid);
    int gid()               const;

    void setName(const QString& name);
    QString name()          const;

    void setScreenName(const QString& screenName);
    QString screenName()    const;

    void setClosed(bool closed);
    bool isClosed()         const;

    void setAdmin(bool admin);
    bool isAdmin()          const;

    void setPhoto(const QString& photo);
    QString photo()         const;

    void setPhotoMedium(const QString& photoMedium);
    QString photoMedium()   const;

    void setPhotoBig(const QString& photoBig);
    QString photoBig()      const;

private:

    class Private;
    Private* const d;
};

typedef QSharedPointer<GroupInfo> GroupInfoPtr;

} // namespace Vkontakte

#endif // DIGIKAM_VKONTAKTE_GROUPINFO_H
