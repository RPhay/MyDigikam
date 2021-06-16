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

// Local includes

#include "digikam_globals.h"

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
    dbg.nospace() << QT_ENDL;
    dbg.nospace() << "wholeAlbums           : " << s.wholeAlbums                         << QT_ENDL;
    dbg.nospace() << "wholeTags             : " << s.wholeTags                           << QT_ENDL;
    dbg.nospace() << "Albums                : " << s.albums.count()                      << QT_ENDL;
    dbg.nospace() << "Tags                  : " << s.tags.count()                        << QT_ENDL;
    dbg.nospace() << "useMutiCoreCPU        : " << s.useMutiCoreCPU                      << QT_ENDL;
    dbg.nospace() << "newItems              : " << s.newItems                            << QT_ENDL;
    dbg.nospace() << "thumbnails            : " << s.thumbnails                          << QT_ENDL;
    dbg.nospace() << "scanThumbs            : " << s.scanThumbs                          << QT_ENDL;
    dbg.nospace() << "fingerPrints          : " << s.fingerPrints                        << QT_ENDL;
    dbg.nospace() << "scanFingerPrints      : " << s.scanFingerPrints                    << QT_ENDL;
    dbg.nospace() << "duplicates            : " << s.duplicates                          << QT_ENDL;
    dbg.nospace() << "minSimilarity         : " << s.minSimilarity                       << QT_ENDL;
    dbg.nospace() << "maxSimilarity         : " << s.maxSimilarity                       << QT_ENDL;
    dbg.nospace() << "duplicatesRestriction : " << s.duplicatesRestriction               << QT_ENDL;
    dbg.nospace() << "faceManagement        : " << s.faceManagement                      << QT_ENDL;
    dbg.nospace() << "faceScannedHandling   : " << s.faceSettings.alreadyScannedHandling << QT_ENDL;
    dbg.nospace() << "qualitySort           : " << s.qualitySort                         << QT_ENDL;
    dbg.nospace() << "quality               : " << s.quality                             << QT_ENDL;
    dbg.nospace() << "qualityScanMode       : " << s.qualityScanMode                     << QT_ENDL;
    dbg.nospace() << "metadataSync          : " << s.metadataSync                        << QT_ENDL;
    dbg.nospace() << "syncDirection         : " << s.syncDirection                       << QT_ENDL;
    dbg.nospace() << "databaseCleanup       : " << s.databaseCleanup                     << QT_ENDL;
    dbg.nospace() << "cleanThumbDb          : " << s.cleanThumbDb                        << QT_ENDL;
    dbg.nospace() << "cleanFacesDb          : " << s.cleanFacesDb                        << QT_ENDL;
    dbg.nospace() << "shrinkDatabases       : " << s.shrinkDatabases                     << QT_ENDL;

    return dbg.space();
}

} // namespace Digikam
