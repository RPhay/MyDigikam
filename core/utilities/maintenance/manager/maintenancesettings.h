/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-01-31
 * Description : maintenance manager settings
 *
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QDebug>

// Local includes

#include "autotagsassign.h"
#include "autotagsassignment.h"
#include "imagequalityconfselector.h"
#include "album.h"
#include "facescansettings.h"
#include "haariface.h"
#include "imagequalitycontainer.h"
#include "metadatasynchronizer.h"
#include "imagequalitysorter.h"

namespace Digikam
{

class MaintenanceSettings
{

public:

    MaintenanceSettings()  = default;
    ~MaintenanceSettings() = default;

public:

    bool                                    wholeAlbums             = true;
    bool                                    wholeTags               = true;

    AlbumList                               albums;
    AlbumList                               tags;


    /// Use Multi-core CPU to process items.
    bool                                    useMutiCoreCPU          = false;

    /// Find new items on whole collection.
    bool                                    newItems                = false;

    /// Generate thumbnails
    bool                                    thumbnails              = false;

    /// Rebuild all thumbnails or only scan missing items.
    bool                                    scanThumbs              = false;

    /// Generate finger-prints
    bool                                    fingerPrints            = false;

    /// Rebuild all fingerprints or only scan missing items.
    bool                                    scanFingerPrints        = false;

    /// Scan for new items
    bool                                    duplicates              = false;

    /// Minimal similarity between items to compare, in percents.
    int                                     minSimilarity           = 90;

    /// Maximal similarity between items to compare, in percents.
    int                                     maxSimilarity           = 100;

    /// The type of restrictions to apply on duplicates search results.
    HaarIface::DuplicatesSearchRestrictions duplicatesRestriction   = HaarIface::DuplicatesSearchRestrictions::None;

    /// Scan for faces.
    bool                                    faceManagement          = false;

    /// Face detection settings.
    FaceScanSettings                        faceSettings;

    /// Autotags assignment.
    bool                                    autotagsAssignment      = false;

    /// autotagging scan mode
    int                                     autotaggingScanMode     = AutotagsAssignment::AllItems;

    /// model selection mode
    int                                     modelSelectionMode      = DetectorModel::YOLOV5NANO;
    /// Autotags languages
    QStringList                             autotagsLanguages;



    /// Perform Image Quality Sorting.
    bool                                    qualitySort             = false;

    /// Mode to assign Pick Labels to items.
    /// NOTE: turn all items by default to prevent clearing whole Pick Labels from Collection
    int                                     qualityScanMode         = ImageQualitySorter::AllItems;

    /// Type of quality settings selected.
    int                                     qualitySettingsSelected = ImageQualityConfSelector::GlobalSettings;

    /// Image Quality Sorting Settings.
    ImageQualityContainer                   quality;

    /// Sync metadata and DB.
    bool                                    metadataSync            = false;

    /// Sync direction (image metadata <-> DB).
    int                                     syncDirection           = MetadataSynchronizer::WriteFromDatabaseToFile;

    /// Perform database cleanup
    bool                                    databaseCleanup         = false;
    bool                                    cleanThumbDb            = false;
    bool                                    cleanFacesDb            = false;
    bool                                    cleanSimilarityDb       = false;
    bool                                    shrinkDatabases         = false;
};

//! qDebug() stream operator. Writes property @a s to the debug output in a nicely formatted way.
QDebug operator<<(QDebug dbg, const MaintenanceSettings& s);

} // namespace Digikam
