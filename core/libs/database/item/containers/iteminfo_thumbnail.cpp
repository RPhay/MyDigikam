/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-04-21
 * Description : Handling access to one item and associated data
 *
 * Copyright (C) 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2007-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013      by Michael G. Hansen <mike at mghansen dot de>
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

#include "iteminfo_p.h"

namespace Digikam
{

ThumbnailIdentifier ItemInfo::thumbnailIdentifier() const
{
    if (!m_data)
    {
        return ThumbnailIdentifier();
    }

    ThumbnailIdentifier id;
    id.id       = m_data->id;
    id.filePath = filePath();

    return id;
}

ThumbnailInfo ItemInfo::thumbnailInfo() const
{
    if (!m_data)
    {
        return ThumbnailInfo();
    }

    ThumbnailInfo thumbinfo;

    thumbinfo.id               = m_data->id;
    thumbinfo.filePath         = filePath();
    thumbinfo.fileName         = name();
    thumbinfo.isAccessible     = CollectionManager::instance()->locationForAlbumRootId(m_data->albumRootId).isAvailable();
    thumbinfo.modificationDate = modDateTime();
    thumbinfo.orientationHint  = orientation();
    thumbinfo.uniqueHash       = uniqueHash();
    thumbinfo.fileSize         = fileSize();

    if      (category() == DatabaseItem::Image)
    {
        thumbinfo.mimeType = QLatin1String("image");
    }
    else if (category() == DatabaseItem::Video)
    {
        thumbinfo.mimeType = QLatin1String("video");
    }

    return thumbinfo;
}

ThumbnailIdentifier ItemInfo::thumbnailIdentifier(qlonglong id)
{
    ItemInfo info(id);

    return info.thumbnailIdentifier();
}

} // namespace Digikam
