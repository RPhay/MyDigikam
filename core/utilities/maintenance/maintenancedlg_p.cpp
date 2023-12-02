/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-01-30
 * Description : maintenance dialog
 *
 * SPDX-FileCopyrightText: 2012-2023 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "maintenancedlg_p.h"

namespace Digikam
{

MaintenanceDlg::Private::Private()
   : buttons                 (nullptr),
     logo                    (nullptr),
     title                   (nullptr),
     scanThumbs              (nullptr),
     scanFingerPrints        (nullptr),
     useLastSettings         (nullptr),
     useMutiCoreCPU          (nullptr),
     cleanThumbsDb           (nullptr),
     cleanFacesDb            (nullptr),
     cleanSimilarityDb       (nullptr),
     retrainAllFaces         (nullptr),
     shrinkDatabases         (nullptr),
     qualityScanMode         (nullptr),
     metadataSetup           (nullptr),
     qualitySelector         (nullptr),
     syncDirection           (nullptr),
     similarityRangeBox      (nullptr),
     dupeRestrictionBox      (nullptr),
     vbox                    (nullptr),
     vbox2                   (nullptr),
     vbox3                   (nullptr),
     vbox4                   (nullptr),
     duplicatesBox           (nullptr),
     similarityRange         (nullptr),
     faceScannedHandling     (nullptr),
     searchResultRestriction (nullptr),
     expanderBox             (nullptr),
     albumSelectors          (nullptr)
{
}

const QString MaintenanceDlg::Private::configGroupName(QLatin1String("MaintenanceDlg Settings"));
const QString MaintenanceDlg::Private::configUseLastSettings(QLatin1String("UseLastSettings"));
const QString MaintenanceDlg::Private::configUseMutiCoreCPU(QLatin1String("UseMutiCoreCPU"));
const QString MaintenanceDlg::Private::configNewItems(QLatin1String("NewItems"));
const QString MaintenanceDlg::Private::configThumbnails(QLatin1String("Thumbnails"));
const QString MaintenanceDlg::Private::configScanThumbs(QLatin1String("ScanThumbs"));
const QString MaintenanceDlg::Private::configFingerPrints(QLatin1String("FingerPrints"));
const QString MaintenanceDlg::Private::configScanFingerPrints(QLatin1String("ScanFingerPrints"));
const QString MaintenanceDlg::Private::configDuplicates(QLatin1String("Duplicates"));
const QString MaintenanceDlg::Private::configMinSimilarity(QLatin1String("minSimilarity"));
const QString MaintenanceDlg::Private::configMaxSimilarity(QLatin1String("maxSimilarity"));
const QString MaintenanceDlg::Private::configDuplicatesRestriction(QLatin1String("duplicatesRestriction"));
const QString MaintenanceDlg::Private::configFaceManagement(QLatin1String("FaceManagement"));
const QString MaintenanceDlg::Private::configFaceScannedHandling(QLatin1String("FaceScannedHandling"));
const QString MaintenanceDlg::Private::configAutotagsAssignment(QLatin1String("AutotagsAssignment"));
const QString MaintenanceDlg::Private::configAutotaggingScanMode(QLatin1String("AutotaggingScanMode"));
const QString MaintenanceDlg::Private::configModelSelectionMode(QLatin1String("ModelSelectionMode"));
const QString MaintenanceDlg::Private::configImageQualitySorter(QLatin1String("ImageQualitySorter"));
const QString MaintenanceDlg::Private::configQualityScanMode(QLatin1String("QualityScanMode"));
const QString MaintenanceDlg::Private::configQualitySettingsSelected(QLatin1String("QualitySettingsSelected"));
const QString MaintenanceDlg::Private::configMetadataSync(QLatin1String("MetadataSync"));
const QString MaintenanceDlg::Private::configSyncDirection(QLatin1String("SyncDirection"));
const QString MaintenanceDlg::Private::configCleanupDatabase(QLatin1String("CleanupDatabase"));
const QString MaintenanceDlg::Private::configCleanupThumbDatabase(QLatin1String("CleanupThumbDatabase"));
const QString MaintenanceDlg::Private::configCleanupFacesDatabase(QLatin1String("CleanupFacesDatabase"));
const QString MaintenanceDlg::Private::configCleanupSimilarityDatabase(QLatin1String("CleanupSimilarityDatabase"));
const QString MaintenanceDlg::Private::configShrinkDatabases(QLatin1String("ShrinkDatabases"));

} // namespace Digikam
