/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-01-31
 * Description : maintenance manager settings
 *
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "maintenancesettings.h"

namespace Digikam
{

MaintenanceSettings::MaintenanceSettings()
    : wholeAlbums(true),
      wholeTags(true),
      useMutiCoreCPU(false),
      newItems(false),
      thumbnails(false),
      scanThumbs(false),
      fingerPrints(false),
      scanFingerPrints(false),
      duplicates(false),
      minSimilarity(90),
      maxSimilarity(100),
      duplicatesRestriction(HaarIface::DuplicatesSearchRestrictions::None),
      faceManagement(false),
      qualitySort(false),
      qualityScanMode(true),   // NOTE: turn on by default to prevent clearing whole Pick Labels from Collection
      metadataSync(false),
      syncDirection(MetadataSynchronizer::WriteFromDatabaseToFile),
      databaseCleanup(false),
      cleanThumbDb(false),
      cleanFacesDb(false),
      shrinkDatabases(false)
{
}

MaintenanceSettings::~MaintenanceSettings()
{
}

//! qCDebug(DIGIKAM_GENERAL_LOG) stream operator. Writes property @a s to the debug output in a nicely formatted way.
QDebug operator<<(QDebug dbg, const MaintenanceSettings& s)
{
    dbg.nospace() << Qt::endl;
    dbg.nospace() << "wholeAlbums           : " << s.wholeAlbums << Qt::endl;
    dbg.nospace() << "wholeTags             : " << s.wholeTags << Qt::endl;
    dbg.nospace() << "Albums                : " << s.albums.count() << Qt::endl;
    dbg.nospace() << "Tags                  : " << s.tags.count() << Qt::endl;
    dbg.nospace() << "useMutiCoreCPU        : " << s.useMutiCoreCPU << Qt::endl;
    dbg.nospace() << "newItems              : " << s.newItems << Qt::endl;
    dbg.nospace() << "thumbnails            : " << s.thumbnails << Qt::endl;
    dbg.nospace() << "scanThumbs            : " << s.scanThumbs << Qt::endl;
    dbg.nospace() << "fingerPrints          : " << s.fingerPrints << Qt::endl;
    dbg.nospace() << "scanFingerPrints      : " << s.scanFingerPrints << Qt::endl;
    dbg.nospace() << "duplicates            : " << s.duplicates << Qt::endl;
    dbg.nospace() << "minSimilarity         : " << s.minSimilarity << Qt::endl;
    dbg.nospace() << "maxSimilarity         : " << s.maxSimilarity << Qt::endl;
    dbg.nospace() << "duplicatesRestriction : " << s.duplicatesRestriction << Qt::endl;
    dbg.nospace() << "faceManagement        : " << s.faceManagement << Qt::endl;
    dbg.nospace() << "faceScannedHandling   : " << s.faceSettings.alreadyScannedHandling << Qt::endl;
    dbg.nospace() << "qualitySort           : " << s.qualitySort << Qt::endl;
    dbg.nospace() << "quality               : " << s.quality << Qt::endl;
    dbg.nospace() << "qualityScanMode       : " << s.qualityScanMode << Qt::endl;
    dbg.nospace() << "metadataSync          : " << s.metadataSync << Qt::endl;
    dbg.nospace() << "syncDirection         : " << s.syncDirection << Qt::endl;
    dbg.nospace() << "databaseCleanup       : " << s.databaseCleanup << Qt::endl;
    dbg.nospace() << "cleanThumbDb          : " << s.cleanThumbDb << Qt::endl;
    dbg.nospace() << "cleanFacesDb          : " << s.cleanFacesDb << Qt::endl;
    dbg.nospace() << "shrinkDatabases       : " << s.shrinkDatabases << Qt::endl;

    return dbg.space();
}

} // namespace Digikam
