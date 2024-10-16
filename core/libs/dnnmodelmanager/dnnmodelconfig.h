/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-10-13
 * Description : digiKam DNNModelConfig for generic OpenCV config
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
#include "dnnmodelbase.h"
#include "dnnmodelmanager.h"

namespace Digikam
{

class DIGIKAM_EXPORT DNNModelConfig: public DNNModelBase
{

public:

    // ---------- public methods ----------

    explicit DNNModelConfig(
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
    virtual ~DNNModelConfig() override  = default;

/*
    // input: uiThreshold is the slider value from the UI
    // return: float threshold to be used by processing (FaceDetector, FaceRecognizer, etc...)
    float processingThreshold(int uiThreshold);
*/

public:

    // ---------- public members ----------
/*
    const QString      getConfig();
*/
private:

    DNNModelConfig()                    = delete;

    virtual bool loadModel() override;                  ///< must be overridden in child class
};

} // namespace Digikam
