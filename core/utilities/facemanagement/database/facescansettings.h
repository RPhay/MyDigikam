/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-10-09
 * Description : Face scan settings
 *
 * SPDX-FileCopyrightText: 2010-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

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

    /// To detect and recognize
    enum AlreadyScannedHandling
    {
        Skip,
        Merge,
        Rescan,
        ClearAll
    };

    /// face detection AI models
    enum FaceDetectionModel
    {
        YOLOv3,
        YuNet
    };

    /// Face detection size
    enum FaceDetectionSize
    {
        ExtraSmall,
        Small,
        Medium,
        Large,
        ExtraLarge
    };

    /// face recognition AI models
    enum FaceRecognitionModel
    {
        OpenFace,
        SFace
    };

public:

    FaceScanSettings()  = default;
    ~FaceScanSettings() = default;

public:

    /// whole albums checked
    bool                                    wholeAlbums                 = false;

    /// Processing power
    bool                                    useFullCpu                  = false;

    /// detection Model
    FaceDetectionModel                      detectModel                 = FaceDetectionModel::YuNet;

    /// detection Model
    FaceDetectionSize                       detectSize                  = FaceDetectionSize::Medium;

    /// Detection accuracy
    double                                  detectAccuracy              = 0.6;

    /// detection Model
    FaceRecognitionModel                    recognizeModel              = FaceRecognitionModel::OpenFace;

    /// Detection accuracy
    double                                  recognizeAccuracy           = 0.7;

    /// Albums to scan
    AlbumList                               albums;

    /// Image infos to scan
    ItemInfoList                            infos;

    ScanTask                                task                        = Detect;

    AlreadyScannedHandling                  alreadyScannedHandling      = Skip;
};

} // namespace Digikam
