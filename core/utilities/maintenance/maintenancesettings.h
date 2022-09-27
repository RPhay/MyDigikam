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

#ifndef DIGIKAM_MAINTENANCE_SETTINGS_H
#define DIGIKAM_MAINTENANCE_SETTINGS_H

// Qt includes

#include <QDebug>

// Local includes

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

    explicit MaintenanceSettings();
    ~MaintenanceSettings();

public:

    bool                                    wholeAlbums;
    bool                                    wholeTags;

    AlbumList                               albums;
    AlbumList                               tags;

    /// Use Multi-core CPU to process items.
    bool                                    useMutiCoreCPU;

    /// Find new items on whole collection.
    bool                                    newItems;

    /// Generate thumbnails
    bool                                    thumbnails;
    /// Rebuild all thumbnails or only scan missing items.
    bool                                    scanThumbs;

    /// Generate finger-prints
    bool                                    fingerPrints;
    /// Rebuild all fingerprints or only scan missing items.
    bool                                    scanFingerPrints;

    /// Scan for new items
    bool                                    duplicates;
    /// Minimal similarity between items to compare, in percents.
    int                                     minSimilarity;
    /// Maximal similarity between items to compare, in percents.
    int                                     maxSimilarity;
    /// The type of restrictions to apply on duplicates search results.
    HaarIface::DuplicatesSearchRestrictions duplicatesRestriction;

    /// Scan for faces.
    bool                                    faceManagement;
    /// Face detection settings.
    FaceScanSettings                        faceSettings;

    /// Perform Image Quality Sorting.
    bool                                    qualitySort;
    /// Mode to assign Pick Labels to items.
    int                                     qualityScanMode;
    /// Type of quality settings selected.
    int                                     qualitySettingsSelected;
    /// Image Quality Sorting Settings.
    ImageQualityContainer                   quality;

    /// Sync metadata and DB.
    bool                                    metadataSync;
    /// Sync direction (image metadata <-> DB).
    int                                     syncDirection;

    /// Perform database cleanup
    bool                                    databaseCleanup;
    bool                                    cleanThumbDb;
    bool                                    cleanFacesDb;
    bool                                    cleanSimilarityDb;
    bool                                    shrinkDatabases;
};

//! qDebug() stream operator. Writes property @a s to the debug output in a nicely formatted way.
QDebug operator<<(QDebug dbg, const MaintenanceSettings& s);

} // namespace Digikam

#endif // DIGIKAM_MAINTENANCE_SETTINGS_H
