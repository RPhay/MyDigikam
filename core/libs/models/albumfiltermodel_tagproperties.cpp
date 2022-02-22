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

TagPropertiesFilterModel::TagPropertiesFilterModel(QObject* const parent)
    : CheckableAlbumFilterModel(parent)
{
    connect(AlbumManager::instance(), SIGNAL(signalTagPropertiesChanged(TAlbum*)),
            this, SLOT(tagPropertiesChanged(TAlbum*)));
}

void TagPropertiesFilterModel::setSourceAlbumModel(TagModel* const source)
{
    CheckableAlbumFilterModel::setSourceAlbumModel(source);
}

TagModel* TagPropertiesFilterModel::sourceTagModel() const
{
    return (dynamic_cast<TagModel*>(sourceModel()));
}

void TagPropertiesFilterModel::listOnlyTagsWithProperty(const QString& property)
{
    if (m_propertiesWhiteList.contains(property))
    {
        return;
    }

    m_propertiesWhiteList << property;
    invalidateFilter();

    emit signalFilterChanged();
}

void TagPropertiesFilterModel::removeListOnlyProperty(const QString& property)
{
    if (!m_propertiesWhiteList.contains(property))
    {
        return;
    }

    m_propertiesWhiteList.remove(property);
    invalidateFilter();

    emit signalFilterChanged();
}

void TagPropertiesFilterModel::doNotListTagsWithProperty(const QString& property)
{
    if (m_propertiesBlackList.contains(property))
    {
        return;
    }

    m_propertiesBlackList << property;
    invalidateFilter();

    emit signalFilterChanged();
}

void TagPropertiesFilterModel::removeDoNotListProperty(const QString& property)
{
    if (!m_propertiesBlackList.contains(property))
    {
        return;
    }

    m_propertiesBlackList.remove(property);
    invalidateFilter();

    emit signalFilterChanged();
}

bool TagPropertiesFilterModel::isFiltering() const
{
    return (!m_propertiesWhiteList.isEmpty() || !m_propertiesBlackList.isEmpty());
}

void TagPropertiesFilterModel::tagPropertiesChanged(TAlbum*)
{
    // I do not expect batch changes. Otherwise we'll need a timer.

    if (isFiltering())
    {
        invalidateFilter();

        // Sort new when tag properties change.

        invalidate();
    }
}

bool TagPropertiesFilterModel::matches(Album* album) const
{
    if (!CheckableAlbumFilterModel::matches(album))
    {
        return false;
    }

    TAlbum* const talbum = static_cast<TAlbum*>(album);

    foreach (const QString& prop, m_propertiesBlackList)
    {
        if (talbum->hasProperty(prop))
        {   // cppcheck-suppress useStlAlgorithm
            return false;
        }
    }

    foreach (const QString& prop, m_propertiesWhiteList)
    {
        if (!talbum->hasProperty(prop))
        {   // cppcheck-suppress useStlAlgorithm
            return false;
        }
    }

    return true;
}

} // namespace Digikam
