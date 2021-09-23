/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : Showfoto folder view model.
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

#include "showfotofolderviewmodel.h"

// Qt includes

#include <QPainter>
#include <QIODevice>
#include <QModelIndex>
#include <QMimeDatabase>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "thumbnailloadthread.h"
#include "showfotofolderviewlist.h"

using namespace Digikam;

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoFolderViewModel::Private
{

public:

    explicit Private()
        : thumbnailThread (nullptr),
          view            (nullptr)
    {
    }

    ThumbnailLoadThread*    thumbnailThread;
    ShowfotoFolderViewList* view;

    static const int        s_maxSize;         ///< Max icon size.
};

const int ShowfotoFolderViewModel::Private::s_maxSize = 256;

ShowfotoFolderViewModel::ShowfotoFolderViewModel(ShowfotoFolderViewList* const view)
    : QFileSystemModel(view),
      d               (new Private)
{
    d->view               = view;

    setObjectName(QLatin1String("ShowfotoFolderModel"));

    // --- Populate the model

    setRootPath(QDir::rootPath());

    // If an item fails the filter, hide it

    setNameFilterDisables(false);

    d->thumbnailThread = new ThumbnailLoadThread;
    d->thumbnailThread->setSendSurrogatePixmap(false);
    d->thumbnailThread->setThumbnailSize(Private::s_maxSize);

    connect(d->thumbnailThread, SIGNAL(signalThumbnailLoaded(LoadingDescription,QPixmap)),
            this, SLOT(refreshThumbnails(LoadingDescription,QPixmap)));
}

ShowfotoFolderViewModel::~ShowfotoFolderViewModel()
{
    d->thumbnailThread->stopAllTasks();

    delete d->thumbnailThread;
    delete d;
}

int ShowfotoFolderViewModel::maxIconSize()
{
    return (Private::s_maxSize);
}

QVariant ShowfotoFolderViewModel::data(const QModelIndex& index, int role) const
{
    if ((role == Qt::DecorationRole) && (index.column() == 0))
    {
        QFileInfo info(fileInfo(index));

        if (info.isFile() && !info.isSymLink() && !info.isDir() && !info.isRoot())
        {
            QString path    = info.absoluteFilePath();
            qCDebug(DIGIKAM_SHOWFOTO_LOG) << "request thumb icon for " << path;

            QMimeType mtype = QMimeDatabase().mimeTypeForFile(path);
            QString suffix  = info.suffix().toUpper();

            if (mtype.name().startsWith(QLatin1String("image/")) ||
                (suffix == QLatin1String("PGF"))                 ||
                (suffix == QLatin1String("KRA"))                 ||
                (suffix == QLatin1String("CR3"))                 ||
                (suffix == QLatin1String("HEIC"))                ||
                (suffix == QLatin1String("HEIF")))
            {
                QPixmap pix;

                if (d->thumbnailThread->find(ThumbnailIdentifier(path), pix))
                {
                    pix = pix.scaled(d->view->iconSize(), Qt::KeepAspectRatio,
                                                          Qt::FastTransformation);

                    QPixmap icon(d->view->iconSize());
                    icon.fill(Qt::transparent);
                    QPainter p(&icon);
                    p.drawPixmap((icon.width()  - pix.width() ) / 2,
                                 (icon.height() - pix.height()) / 2,
                                 pix);

                    return icon;
                }
            }
        }
    }

    return QFileSystemModel::data(index, role);
}

void ShowfotoFolderViewModel::refreshThumbnails(const LoadingDescription& desc, const QPixmap& pix)
{
    QModelIndex current = index(desc.filePath);

    if (current.isValid() && !pix.isNull())
    {
        emit dataChanged(current, current);
    }
}

} // namespace ShowFoto
