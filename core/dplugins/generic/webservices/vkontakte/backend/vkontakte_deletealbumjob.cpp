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

#include "vkontakte_deletealbumjob.h"

// Qt includes

#include <QJsonValue>

// KDE include

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"

namespace Vkontakte
{

DeleteAlbumJob::DeleteAlbumJob(const QString& accessToken, int aid)
    : VkontakteJob(accessToken, QLatin1String("photos.deleteAlbum"), true)
{
    addQueryItem(QLatin1String("aid"), QString::number(aid));
}

void DeleteAlbumJob::handleData(const QJsonValue& data)
{
    if (data.toInt(-1) != 1)
    {
        setError(1);
        setErrorText(i18n("Failed to delete album"));
        qCWarning(DIGIKAM_WEBSERVICES_LOG) << "Failed to delete album";
    }
}

} // namespace Vkontakte
