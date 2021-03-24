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

#ifndef DIGIKAM_VKONTAKTE_PHOTOPOSTJOB_H
#define DIGIKAM_VKONTAKTE_PHOTOPOSTJOB_H

// Qt includes

#include <QStringList>
#include <QVariantMap>

// Local includes

#include "vkontakte_jobs.h"
#include "vkontakte_uploadphotosjob.h"

class QHttpMultiPart;
class QNetworkReply;

namespace Vkontakte
{

// This class is not exported, so:
//    - we don't care about ABI of this class (not using Pimpl)
//    - library's users should use class UploadPhotosJob
class PhotoPostJob : public KJob
{
    Q_OBJECT

public:

    // files.size <= 5
    explicit PhotoPostJob(Vkontakte::UploadPhotosJob::Dest dest,
                          const QUrl& url,
                          const QStringList& files);

    void start() override;
    QVariantMap response() const;

protected:

    /**
     * Check for a return error and set the appropriate error messages
     */
    void handleError(const QJsonValue& data);

private Q_SLOTS:

    void parseNetworkResponse(QNetworkReply* reply);

private:

    bool appendFile(QHttpMultiPart* multiPart, const QString& header, const QString& path);

private:

    UploadPhotosJob::Dest m_dest;
    QUrl                  m_url;
    QStringList           m_files;
    bool                  m_ok;
    QVariantMap           m_response;

private:

    // Disable
    explicit PhotoPostJob(QObject*) = delete;
};

} // namespace Vkontakte

#endif // DIGIKAM_VKONTAKTE_PHOTOPOSTJOB_H
