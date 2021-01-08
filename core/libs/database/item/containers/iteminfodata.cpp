/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-05-01
 * Description : ItemInfo common data
 *
 * Copyright (C) 2007-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "iteminfodata.h"

// Qt includes

#include <QHash>

// Local includes

#include "digikam_globals.h"

namespace Digikam
{

ItemInfoStatic* ItemInfoStatic::m_instance = nullptr;

void ItemInfoStatic::create()
{
    if (!m_instance)
    {
        m_instance = new ItemInfoStatic;
    }
}

void ItemInfoStatic::destroy()
{
    delete m_instance;
    m_instance = nullptr;
}

ItemInfoCache* ItemInfoStatic::cache()
{
    return &m_instance->m_cache;
}

// ---------------------------------------------------------------

ItemInfoData::ItemInfoData()
    : id(-1),
      currentReferenceImage(-1),
      albumId(-1),
      albumRootId(-1),

      pickLabel(NoPickLabel),
      colorLabel(NoColorLabel),
      rating(-1),
      category(DatabaseItem::UndefinedCategory),
      orientation(0),
      fileSize(0),
      manualOrder(0),
      unconfirmedFaceCount(0),

      longitude(0),
      latitude(0),
      altitude(0),
      currentSimilarity(0.0),

      groupImage(-1),

      hasCoordinates(false),
      hasAltitude(false),

      defaultTitleCached(false),
      defaultCommentCached(false),
      pickLabelCached(false),
      colorLabelCached(false),
      ratingCached(false),
      categoryCached(false),
      formatCached(false),
      creationDateCached(false),
      modificationDateCached(false),
      orientationCached(false),
      fileSizeCached(false),
      manualOrderCached(false),
      uniqueHashCached(false),
      imageSizeCached(false),
      tagIdsCached(false),
      positionsCached(false),
      groupImageCached(false),
      unconfirmedFaceCountCached(false),
      faceSuggestionsCached(false),

      invalid(false),

      hasVideoMetadata(true),
      hasImageMetadata(true),
      videoMetadataCached(DatabaseFields::VideoMetadataNone),
      imageMetadataCached(DatabaseFields::ImageMetadataNone)
{
}

ItemInfoData::~ItemInfoData()
{
}

} // namespace Digikam
