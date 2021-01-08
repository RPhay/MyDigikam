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

#ifndef DIGIKAM_VKONTAKTE_GETPHOTOUPLOADSERVERJOB_H
#define DIGIKAM_VKONTAKTE_GETPHOTOUPLOADSERVERJOB_H

// Local includes

#include "vkontakte_jobs.h"
#include "vkontakte_uploadphotosjob.h"

namespace Vkontakte
{

// This class is not exported, so:
//    - we don't care about ABI of this class (not using Pimpl)
//    - library's users should use class UploadPhotosJob
class GetPhotoUploadServerJob : public VkontakteJob
{
    Q_OBJECT

public:

    GetPhotoUploadServerJob(const QString& accessToken, Vkontakte::UploadPhotosJob::Dest m_dest);

    void initUploadAlbum(int aid, int gid = -1);
    QUrl uploadUrl() const;

protected:

    void prepareQueryItems()                override;
    void handleData(const QJsonValue& data) override;

    static QString getMethod(enum UploadPhotosJob::Dest dest);

private:

    UploadPhotosJob::Dest m_dest;
    QUrl                  m_uploadUrl;
    int                   m_aid;
    int                   m_gid;
    int                   m_uid;

private:

    // Disable
    GetPhotoUploadServerJob(QObject*);
};

} // namespace Vkontakte

#endif // DIGIKAM_VKONTAKTE_GETPHOTOUPLOADSERVERJOB_H
