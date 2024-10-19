/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-10-09
 * Description : Face scan settings container.
 *
 * SPDX-FileCopyrightText: 2010-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024      by Michael Miller <michael underscore miller at msn dot com>
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

class DIGIKAM_GUI_EXPORT FaceScanSettings
{
    Q_GADGET

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
    Q_ENUM(ScanTask)

    /// To detect and recognize
    enum AlreadyScannedHandling
    {
        Skip,
        Merge,
        Rescan,
        ClearAll
    };
    Q_ENUM(AlreadyScannedHandling)

    /// face detection AI models
    enum FaceDetectionModel
    {
        SSDMOBILENET,
        YOLOv3,
        YuNet
    };
    Q_ENUM(FaceDetectionModel)

    /// Face detection size
    enum FaceDetectionSize
    {
        ExtraSmall,
        Small,
        Medium,
        Large,
        ExtraLarge
    };
    Q_ENUM(FaceDetectionSize)

    /// face recognition AI models
    enum FaceRecognitionModel
    {
        OpenFace,
        SFace
    };
    Q_ENUM(FaceRecognitionModel)

public:

    FaceScanSettings();
    ~FaceScanSettings();

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
    int                                     detectAccuracy              = DNN_MODEL_THRESHOLD_NOT_SET;      ///< use default value from dnnmodels.conf

    /// detection Model
    FaceRecognitionModel                    recognizeModel              = FaceRecognitionModel::OpenFace;

    /// Detection accuracy
    int                                     recognizeAccuracy           = DNN_MODEL_THRESHOLD_NOT_SET;      ///< use default value from dnnmodels.conf

    /// Albums to scan
    AlbumList                               albums;

    /// Image infos to scan
    ItemInfoList                            infos;

    ScanTask                                task                        = Detect;

    AlreadyScannedHandling                  alreadyScannedHandling      = Skip;
};

} // namespace Digikam
