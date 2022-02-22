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

CheckableAlbumFilterModel::CheckableAlbumFilterModel(QObject* const parent)
    : AlbumFilterModel        (parent),
      m_filterChecked         (false),
      m_filterPartiallyChecked(false)
{
}

void CheckableAlbumFilterModel::setSourceAlbumModel(AbstractCheckableAlbumModel* const source)
{
    AlbumFilterModel::setSourceAlbumModel(source);
}

void CheckableAlbumFilterModel::setSourceFilterModel(CheckableAlbumFilterModel* const source)
{
    AlbumFilterModel::setSourceFilterModel(source);
}

AbstractCheckableAlbumModel* CheckableAlbumFilterModel::sourceAlbumModel() const
{
    return (static_cast<AbstractCheckableAlbumModel*>(AlbumFilterModel::sourceAlbumModel()));
}

void CheckableAlbumFilterModel::setFilterChecked(bool filter)
{
    m_filterChecked = filter;
    invalidateFilter();

    emit signalFilterChanged();
}

void CheckableAlbumFilterModel::setFilterPartiallyChecked(bool filter)
{
    m_filterPartiallyChecked = filter;
    invalidateFilter();

    emit signalFilterChanged();
}

bool CheckableAlbumFilterModel::isFiltering() const
{
    return (AlbumFilterModel::isFiltering() || m_filterChecked || m_filterPartiallyChecked);
}

bool CheckableAlbumFilterModel::matches(Album* album) const
{
    bool accepted = AlbumFilterModel::matches(album);

    if (!m_filterChecked && !m_filterPartiallyChecked)
    {
        return accepted;
    }

    Qt::CheckState state = sourceAlbumModel()->checkState(album);

    bool stateAccepted = false;

    if (m_filterPartiallyChecked)
    {
        stateAccepted |= (state == Qt::PartiallyChecked);
    }

    if (m_filterChecked)
    {
        stateAccepted |= (state == Qt::Checked);
    }

    return (accepted && stateAccepted);
}

} // namespace Digikam
