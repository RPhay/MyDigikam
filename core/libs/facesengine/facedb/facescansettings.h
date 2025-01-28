/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-10-09
 * Description : Face scan settings container.
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

class DIGIKAM_GUI_EXPORT FaceScanSettings
{
    Q_GADGET

public:

    /**
     * @brief Different possible tasks processed while scanning operation.
     */
    enum ScanTask
    {
        // Detect,              ///< Detect faces only.
        DetectAndRecognize,     ///< Detect and recognize faces only.
        RecognizeMarkedFaces,   ///< Recognize already marked faces only.
        RetrainAll,             ///< Retrain faces only.
        Reset
        // BenchmarkDetection,     ///< Bench performances of detect faces.
        // BenchmarkRecognition    ///< Bench performance of recognize faces.
    };
    Q_ENUM(ScanTask)

    /**
     * @brief To detect and recognize.
     */
    enum AlreadyScannedHandling
    {
        Skip,                   ///< Skip faces from images already scanned.
     // Merge,                  ///< Merge faces from images already scanned.
        Rescan,                 ///< Rescan faces from images already scanned.
        ClearAll,               ///< Clear all faces data from images already scanned. Clear identities and training data from FacesDb
        RecognizeOnly           ///< Recognize faces from images already scanned.
    };
    Q_ENUM(AlreadyScannedHandling)

    /**
     * @brief Face detection AI models.
     */
    enum FaceDetectionModel
    {
        SSDMOBILENET,           ///< SSD MobileNet neural network inference [https://github.com/arunponnusamy/cvlib]
        YOLOv3,                 ///< YOLO neural network inference          [https://github.com/sthanhng/yoloface]
        YuNet                   ///< YuNet neural network inference         [https://github.com/opencv/opencv_zoo/tree/main]
    };
    Q_ENUM(FaceDetectionModel)

    /**
     * @brief Face detection size.
     */
    enum FaceDetectionSize
    {
        ExtraSmall,
        Small,
        Medium,
        Large,
        ExtraLarge
    };
    Q_ENUM(FaceDetectionSize)

    /**
     * @brief Face recognition AI models.
     */
    enum FaceRecognitionModel
    {
        OpenFace,               ///< OpenFace pre-trained neural network model [https://github.com/sahilshah/openface/tree/master]
        SFace                   ///< SFace pre-trained neural network model    [https://github.com/opencv/opencv_zoo/blob/main/models/face_recognition_sface/]
    };
    Q_ENUM(FaceRecognitionModel)

public:

    FaceScanSettings();
    ~FaceScanSettings();

public:

    /// Whole albums checked.
    bool                                    wholeAlbums                 = false;

    /// Processing power.
    bool                                    useFullCpu                  = false;

    /// Detection Model.
    FaceDetectionModel                      detectModel                 = FaceDetectionModel::YuNet;

    /// Detection Model.
    FaceDetectionSize                       detectSize                  = FaceDetectionSize::Large;

    /// Detection accuracy.
    int                                     detectAccuracy              = DNN_MODEL_THRESHOLD_NOT_SET;      ///< use default value from dnnmodels.conf

    /// Detection Model.
    FaceRecognitionModel                    recognizeModel              = FaceRecognitionModel::SFace;

    /// Detection accuracy.
    int                                     recognizeAccuracy           = DNN_MODEL_THRESHOLD_NOT_SET;      ///< use default value from dnnmodels.conf

    /// Albums to scan.
    AlbumList                               albums;

    /// Image infos to scan.
    ItemInfoList                            infos;

    ScanTask                                task                        = DetectAndRecognize;

    AlreadyScannedHandling                  alreadyScannedHandling      = Skip;
};

} // namespace Digikam
