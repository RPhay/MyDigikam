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

TagModel::TagModel(RootAlbumBehavior rootBehavior, QObject* const parent)
    : AbstractCheckableAlbumModel(Album::TAG,
                                  AlbumManager::instance()->findTAlbum(0),
                                  rootBehavior, parent)
{
    m_columnHeader = i18n("Tags");
    setupThumbnailLoading();

    connect(AlbumManager::instance(), SIGNAL(signalTAlbumsDirty(QMap<int,int>)),
            this, SLOT(setCountMap(QMap<int,int>)));

    setCountMap(AlbumManager::instance()->getTAlbumsCount());
}

void TagModel::setColumnHeader(const QString& header)
{
    m_columnHeader = header;
}

TAlbum* TagModel::albumForIndex(const QModelIndex& index) const
{
    return static_cast<TAlbum*>(AbstractCheckableAlbumModel::albumForIndex(index));
}

QVariant TagModel::albumData(Album* a, int role) const
{
    if ((role == Qt::DisplayRole)                    &&
        !a->isRoot()                                 &&
        m_unconfirmedFaceCount.contains(a->id())     &&
        (a->id() != FaceTags::unknownPersonTagId()))
    {
        QString res = AbstractCheckableAlbumModel::albumData(a, role).toString() +
                      i18ncp("@info: unconfirmed faces in album", " (%1 new)", " (%1 new)",
                             m_unconfirmedFaceCount.value(a->id()));

        return res;
    }

    return AbstractCheckableAlbumModel::albumData(a, role);
}

QVariant TagModel::decorationRoleData(Album* album) const
{
    TAlbum* const tagAlbum = static_cast<TAlbum*>(album);

    if (isFaceTagModel() || tagAlbum->hasProperty(TagPropertyName::person()))
    {
        QPixmap face = AlbumThumbnailLoader::instance()->getFaceThumbnailDirectly(tagAlbum);
        int size     = isFaceTagModel() ? ApplicationSettings::instance()->getTreeViewFaceSize()
                                        : ApplicationSettings::instance()->getTreeViewIconSize();

        double ratio = face.devicePixelRatio();
        int rsize    = qRound((double)size * ratio);
        face         = face.scaled(rsize, rsize, Qt::KeepAspectRatio,
                                                 Qt::SmoothTransformation);

        QPixmap pix(rsize, rsize);
        pix.fill(Qt::transparent);
        pix.setDevicePixelRatio(ratio);

        QPainter p(&pix);
        p.drawPixmap((rsize - face.width())  / 2,
                     (rsize - face.height()) / 2, face);
        p.end();

        prepareAddExcludeDecoration(album, pix);

        return pix;
    }
    else
    {
        QPixmap pix = AlbumThumbnailLoader::instance()->getTagThumbnailDirectly(tagAlbum);
        prepareAddExcludeDecoration(album, pix);

        return pix;
    }
}

QVariant TagModel::fontRoleData(Album* a) const
{
    if (m_unconfirmedFaceCount.contains(a->id())  &&
        (a->id() != FaceTags::unknownPersonTagId()))
    {
        QFont font;
        font.setBold(true);

        return font;
    }

    return QVariant();
}

Album* TagModel::albumForId(int id) const
{
    return AlbumManager::instance()->findTAlbum(id);
}

void TagModel::activateFaceTagModel()
{
    disconnect(AlbumManager::instance(), SIGNAL(signalTAlbumsDirty(QMap<int,int>)),
               this, SLOT(setCountMap(QMap<int,int>)));

    connect(AlbumManager::instance(), &AlbumManager::signalFaceCountsDirty,
            this, [=](const QMap<int, int>& faceCount,
                      const QMap<int, int>& uFaceCount,
                      const QList<int>& toUpdatedFaces)
        {
            setCountMap(faceCount);
            m_unconfirmedFaceCount = uFaceCount;

            foreach (int id, toUpdatedFaces)
            {
                Album* const album = albumForId(id);

                if (!album)
                {
                    continue;
                }

                QModelIndex index = indexForAlbum(album);

                if (!index.isValid())
                {
                    continue;
                }

                emit dataChanged(index, index);
            }
        }
    );

    setFaceTagModel(true);
    setCountMap(AlbumManager::instance()->getFaceCount());
}

bool TagModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    TAlbum* const talbum = albumForIndex(index);

    if (talbum && talbum->isInternalTag())
    {
        return false;
    }

    return AbstractCheckableAlbumModel::setData(index, value, role);
}

} // namespace Digikam
