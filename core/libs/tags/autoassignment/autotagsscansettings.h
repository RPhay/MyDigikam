/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-10-09
 * Description : Autotags scan settings container.
 *
 * SPDX-FileCopyrightText: 2010-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "album.h"
#include "iteminfo.h"
#include "digikam_export.h"
#include "dnnmodeldefinitions.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT AutotagsScanSettings
{
    Q_GADGET

public:

    /**
     * Different possible tasks processed while scanning operation.
     */
    enum ScanMode
    {
        AllItems = 0,        ///< Scan all items
        NonAssignedItems     ///< Scan only items with no tags assigned.
    };
    Q_ENUM(ScanMode)

    /**
     * Different possible tasks processed while scanning operation.
     */
    enum TagMode
    {
        Replace = 0,        ///< Clean all tags already assigned
        Update              ///< Add new tags to existing tags
    };
    Q_ENUM(TagMode)

public:

    AutotagsScanSettings();
    ~AutotagsScanSettings();

public:

    /// Whole albums checked.
    ScanMode                                scanMode                    = ScanMode::AllItems;

    /// Processing power.
    bool                                    wholeAlbums                 = true;

    /// Whole albums checked.
    TagMode                                 tagMode                     = TagMode::Replace;

    /// Processing power.
    bool                                    useFullCpu                  = false;

    /// Object Detection Model.
    QString                                 objectDetectModel           = QStringLiteral("yolov11-nano");

    /// Object Detection Model.
    QString                                 imageClassificationModel    = QStringLiteral("efficientnet-b7");

    /// Albums to scan.
    AlbumList                               albums;

    /// confidence threshold
    int                                     uiConfidenceThreshold       = 7;

    /// Autotags languages
    QStringList                             languages;

    /// Set true for BQM
    bool                                    bqmMode                     = false;
};

} // namespace Digikam
