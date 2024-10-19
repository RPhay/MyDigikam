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

#include "dnnmodelbase.h"

// Qt Includes

#include <QFileInfo>
#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"
#include "dnnmodelmanager.h"
#include "dnnmodelinfocontainer.h"

namespace Digikam
{

DownloadInfo DNNModelBase::getDownloadInformation() const
{
    return DownloadInfo(
                        info.downloadPath,
                        info.fileName,
                        info.sha256,
                        info.fileSize
                       );
}

bool DNNModelBase::checkFilename() const
{
    QFileInfo fileInfo(getModelPath());

    return (fileInfo.exists() && (fileInfo.size() == info.fileSize));
}

const QString DNNModelBase::getModelPath() const
{
    QString appPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                             QLatin1String("digikam/facesengine"),
                                             QStandardPaths::LocateDirectory);

    if (!appPath.isEmpty())
    {
        QString modelPath = appPath + QLatin1Char('/') + info.fileName;

        return modelPath;
    }

    qCCritical(DIGIKAM_DNNMODELMNGR_LOG) << "Cannot find DNN models path";

    return QString();
}

const QPair<int, int> DNNModelBase::getBackendAndTarget() const
{
    int backend_id       = cv::dnn::DNN_BACKEND_DEFAULT;
    int target_id        = cv::dnn::DNN_TARGET_CPU;

    const std::map<std::string, int> str2backend
    {
        { "default", cv::dnn::DNN_BACKEND_DEFAULT          },
        { "halide",  cv::dnn::DNN_BACKEND_HALIDE           },
        { "ie",      cv::dnn::DNN_BACKEND_INFERENCE_ENGINE },
        { "opencv",  cv::dnn::DNN_BACKEND_OPENCV           },
        { "cuda",    cv::dnn::DNN_BACKEND_CUDA             }
    };

    const std::map<std::string, int> str2target
    {
        { "cpu",         cv::dnn::DNN_TARGET_CPU           },
        { "opencl",      cv::dnn::DNN_TARGET_OPENCL        },
        { "myriad",      cv::dnn::DNN_TARGET_MYRIAD        },
        { "vulkan",      cv::dnn::DNN_TARGET_VULKAN        },
        { "opencl_fp16", cv::dnn::DNN_TARGET_OPENCL_FP16   },
        { "cuda",        cv::dnn::DNN_TARGET_CUDA          },
        { "cuda_fp16",   cv::dnn::DNN_TARGET_CUDA_FP16     }
    };

    // env vars for testing combinations

    QString cvBackend    = QString::fromLocal8Bit(qgetenv("DIGIKAM_DNN_BACKEND"));
    QString cvTarget     = QString::fromLocal8Bit(qgetenv("DIGIKAM_DNN_TARGET"));

    if (cvBackend.length() > 0)
    {
        try
        {
            backend_id = str2backend.at(cvBackend.toLower().toUtf8().data());
            qCDebug(DIGIKAM_DNNMODELMNGR_LOG) << "Using OpenCV backend:" << cvBackend;
        }
        catch (...)
        {
            qCWarning(DIGIKAM_DNNMODELMNGR_LOG) << "Invalid OpenCV backend:" << cvBackend;
        }
    }

    if (cvTarget.length() > 0)
    {
        try
        {
            target_id = str2target.at(cvTarget.toLower().toUtf8().data());
            qCDebug(DIGIKAM_DNNMODELMNGR_LOG) << "Using OpenCV target:" << cvTarget;
        }
        catch (...)
        {
            qCWarning(DIGIKAM_DNNMODELMNGR_LOG) << "Invalid OpenCV target:" << cvTarget;
        }
    }

    // TODO: query OpenCV for capabilities.  Return best match
    //
    //
    //

    // return the result

    return QPair<int, int>(backend_id, target_id);
}

float DNNModelBase::getThreshold(int uiThreshold) const
{
    if (1000 == uiThreshold)
    {
        return (float)info.baseThreshold / 100.0f;
    }
    else
    {
        float increment = (info.maxUsableThreshold - info.minUsableThreshold) / 10;

        return ((float)(uiThreshold) * increment) + (float)info.minUsableThreshold;
    }
}

} // namespace Digikam
