/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-03-13
 * Description : Image files selection dialog - list view icon provider.
 *
 * Copyright (C) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "imagedialog_p.h"

namespace Digikam
{

class Q_DECL_HIDDEN ImageDialogIconProvider::Private
{

public:

    explicit Private()
      : catcher     (nullptr),
        thread      (nullptr)
    {
    }

    ThumbnailImageCatcher* catcher;           ///< Thumbnail thread catcher from main process.
    ThumbnailLoadThread*   thread;            ///< The separated thread to render thumbnail images.
};

ImageDialogIconProvider::ImageDialogIconProvider()
    : QFileIconProvider(),
      d                (new Private)
{
    d->thread  = new ThumbnailLoadThread;
    d->thread->setThumbnailSize(256);
    d->thread->setPixmapRequested(false);
    d->catcher = new ThumbnailImageCatcher(d->thread);
}

ImageDialogIconProvider::~ImageDialogIconProvider()
{
    d->catcher->thread()->stopAllTasks();
    d->catcher->cancel();

    delete d->catcher->thread();
    delete d->catcher;
    delete d;
}

QIcon ImageDialogIconProvider::icon(const QFileInfo& info) const
{
    // We will only process image files

    if (info.isFile() && !info.isSymLink() && !info.isDir() && !info.isRoot())
    {
        QString path    = info.absoluteFilePath();
        qCDebug(DIGIKAM_GENERAL_LOG) << "request thumb icon for " << path;

        QMimeType mtype = QMimeDatabase().mimeTypeForFile(path);
        QString suffix  = info.suffix().toUpper();

        if (mtype.name().startsWith(QLatin1String("image/")) ||
            (suffix == QLatin1String("PGF"))                 ||
            (suffix == QLatin1String("KRA"))                 ||
            (suffix == QLatin1String("CR3"))                 ||
            (suffix == QLatin1String("HEIC"))                ||
            (suffix == QLatin1String("HEIF")))
        {
            // --- Critical section.

            // NOTE: this part run in separated thread.
            //       Do not use QPixmap here, as it's not re-entrant with X11 under Linux.

            d->catcher->setActive(true);    // ---

                d->catcher->thread()->find(ThumbnailIdentifier(path));
                d->catcher->enqueue();
                QList<QImage> images = d->catcher->waitForThumbnails();

                if (!images.isEmpty())
                {
                    // resize and center pixmap on target icon.

                    QPixmap pix = QPixmap::fromImage(images.first());
                    pix         = pix.scaled(QSize(256, 256), Qt::KeepAspectRatio, Qt::FastTransformation);

                    QPixmap icon(QSize(256, 256));
                    icon.fill(Qt::transparent);
                    QPainter p(&icon);
                    p.drawPixmap((icon.width()  - pix.width() )  / 2,
                                 (icon.height() - pix.height())  / 2,
                                 pix);

                    return icon;
                }

            // --- End of critical section.

            d->catcher->setActive(false);   // ---
        }
    }

    // For non-iages, we will use default provider implementation.

    return QFileIconProvider::icon(info);
}

} // namespace Digikam
