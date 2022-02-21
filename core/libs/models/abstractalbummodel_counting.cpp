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

class Q_DECL_HIDDEN AbstractCountingAlbumModel::Private
{
public:

    explicit Private()
      : showCount(false)
    {
    }

    bool            showCount;
    QMap<int, int>  countMap;
    QHash<int, int> countHashReady;
    QSet<int>       includeChildrenAlbums;
};

AbstractCountingAlbumModel::AbstractCountingAlbumModel(Album::Type albumType,
                                                       Album* const rootAlbum,
                                                       RootAlbumBehavior rootBehavior,
                                                       QObject* const parent)
    : AbstractSpecificAlbumModel(albumType, rootAlbum, rootBehavior, parent),
      d(new Private)
{
}

void AbstractCountingAlbumModel::setup()
{
    connect(AlbumManager::instance(), SIGNAL(signalAlbumMoved(Album*)),
            this, SLOT(slotAlbumMoved(Album*)));
}

AbstractCountingAlbumModel::~AbstractCountingAlbumModel()
{
    delete d;
}

void AbstractCountingAlbumModel::setShowCount(bool show)
{
    if (d->showCount != show)
    {
        d->showCount = show;
        emitDataChangedForChildren(rootAlbum());
    }
}

bool AbstractCountingAlbumModel::showCount() const
{
    return d->showCount;
}

void AbstractCountingAlbumModel::excludeChildrenCount(const QModelIndex& index)
{
    Album* const album = albumForIndex(index);

    if (!album)
    {
        return;
    }

    d->includeChildrenAlbums.remove(album->id());
    updateCount(album);
}

void AbstractCountingAlbumModel::includeChildrenCount(const QModelIndex& index)
{
    Album* const album = albumForIndex(index);

    if (!album)
    {
        return;
    }

    d->includeChildrenAlbums << album->id();
    updateCount(album);
}

void AbstractCountingAlbumModel::setCountMap(const QMap<int, int>& idCountMap)
{
    d->countMap                       = idCountMap;
    QMap<int, int>::const_iterator it = d->countMap.constBegin();

    for ( ; it != d->countMap.constEnd() ; ++it)
    {
        updateCount(albumForId(it.key()));
    }
}

void AbstractCountingAlbumModel::updateCount(Album* album)
{
    if (!album)
    {
        return;
    }

    // if the model does not contain the album, do nothing.

    QModelIndex index = indexForAlbum(album);

    if (!index.isValid())
    {
        return;
    }

    QHash<int, int>::iterator includeIt = d->countHashReady.find(album->id());
    bool changed                        = false;

    // get count for album without children

    int count                           = d->countMap.value(album->id());

    // if wanted, add up children's counts

    if (d->includeChildrenAlbums.contains(album->id()))
    {
        AlbumIterator it(album);

        while (it.current())
        {
            count += d->countMap.value((*it)->id());
            ++it;
        }
    }

    // insert or update

    if (includeIt == d->countHashReady.end())
    {
        changed                        = true;
        d->countHashReady[album->id()] = count;
    }
    else
    {
        changed           = (includeIt.value() != count);
        includeIt.value() = count;
    }

    // notify views

    if (changed)
    {
        emit dataChanged(index, index);
    }
}

void AbstractCountingAlbumModel::setCount(Album* album, int count)
{
    if (!album)
    {
        return;
    }

    // if the model does not contain the album, do nothing.

    QModelIndex index = indexForAlbum(album);

    if (!index.isValid())
    {
        return;
    }

    QHash<int, int>::iterator includeIt = d->countHashReady.find(album->id());
    bool changed                        = false;

    // insert or update

    if (includeIt == d->countHashReady.end())
    {
        changed                        = true;
        d->countHashReady[album->id()] = count;
    }
    else
    {
        changed           = (includeIt.value() != count);
        includeIt.value() = count;
    }

    // notify views

    if (changed)
    {
        emit dataChanged(index, index);
    }
}

QVariant AbstractCountingAlbumModel::albumData(Album* album, int role) const
{
    if ((role == Qt::DisplayRole) && d->showCount && !album->isRoot())
    {
        QHash<int, int>::const_iterator it = d->countHashReady.constFind(album->id());

        if (it != d->countHashReady.constEnd())
        {
            return QString::fromUtf8("%1 (%2)").arg(albumName(album)).arg(it.value());
        }
    }

    return AbstractSpecificAlbumModel::albumData(album, role);
}

int AbstractCountingAlbumModel::albumCount(Album* album) const
{
    QHash<int, int>::const_iterator it = d->countHashReady.constFind(album->id());

    if (it != d->countHashReady.constEnd())
    {
        return it.value();
    }

    return -1;
}

QString AbstractCountingAlbumModel::albumName(Album* album) const
{
    return album->title();
}

void AbstractCountingAlbumModel::albumCleared(Album* album)
{
    if (!AlbumManager::instance()->isMovingAlbum(album))
    {
        d->countMap.remove(album->id());
        d->countHashReady.remove(album->id());
        d->includeChildrenAlbums.remove(album->id());
    }
}

void AbstractCountingAlbumModel::allAlbumsCleared()
{
    d->countMap.clear();
    d->countHashReady.clear();
    d->includeChildrenAlbums.clear();
}

void AbstractCountingAlbumModel::slotAlbumMoved(Album*)
{
    // need to update counts of all parents

    setCountMap(d->countMap);
}

} // namespace Digikam
