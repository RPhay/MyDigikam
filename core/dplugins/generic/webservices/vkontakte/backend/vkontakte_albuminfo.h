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

#ifndef DIGIKAM_VKONTAKTE_ALBUMINFO_H
#define DIGIKAM_VKONTAKTE_ALBUMINFO_H

// Qt includes

#include <QJsonObject>
#include <QSharedDataPointer>

namespace Vkontakte
{

// https://vk.com/dev/photos.getAlbums
class AlbumInfo
{
public:

    enum
    {
        PRIVACY_UNKNOWN            = -1,
        PRIVACY_PUBLIC             = 0,
        PRIVACY_FRIENDS            = 1,
        PRIVACY_FRIENDS_OF_FRIENDS = 2,
        PRIVACY_PRIVATE            = 3
    };

public:

    AlbumInfo();
    AlbumInfo(const AlbumInfo& other);
    explicit AlbumInfo(const QJsonObject& jsonData);
    ~AlbumInfo();

    AlbumInfo& operator=(const AlbumInfo& other);

    bool isEmpty()        const;

    int aid()             const;
    QString title()       const;
    QString description() const;

    /**
     * @brief Returns number of photos in the album
     */
    int size()            const;

    int privacy()         const;
    int commentPrivacy()  const;

private:

    class Private;
    QSharedDataPointer<Private> d;
};

} // namespace Vkontakte

#endif // DIGIKAM_VKONTAKTE_ALBUMINFO_H
