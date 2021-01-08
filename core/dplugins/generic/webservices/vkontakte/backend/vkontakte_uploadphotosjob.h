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

#ifndef DIGIKAM_VKONTAKTE_UPLOADPHOTOSJOB_H
#define DIGIKAM_VKONTAKTE_UPLOADPHOTOSJOB_H

// Qt includes

#include <QVariantMap>

// Local includes

#include "vkontakte_jobs.h"
#include "vkontakte_photoinfo.h"

namespace Vkontakte
{

class UploadPhotosJob : public KJobWithSubjobs
{
    Q_OBJECT

public:

    enum Dest
    {
        DEST_ALBUM = 1,
        DEST_PROFILE,
        DEST_WALL
    };

    explicit UploadPhotosJob(const QString& accessToken,
                             const QStringList& files, bool saveBig, int aid, int gid = -1);
    ~UploadPhotosJob() override;

    void start() override;

    QList<PhotoInfo> list() const;

protected:

    static const int MAX_POST_JOBS;
    static const int REQUEST_FILES_COUNT;

protected:

    int getMaxRequestFilesCount() const;

    bool mayStartPostJob();

    void startPostJob(int offset, int count);
    void startSaveJob(const QVariantMap& photoIdData);

Q_SIGNALS:

    void progress(int);

private Q_SLOTS:

    void serverJobFinished(KJob*);
    void postJobFinished(KJob*);
    void saveJobFinished(KJob*);

private:

    // Disable
    UploadPhotosJob(QObject*);

    class Private;
    Private* const d;
};

} // namespace Vkontakte

#endif // DIGIKAM_VKONTAKTE_UPLOADPHOTOSJOB_H
