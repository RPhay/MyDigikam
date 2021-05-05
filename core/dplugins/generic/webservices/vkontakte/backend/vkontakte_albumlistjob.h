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

#ifndef DIGIKAM_VKONTAKTE_ALBUMLISTJOB_H
#define DIGIKAM_VKONTAKTE_ALBUMLISTJOB_H

// Local includes

#include "vkontakte_jobs.h"
#include "vkontakte_albuminfo.h"

namespace Vkontakte
{

// https://vk.com/dev/photos.getAlbums
class AlbumListJob : public VkontakteJob
{
    Q_OBJECT

public:

    explicit AlbumListJob(const QString& accessToken, int uid = -1, const QList<int>& aids = QList<int>());
    ~AlbumListJob() override;

    QList<AlbumInfo> list() const;

protected:

    void handleData(const QJsonValue& data) override;
    void handleItem(const QVariant& data);

private:

    // Disable
    AlbumListJob(QObject*) = delete;

private:

    class Private;
    Private* const d;
};

} // namespace Vkontakte

#endif // DIGIKAM_VKONTAKTE_ALBUMLISTJOB_H
