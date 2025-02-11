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

// Qt includes

#include <QDebug>

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
    //  BenchmarkDetection,     ///< Bench performances of detect faces.
    //  BenchmarkRecognition    ///< Bench performance of recognize faces.
    };
    Q_ENUM(ScanTask)

    /**
     * @brief To detect and recognize.
     */
    enum AlreadyScannedHandling
    {
        Skip,                   ///< Skip faces from images already scanned.
    //  Merge,                  ///< Merge faces from images already scanned.
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
        /**
         * OpenFace pre-trained neural network model [https://github.com/sahilshah/openface/tree/master]
         */
        OpenFace,
        /**
         * SFace pre-trained neural network model    [https://github.com/opencv/opencv_zoo/blob/main/models/face_recognition_sface/]
         */
        SFace
    };
    Q_ENUM(FaceRecognitionModel)

public:

    FaceScanSettings()  = default;
    FaceScanSettings(const FaceScanSettings& other);
    ~FaceScanSettings() = default;

    FaceScanSettings& operator=(const FaceScanSettings& other);

public:

    /**
     * @brief Whole albums checked.
     */
    bool                                    wholeAlbums                 = false;

    /**
     * @brief Processing power.
     */
    bool                                    useFullCpu                  = false;

    /**
     * @brief Detection Model.
     */
    FaceDetectionModel                      detectModel                 = FaceDetectionModel::YuNet;

    /**
     * @brief Face size used by the detection Model.
     */
    FaceDetectionSize                       detectSize                  = FaceDetectionSize::Large;

    /**
     * @brief Detection accuracy.
     * @note Use default value from dnnmodels.conf
     */
    int                                     detectAccuracy              = DNN_MODEL_THRESHOLD_NOT_SET;

    /**
     * @brief Detection Model.
     */
    FaceRecognitionModel                    recognizeModel              = FaceRecognitionModel::SFace;

    /**
     * @brief Recognition accuracy.
     * @note Use default value from dnnmodels.conf
     */
    int                                     recognizeAccuracy           = DNN_MODEL_THRESHOLD_NOT_SET;

    /**
     * @brief Albums to scan.
     */
    AlbumList                               albums;

    /**
     * @brief Image infos to scan.
     */
    ItemInfoList                            infos;

    ScanTask                                task                        = DetectAndRecognize;

    AlreadyScannedHandling                  alreadyScannedHandling      = Skip;
};

//! qDebug() stream operator. Writes property @a s to the debug output in a nicely formatted way.
DIGIKAM_GUI_EXPORT QDebug operator<<(QDebug dbg, const FaceScanSettings& s);

} // namespace Digikam


