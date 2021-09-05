/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : List-view for the Showfoto folder view.
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "showfotofolderviewiconprovider.h"

// Qt includes

#include <QApplication>
#include <QMimeDatabase>
#include <QStyle>
#include <QPainter>
#include <QPixmap>
#include <QImage>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "loadingdescription.h"
#include "thumbnailloadthread.h"
#include "showfotofolderviewmodel.h"

using namespace Digikam;

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoFolderViewIconProvider::Private
{

public:

    explicit Private()
      : catcher     (nullptr),
        thread      (nullptr),
        model       (nullptr)
    {
    }

    ThumbnailImageCatcher*   catcher;       ///< Thumbnail thread catcher from main process.
    ThumbnailLoadThread*     thread;        ///< The separated thread to render thumbnail images. We will use the Showfoto core instance to optimize memory allocations.
    ShowfotoFolderViewModel* model;         ///< The MVC model, especially used to share icon-size property from view.
};

ShowfotoFolderViewIconProvider::ShowfotoFolderViewIconProvider(ShowfotoFolderViewModel* const model)
    : QFileIconProvider(),
      d                (new Private)
{
    d->model   = model;
    d->thread  = ThumbnailLoadThread::defaultThread();
    d->catcher = new ThumbnailImageCatcher(d->thread);
}

ShowfotoFolderViewIconProvider::~ShowfotoFolderViewIconProvider()
{
    d->catcher->thread()->stopAllTasks();
    d->catcher->cancel();

    delete d->catcher;
    delete d;
}

QIcon ShowfotoFolderViewIconProvider::icon(const QFileInfo& info) const
{
    // We will only process image files supported by Showfoto

    if (info.isFile() && !info.isSymLink() && !info.isDir() && !info.isRoot())
    {
        QString path    = info.absoluteFilePath();
        qCDebug(DIGIKAM_SHOWFOTO_LOG) << "request thumb icon for " << path;

        QMimeType mtype = QMimeDatabase().mimeTypeForFile(path);

        if (mtype.name().startsWith(QLatin1String("image/")))
        {
            // --- Critical section.

            // NOTE: this part run in separated thread.
            //       Do not use QPixmap here, as it's not re-entrant with X11 under Linux.

            d->catcher->setActive(true);    // ---

                d->catcher->thread()->find(ThumbnailIdentifier(path));
                d->catcher->enqueue();
                QList<QImage> images = d->catcher->waitForThumbnails();

            d->catcher->setActive(false);   // ---

            // --- End of critical section.

            if (!images.isEmpty())
            {
                // resize and center pixmap on target icon.

                QPixmap pix = QPixmap::fromImage(images.first());
                pix         = pix.scaled(d->model->iconSize(), Qt::KeepAspectRatio, Qt::FastTransformation);

                QPixmap icon(d->model->iconSize());
                icon.fill(Qt::transparent);
                QPainter p(&icon);
                p.drawPixmap((icon.width()  - pix.width() )  / 2,
                             (icon.height() - pix.height())  / 2,
                             pix);

                return icon;
            }
        }
    }

    // For non-iages, we will use default provider implementation.

    return QFileIconProvider::icon(info);
}

} // namespace ShowFoto
