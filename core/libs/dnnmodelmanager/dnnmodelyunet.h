/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-10-13
 * Description : digiKam DNNModelYuNet specific for create FaceDetectorYN instances
 *
 * SPDX-FileCopyrightText: 2024 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QMutex>

// Local includes

#include "digikam_opencv.h"
#include "digikam_export.h"
#include "dnnmodelmanager.h"

namespace Digikam
{

class DIGIKAM_EXPORT DNNModelYuNet: public DNNModelBase
{

public:
    // ---------- public methods ----------
    DNNModelYuNet(
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
    virtual ~DNNModelYuNet()  override;

public:
    // ---------- public members ----------
    cv::Ptr<cv::FaceDetectorYN>&    getNet();

private:

    cv::Ptr<cv::FaceDetectorYN> net;

private:
    DNNModelYuNet()               = delete;

    virtual bool loadModel() override;                  // must be overridden in child class
    bool callLoader();

};

} // namespace Digikam
