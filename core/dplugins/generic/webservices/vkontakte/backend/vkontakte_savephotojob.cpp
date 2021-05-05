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

#include "vkontakte_savephotojob.h"

// Qt includes

#include <QJsonArray>

namespace Vkontakte
{

SavePhotoJob::SavePhotoJob(const QString& accessToken,
                           UploadPhotosJob::Dest dest,
                           const QVariantMap& photoIdData, int gid)
    : VkontakteJob(accessToken, getMethod(dest), true)
{
    m_dest = dest;

    switch (dest)
    {
        case Vkontakte::UploadPhotosJob::DEST_ALBUM:
        {
            addQueryItem(QLatin1String("aid"),         photoIdData[QLatin1String("aid")].toString());
            addQueryItem(QLatin1String("server"),      photoIdData[QLatin1String("server")].toString());
            addQueryItem(QLatin1String("photos_list"), photoIdData[QLatin1String("photos_list")].toString());
            addQueryItem(QLatin1String("hash"),        photoIdData[QLatin1String("hash")].toString());

            // TODO: may be "gid" will also be in "photoIdData", so this argument is unnecessary?

            if (gid != -1)
            {
                addQueryItem(QLatin1String("gid"), QString::number(gid));
            }

            break;
        }

        case Vkontakte::UploadPhotosJob::DEST_PROFILE:
        {
            addQueryItem(QLatin1String("server"), photoIdData[QLatin1String("server")].toString());
            addQueryItem(QLatin1String("photo"),  photoIdData[QLatin1String("photos")].toString());
            addQueryItem(QLatin1String("hash"),   photoIdData[QLatin1String("hash")].toString());
            break;
        }

        case Vkontakte::UploadPhotosJob::DEST_WALL:
        {
            // TODO: support optional parameters "uid" and "gid" (for posting to other users' and groups' walls)
            // TODO: for posting onto a wall, we must also call the "wall.post" VK method

            addQueryItem(QLatin1String("server"), photoIdData[QLatin1String("server")].toString());
            addQueryItem(QLatin1String("photo"),  photoIdData[QLatin1String("photo")].toString());
            addQueryItem(QLatin1String("hash"),   photoIdData[QLatin1String("hash")].toString());
            break;
        }

        default:
        {
            // TODO: handle unknown destination error

            break;
        }
    }
}

// static

QString SavePhotoJob::getMethod(Vkontakte::UploadPhotosJob::Dest dest)
{
    switch (dest)
    {
        case Vkontakte::UploadPhotosJob::DEST_ALBUM:
        {
            return QLatin1String("photos.save");
        }

        case Vkontakte::UploadPhotosJob::DEST_PROFILE:
        {
            return QLatin1String("photos.saveProfilePhoto");
        }

        case Vkontakte::UploadPhotosJob::DEST_WALL:
        {
            return QLatin1String("photos.saveWallPhoto");
        }

        default:
        {
            return QLatin1String("");
        }
    }
}

void SavePhotoJob::handleItem(const QJsonValue& item)
{
    if (!item.isObject())
    {
        // TODO: report error!!!

        m_list.clear();
        return;
    }

    m_list.append(PhotoInfo(item.toObject()));
}

void SavePhotoJob::handleData(const QJsonValue& data)
{
    switch (m_dest)
    {
        case Vkontakte::UploadPhotosJob::DEST_ALBUM:
        {
            if (!data.isArray())
            {
                // TODO: report error!!!

                return;
            }

            foreach (const QJsonValue& item, data.toArray())
            {
                handleItem(item);
            }
            break;
        }

        case Vkontakte::UploadPhotosJob::DEST_PROFILE:
        case Vkontakte::UploadPhotosJob::DEST_WALL:
        {
            handleItem(data);
            break;
        }

        default:
        {
            break;
        }
    }
}

QList<PhotoInfo> SavePhotoJob::list() const
{
    return m_list;
}

} // namespace Vkontakte
