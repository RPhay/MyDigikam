/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-10-09
 * Description : Face scan settings
 *
 * SPDX-FileCopyrightText: 2010-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FACE_SCAN_SETTINGS_H
#define DIGIKAM_FACE_SCAN_SETTINGS_H

// Local includes

#include "album.h"
#include "iteminfo.h"

namespace Digikam
{

class FaceScanSettings
{
public:

    enum ScanTask
    {
        Detect,
        DetectAndRecognize,
        RecognizeMarkedFaces,
        RetrainAll,
        BenchmarkDetection,
        BenchmarkRecognition
    };

    /// For detect and recognize
    enum AlreadyScannedHandling
    {
        Skip,
        Merge,
        Rescan,
        ClearAll
    };

public:

    explicit FaceScanSettings();
    ~FaceScanSettings();

public:

    /// whole albums checked
    bool                                    wholeAlbums;

    /// Processing power
    bool                                    useFullCpu;

    /// Use Yolo V3 model
    bool                                    useYoloV3;

    /// Detection accuracy
    double                                  accuracy;

    /// Albums to scan
    AlbumList                               albums;

    /// Image infos to scan
    ItemInfoList                            infos;

    ScanTask                                task;

    AlreadyScannedHandling                  alreadyScannedHandling;
};

} // namespace Digikam

#endif // DIGIKAM_FACE_SCAN_SETTINGS_H
