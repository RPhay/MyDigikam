/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-06-05
 * Description : Thumbnail loading - private containers
 *
 * Copyright (C) 2006-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2015      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
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

#include "thumbnailloadthread_p.h"

namespace Digikam
{

bool ThumbnailLoadThread::Private::hasHighlightingBorder() const
{
    return (highlight && (size >= 10));
}

int ThumbnailLoadThread::Private::pixmapSizeForThumbnailSize(int thumbnailSize) const
{
    if (hasHighlightingBorder())
    {
        return (thumbnailSize + 2);
    }

    return thumbnailSize;
}

int ThumbnailLoadThread::Private::thumbnailSizeForPixmapSize(int pixmapSize) const
{
    // Bug #206666: Do not cut off one-pixel line for highlighting border

    if (hasHighlightingBorder())
    {
        return (pixmapSize - 2);
    }

    return pixmapSize;
}

bool ThumbnailLoadThread::Private::checkDescription(const LoadingDescription& description)
{
    QString cacheKey = description.cacheKey();

    {
        LoadingCache* const cache = LoadingCache::cache();
        LoadingCache::CacheLock lock(cache);

        if (cache->hasThumbnailPixmap(cacheKey))
        {
            return false;
        }
    }

    {
        QMutexLocker lock(&resultsMutex);

        if (collectedResults.contains(cacheKey))
        {
            return false;
        }
    }

    return true;
}

QList<LoadingDescription> ThumbnailLoadThread::Private::makeDescriptions(const QList<ThumbnailIdentifier>& identifiers, int size)
{
    QList<LoadingDescription> descriptions;
    {
        LoadingDescription description = createLoadingDescription(ThumbnailIdentifier(), size, false);

        foreach (const ThumbnailIdentifier& identifier, identifiers)
        {
            description.filePath = identifier.filePath;
            description.previewParameters.storageReference = identifier.id;

            if (!checkDescription(description))
            {
                continue;
            }

            descriptions << description;
        }
    }

    lastDescriptions = descriptions;

    return descriptions;
}

QList<LoadingDescription> ThumbnailLoadThread::Private::makeDescriptions(const QList<QPair<ThumbnailIdentifier, QRect> >& identifiersAndRects, int size)
{
    QList<LoadingDescription> descriptions;
    {
        LoadingDescription description = createLoadingDescription(ThumbnailIdentifier(), size, QRect(1, 1, 1, 1), false);
        typedef QPair<ThumbnailIdentifier, QRect> IdRectPair;

        foreach (const IdRectPair& pair, identifiersAndRects)
        {
            description.filePath                           = pair.first.filePath;
            description.previewParameters.storageReference = pair.first.id;

            if (!checkDescription(description))
            {
                continue;
            }

            description.previewParameters.extraParameter = pair.second;
            descriptions << description;
        }
    }

    lastDescriptions = descriptions;

    return descriptions;
}

void ThumbnailImageCatcher::Private::reset()
{
    intermediate.clear();
    tasks.clear();

    if (active)
    {
        state = Accepting;
    }
    else
    {
        state = Inactive;
    }
}

void ThumbnailImageCatcher::Private::harvest(const LoadingDescription& description, const QImage& image)
{
    // called under lock

    bool finished = true;

    for (int i = 0 ; i < tasks.size() ; ++i)
    {
        Private::CatcherResult& task = tasks[i];

        if (task.description == description)
        {
            task.image    = image;
            task.received = true;
        }

        finished = (finished && task.received);
    }

    // cppcheck-suppress knownConditionTrueFalse
    if (finished)
    {
        state = Quitting;
        condVar.wakeOne();
    }
}

} // namespace Digikam
