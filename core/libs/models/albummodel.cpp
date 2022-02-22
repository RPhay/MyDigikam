/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-03-22
 * Description : Qt Model for Albums
 *
 * Copyright (C) 2008-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "albummodel_p.h"

namespace Digikam
{

AlbumModel::AlbumModel(RootAlbumBehavior rootBehavior, QObject* const parent)
    : AbstractCheckableAlbumModel(Album::PHYSICAL,
                                  AlbumManager::instance()->findPAlbum(0),
                                  rootBehavior, parent)
{
    m_columnHeader = i18n("Albums");
    setupThumbnailLoading();

    connect(AlbumManager::instance(), SIGNAL(signalPAlbumsDirty(QMap<int,int>)),
            this, SLOT(setCountMap(QMap<int,int>)));

    setCountMap(AlbumManager::instance()->getPAlbumsCount());
}

AlbumModel::~AlbumModel()
{
}

PAlbum* AlbumModel::albumForIndex(const QModelIndex& index) const
{
    return static_cast<PAlbum*>(AbstractCheckableAlbumModel::albumForIndex(index));
}

QVariant AlbumModel::decorationRoleData(Album* album) const
{
    // asynchronous signals are handled by parent class

    QPixmap thumb = AlbumThumbnailLoader::instance()->getAlbumThumbnailDirectly(static_cast<PAlbum*>(album));
    int size      = ApplicationSettings::instance()->getTreeViewIconSize();

    double ratio  = thumb.devicePixelRatio();
    int rsize     = qRound((double)size * ratio);
    thumb         = thumb.scaled(rsize, rsize, Qt::KeepAspectRatio,
                                               Qt::SmoothTransformation);

    QPixmap pix(rsize, rsize);
    pix.fill(Qt::transparent);
    pix.setDevicePixelRatio(ratio);

    QPainter p(&pix);
    p.drawPixmap((rsize - thumb.width())  / 2,
                 (rsize - thumb.height()) / 2, thumb);
    p.end();

    prepareAddExcludeDecoration(album, pix);

    return pix;
}

Album* AlbumModel::albumForId(int id) const
{
    return AlbumManager::instance()->findPAlbum(id);
}

QVariant AlbumModel::albumData(Album* a, int role) const
{
    if ((role == Qt::DisplayRole) &&
         showCount()              &&
         a->isTrashAlbum())
    {
        PAlbum* const palbum = AlbumManager::instance()->findPAlbum(a->parent()->id());

        if (palbum)
        {
            QString path = palbum->folderPath();
            path.append(QLatin1String(".dtrash/files"));
            QDir dir(path, QLatin1String(""), QDir::Unsorted, QDir::Files);

            return QString::fromUtf8("%1 (%2)").arg(albumName(a)).arg(dir.count());
        }
    }

    return AbstractCheckableAlbumModel::albumData(a, role);
}

} // namespace Digikam
