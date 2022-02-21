/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-03-23
 * Description : Qt Model for Albums
 *
 * Copyright (C) 2008-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010      by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright (C) 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "abstractalbummodel_p.h"

namespace Digikam
{

AbstractSpecificAlbumModel::AbstractSpecificAlbumModel(Album::Type albumType,
                                                       Album* const rootAlbum,
                                                       RootAlbumBehavior rootBehavior,
                                                       QObject* const parent)
    : AbstractAlbumModel(albumType, rootAlbum, rootBehavior, parent)
{
}

void AbstractSpecificAlbumModel::setupThumbnailLoading()
{
    AlbumThumbnailLoader* const loader = AlbumThumbnailLoader::instance();

    connect(loader, SIGNAL(signalThumbnail(Album*,QPixmap)),
            this, SLOT(slotGotThumbnailFromIcon(Album*,QPixmap)));

    connect(loader, SIGNAL(signalFailed(Album*)),
            this, SLOT(slotThumbnailLost(Album*)));

    connect(loader, SIGNAL(signalReloadThumbnails()),
            this, SLOT(slotReloadThumbnails()));
}

QString AbstractSpecificAlbumModel::columnHeader() const
{
    return m_columnHeader;
}

void AbstractSpecificAlbumModel::setColumnHeader(const QString& header)
{
    m_columnHeader = header;
    emit headerDataChanged(Qt::Horizontal, 0, 0);
}

void AbstractSpecificAlbumModel::slotGotThumbnailFromIcon(Album* album, const QPixmap&)
{
    // see decorationRole() method of subclasses

    if (!filterAlbum(album))
    {
        return;
    }

    QModelIndex index = indexForAlbum(album);
    emit dataChanged(index, index);
}

void AbstractSpecificAlbumModel::slotThumbnailLost(Album*)
{
    // ignore, use default thumbnail
}

void AbstractSpecificAlbumModel::slotReloadThumbnails()
{
    // emit dataChanged() for all albums

    emitDataChangedForChildren(rootAlbum());
}

void AbstractSpecificAlbumModel::emitDataChangedForChildren(Album* album)
{
    if (!album)
    {
        return;
    }

    for (Album* child = album->firstChild() ; child ; child = child->next())
    {
        if (filterAlbum(child))
        {
            // recurse to children of children

            emitDataChangedForChildren(child);

            // emit signal for child

            QModelIndex index = indexForAlbum(child);
            emit dataChanged(index, index);
        }
    }
}

} // namespace Digikam
