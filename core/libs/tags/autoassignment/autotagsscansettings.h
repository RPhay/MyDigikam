/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2025-01-06
 * Description : autotags scanning settings container
 *
 * SPDX-FileCopyrightText: 2024-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

namespace Digikam
{

class DIGIKAM_GUI_EXPORT AutoTagsScanSettings
{
    Q_GADGET

public:

   /**
    * Scanning modes
    */
    enum ScanMode
    {
        AllItems = 0,        ///< Clean all tags already assigned and re-scan all items.
        NonAssignedItems     ///< Scan only items with no tags assigned.
    };

    /**
     * Objects detection model
     */
    enum DetectorModel
    {
        YOLOV5NANO = 0,   ///< YOLO nano neural network model.
        YOLOV5XLARGE,     ///< YOLO large neural network model.
        RESNET50

        // Add here another model.
    };

public:

    AutoTagsScanSettings();
    ~AutoTagsScanSettings();

public:

    /// Whole albums checked.
    bool                                    wholeAlbums = false;

    /// Albums to scan.
    AlbumList                               albums;

    ScanMode                                mode        = AllItems;

    DetectorModel                           modelType   = YOLOV5NANO;

    QStringList                             langs;
};

} // namespace Digikam
