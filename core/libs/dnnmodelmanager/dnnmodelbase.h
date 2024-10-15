/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-10-13
 * Description : digiKam DNNModelNet base class
 *
 * SPDX-FileCopyrightText: 2024 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QMutex>
#include <QVersionNumber>

// Local includes

#include "digikam_opencv.h"
#include "digikam_export.h"
#include "filesdownloader.h"
#include "dnnmodeldefinitions.h"

namespace Digikam
{

class DIGIKAM_EXPORT DNNModelBase
{

public:

    // ---------- public methods ----------

    DNNModelBase(
                 const QString&                 _displayName,
                 const QString&                 _fileName,
                 const DNNModelUsageList&       _usage,
                 const QVersionNumber&          _minVersion,
                 const QString&                 _downloadPath,
                 const QString&                 _sha256,
                 const qint64&                  _fileSize,
                 int                            _minUsableThreshold,
                 int                            _maxUsableThreshold,
                 DNNLoaderType                  _loaderType,
                 const QString&                 _configName,
                 const cv::Scalar&              _meanValToSubtract,
                 int                            _imageSize
                );
    virtual ~DNNModelBase() = default;

    /**
     * input:  uiThreshold is the slider value from the UI
     * return: float threshold to be used by processing (FaceDetector, FaceRecognizer, etc...)
     */
    float processingThreshold(int uiThreshold);

public:

    // ---------- public members ----------

    DownloadInfo                getDownloadInformation()    const;
    bool                        checkFilename()             const;
    const QString               getModelPath()              const;

public:

    bool                        modelLoaded         = false;    ///< check if the model has been loaded

    QString                     displayName;                    ///< name used for display in UI (QComboBox)
    QString                     fileName;                       ///< used by the downloader and model loader
    DNNModelUsageList           usage;                          ///< how the model can be used. | for more than one use. face_detection, face_recognition, weight, object_detection, etc...
    QVersionNumber              minVersion;                     ///< minimum version of digiKam needed to use this model
    QString                     downloadPath;                   ///< used by the downloader for the download path
    QString                     sha256;                         ///< SHA265 hash of the file for download
    QString                     configName;

    qint64                      fileSize            = 0;        ///< used by the downloader to verify size
    int                         minUsableThreshold  = 0;        ///< used to convert UI 1-10 slider to float for processing
    int                         maxUsableThreshold  = 0;        ///< used to convert UI 1-10 slider to float for processing
    DNNLoaderType               loaderType          = Net;      ///< Model loder type custom (YuNet/SFace), Caffe, Darknet, Torch, Tensorflow
    cv::Scalar                  meanValToSubtract;
    int                         imageSize           = 0;        ///< max dimension of a side of an image

    QMutex                      mutex;                          ///< mutex to sigle-thread model during critical processing functions

private:

    DNNModelBase()              = delete;
    virtual bool loadModel()    = 0;                            ///< must be overridden in child class
};

} // namespace Digikam
