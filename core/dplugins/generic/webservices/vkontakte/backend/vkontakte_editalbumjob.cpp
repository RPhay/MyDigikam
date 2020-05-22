/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : a tool to export images to VKontakte web service
 *
 * Copyright (C) 2011-2015 by Alexander Potashev <aspotashev at gmail dot com>
 * Copyright (C) 2011-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "vkontakte_editalbumjob.h"

#include <klocalizedstring.h>

#include <QDebug>
#include <QJsonValue>

namespace Vkontakte
{

EditAlbumJob::EditAlbumJob(const QString &accessToken,
                           int aid, const QString &title, const QString &description,
                           int privacy, int comment_privacy)
    : VkontakteJob(accessToken, QStringLiteral("photos.editAlbum"), true)
    , d(0)
{
    addQueryItem(QStringLiteral("aid"), QString::number(aid));
    addQueryItem(QStringLiteral("title"), title);
    if (!description.isEmpty())
        addQueryItem(QStringLiteral("description"), description);
    if (privacy != AlbumInfo::PRIVACY_UNKNOWN)
        addQueryItem(QStringLiteral("privacy"), QString::number(privacy));
    if (comment_privacy != AlbumInfo::PRIVACY_UNKNOWN)
        addQueryItem(QStringLiteral("comment_privacy"), QString::number(comment_privacy));
}

void EditAlbumJob::handleData(const QJsonValue &data)
{
    if (data.toInt(-1) != 1)
    {
        setError(1);
        setErrorText(i18n("Failed to edit album"));
        qWarning() << "Failed to edit album";
    }
}

} /* namespace Vkontakte */
