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

#include <QPointer>
#include <QApplication>
#include <QMimeDatabase>
#include <QStyle>
#include <QLocale>
#include <QPixmap>
#include <QImage>
#include <QScopedPointer>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "loadingdescription.h"

namespace ShowFoto
{

ShowfotoFolderViewIconProvider::ShowfotoFolderViewIconProvider()
    : QFileIconProvider()
{
    ThumbnailLoadThread* const thread = new ThumbnailLoadThread;
    m_catcher                         = new ThumbnailImageCatcher(thread);
}

ShowfotoFolderViewIconProvider::~ShowfotoFolderViewIconProvider()
{
    m_catcher->thread()->stopAllTasks();
    m_catcher->cancel();

    delete m_catcher->thread();
    delete m_catcher;
}

/*
QIcon ShowfotoFolderViewIconProvider::icon(const QFileInfo& info) const
{
    QString path    = info.absoluteFilePath();
    qCDebug(DIGIKAM_GENERAL_LOG) << "request thumb icon for " << path;

    QMimeType mtype = QMimeDatabase().mimeTypeForFile(path);

    return QIcon::fromTheme(mtype.iconName());
}
*/

QIcon ShowfotoFolderViewIconProvider::icon(const QFileInfo& info) const
{
    if (info.isFile() && !info.isSymLink() && !info.isDir() && !info.isRoot())
    {
        QString path    = info.absoluteFilePath();
        qCDebug(DIGIKAM_GENERAL_LOG) << "request thumb icon for " << path;

        QMimeType mtype = QMimeDatabase().mimeTypeForFile(path);

        if (mtype.name().startsWith(QLatin1String("image/")))
        {
            m_catcher->setActive(true);

            m_catcher->thread()->find(ThumbnailIdentifier(path));
            m_catcher->enqueue();
            QList<QImage> images = m_catcher->waitForThumbnails();

            m_catcher->setActive(false);

            if (!images.isEmpty())
            {
                return QIcon(QPixmap::fromImage(images.first()));
            }
        }
    }

    return QFileIconProvider::icon(info);
}

} // namespace ShowFoto
