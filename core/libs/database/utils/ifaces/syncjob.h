/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-10-04
 * Description : synchronize Input/Output jobs.
 *
 * Copyright (C) 2006-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_SYNC_JOB_H
#define DIGIKAM_SYNC_JOB_H

// Qt includes

#include <QObject>
#include <QPixmap>

// Local includes

#include "digikam_export.h"

class QString;

namespace Digikam
{

class Album;
class TAlbum;

// -------------------------------------------------------------------------------

class DIGIKAM_GUI_EXPORT SyncJob : public QObject
{
    Q_OBJECT

public:

    /**
     * Load the image or icon for the tag thumbnail.
     */
    static QPixmap getTagThumbnail(TAlbum* const album);
    static QPixmap getTagThumbnail(const QString& name, int size);

private:

    void enterWaitingLoop()                          const;
    void quitWaitingLoop()                           const;

    QPixmap getTagThumbnailPriv(TAlbum* const album) const;

private Q_SLOTS:

    void slotGotThumbnailFromIcon(Album* album, const QPixmap& pix);
    void slotLoadThumbnailFailed(Album* album);

private:

    // Disable
    SyncJob();
    explicit SyncJob(QObject*) = delete;
    ~SyncJob() override;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SYNC_JOB_H
