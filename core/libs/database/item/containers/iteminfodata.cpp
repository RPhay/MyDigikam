/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-05-01
 * Description : ItemInfo common data
 *
 * SPDX-FileCopyrightText: 2007-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2013      by Michael G. Hansen <mike at mghansen dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
    : id                        (-1),
      currentReferenceImage     (-1),
      albumId                   (-1),
      albumRootId               (-1),

      pickLabel                 (NoPickLabel),
      colorLabel                (NoColorLabel),
      rating                    (-1),
      category                  (DatabaseItem::UndefinedCategory),
      orientation               (0),
      fileSize                  (0),
      manualOrder               (0),
      faceCount                 (0),
      unconfirmedFaceCount      (0),

      longitude                 (0),
      latitude                  (0),
      altitude                  (0),
      currentSimilarity         (0.0),

      groupImage                (-1),

      hasCoordinates            (false),
      hasAltitude               (false),

      defaultTitleCached        (false),
      defaultCommentCached      (false),
      pickLabelCached           (false),
      colorLabelCached          (false),
      ratingCached              (false),
      categoryCached            (false),
      formatCached              (false),
      creationDateCached        (false),
      modificationDateCached    (false),
      orientationCached         (false),
      fileSizeCached            (false),
      manualOrderCached         (false),
      uniqueHashCached          (false),
      imageSizeCached           (false),
      tagIdsCached              (false),
      positionsCached           (false),
      groupImageCached          (false),
      unconfirmedFaceCountCached(false),
      faceSuggestionsCached     (false),
      faceCountCached           (false),

      invalid                   (false),

      hasVideoMetadata          (true),
      hasImageMetadata          (true),
      videoMetadataCached       (DatabaseFields::VideoMetadataNone),
      imageMetadataCached       (DatabaseFields::ImageMetadataNone)
{
}

ItemInfoData::~ItemInfoData()
{
}

} // namespace Digikam
