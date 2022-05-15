/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-03-24
 * Description : Qt Model for Albums - filter model
 *
 * Copyright (C) 2008-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009      by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2014-2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
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

#include "albumfiltermodel_p.h"

namespace Digikam
{

TagsManagerFilterModel::TagsManagerFilterModel(QObject* const parent)
    : TagPropertiesFilterModel(parent)
{
}

void TagsManagerFilterModel::setQuickListTags(const QList<int>& tags)
{
    m_keywords.clear();

    Q_FOREACH (int tag, tags)
    {
        m_keywords << tag;
    }

    invalidateFilter();
    Q_EMIT signalFilterChanged();
}

bool TagsManagerFilterModel::matches(Album* album) const
{
    if (!TagPropertiesFilterModel::matches(album))
    {
        return false;
    }

    if (m_keywords.isEmpty())
    {
        return true;
    }

    bool dirty = false;

    for (QSet<int>::const_iterator it = m_keywords.begin() ;
         it != m_keywords.end() ; ++it)
    {
        TAlbum* const talbum = AlbumManager::instance()->findTAlbum(*it);

        if (!talbum)
        {
            continue;
        }

        if (talbum->title().compare(album->title()) == 0)
        {
            dirty = true;
        }
    }

    return dirty;
}

} // namespace Digikam
