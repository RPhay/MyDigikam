/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-01-31
 * Description : maintenance manager settings
 *
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "maintenancesettings.h"

// Local includes

#include "digikam_globals.h"
#include "imagequalityconfselector.h"

namespace Digikam
{

MaintenanceSettings::MaintenanceSettings()
    : wholeAlbums            (true),
      wholeTags              (true),
      useMutiCoreCPU         (false),
      newItems               (false),
      thumbnails             (false),
      scanThumbs             (false),
      fingerPrints           (false),
      scanFingerPrints       (false),
      duplicates             (false),
      minSimilarity          (90),
      maxSimilarity          (100),
      duplicatesRestriction  (HaarIface::DuplicatesSearchRestrictions::None),
      faceManagement         (false),
      qualitySort            (false),
      qualityScanMode        (true),   // NOTE: turn on by default to prevent clearing whole Pick Labels from Collection
      qualitySettingsSelected(ImageQualityConfSelector::GlobalSettings),
      metadataSync           (false),
      syncDirection          (MetadataSynchronizer::WriteFromDatabaseToFile),
      databaseCleanup        (false),
      cleanThumbDb           (false),
      cleanFacesDb           (false),
      cleanSimilarityDb      (false),
      shrinkDatabases        (false)
{
}

MaintenanceSettings::~MaintenanceSettings()
{
}

//! qCDebug(DIGIKAM_GENERAL_LOG) stream operator. Writes property @a s to the debug output in a nicely formatted way.
QDebug operator<<(QDebug dbg, const MaintenanceSettings& s)
{
    dbg.nospace() << QT_ENDL;
    dbg.nospace() << "wholeAlbums            : " << s.wholeAlbums                         << QT_ENDL;
    dbg.nospace() << "wholeTags              : " << s.wholeTags                           << QT_ENDL;
    dbg.nospace() << "Albums                 : " << s.albums.count()                      << QT_ENDL;
    dbg.nospace() << "Tags                   : " << s.tags.count()                        << QT_ENDL;
    dbg.nospace() << "useMutiCoreCPU         : " << s.useMutiCoreCPU                      << QT_ENDL;
    dbg.nospace() << "newItems               : " << s.newItems                            << QT_ENDL;
    dbg.nospace() << "thumbnails             : " << s.thumbnails                          << QT_ENDL;
    dbg.nospace() << "scanThumbs             : " << s.scanThumbs                          << QT_ENDL;
    dbg.nospace() << "fingerPrints           : " << s.fingerPrints                        << QT_ENDL;
    dbg.nospace() << "scanFingerPrints       : " << s.scanFingerPrints                    << QT_ENDL;
    dbg.nospace() << "duplicates             : " << s.duplicates                          << QT_ENDL;
    dbg.nospace() << "minSimilarity          : " << s.minSimilarity                       << QT_ENDL;
    dbg.nospace() << "maxSimilarity          : " << s.maxSimilarity                       << QT_ENDL;
    dbg.nospace() << "duplicatesRestriction  : " << s.duplicatesRestriction               << QT_ENDL;
    dbg.nospace() << "faceManagement         : " << s.faceManagement                      << QT_ENDL;
    dbg.nospace() << "faceScannedHandling    : " << s.faceSettings.alreadyScannedHandling << QT_ENDL;
    dbg.nospace() << "qualitySort            : " << s.qualitySort                         << QT_ENDL;
    dbg.nospace() << "quality                : " << s.quality                             << QT_ENDL;
    dbg.nospace() << "qualityScanMode        : " << s.qualityScanMode                     << QT_ENDL;
    dbg.nospace() << "qualitySettingsSelected: " << s.qualitySettingsSelected             << QT_ENDL;
    dbg.nospace() << "metadataSync           : " << s.metadataSync                        << QT_ENDL;
    dbg.nospace() << "syncDirection          : " << s.syncDirection                       << QT_ENDL;
    dbg.nospace() << "databaseCleanup        : " << s.databaseCleanup                     << QT_ENDL;
    dbg.nospace() << "cleanThumbDb           : " << s.cleanThumbDb                        << QT_ENDL;
    dbg.nospace() << "cleanFacesDb           : " << s.cleanFacesDb                        << QT_ENDL;
    dbg.nospace() << "cleanSimilarityDb      : " << s.cleanSimilarityDb                   << QT_ENDL;
    dbg.nospace() << "shrinkDatabases        : " << s.shrinkDatabases                     << QT_ENDL;

    return dbg.space();
}

} // namespace Digikam
