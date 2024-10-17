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

#include "dnnmodelinfocontainer.h"

// Qt Includes

#include <QFileInfo>
#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"
#include "dnnmodelmanager.h"

namespace Digikam
{

DNNModelInfoContainer::DNNModelInfoContainer(const DNNModelInfoContainer& o)
    : displayName       (o.displayName),
      fileName          (o.fileName),
      usage             (o.usage),
      minVersion        (o.minVersion),
      downloadPath      (o.downloadPath),
      sha256            (o.sha256),
      configName        (o.configName),
      fileSize          (o.fileSize),
      minUsableThreshold(o.minUsableThreshold),
      maxUsableThreshold(o.maxUsableThreshold),
      loaderType        (o.loaderType),
      meanValToSubtract (o.meanValToSubtract),
      imageSize         (o.imageSize)
{
}

DNNModelInfoContainer& DNNModelInfoContainer::operator=(const DNNModelInfoContainer& o)
{
    displayName        = o.displayName;
    fileName           = o.fileName;
    usage              = o.usage;
    minVersion         = o.minVersion;
    downloadPath       = o.downloadPath;
    sha256             = o.sha256;
    configName         = o.configName;
    fileSize           = o.fileSize;
    minUsableThreshold = o.minUsableThreshold;
    maxUsableThreshold = o.maxUsableThreshold;
    loaderType         = o.loaderType;
    meanValToSubtract  = o.meanValToSubtract;
    imageSize          = o.imageSize;

    return *this;
}

DNNModelInfoContainer& DNNModelInfoContainer::operator=(DNNModelInfoContainer&& o)
{
    displayName        = o.displayName;
    fileName           = o.fileName;
    usage              = o.usage;
    minVersion         = o.minVersion;
    downloadPath       = o.downloadPath;
    sha256             = o.sha256;
    configName         = o.configName;
    fileSize           = o.fileSize;
    minUsableThreshold = o.minUsableThreshold;
    maxUsableThreshold = o.maxUsableThreshold;
    loaderType         = o.loaderType;
    meanValToSubtract  = o.meanValToSubtract;
    imageSize          = o.imageSize;

    return *this;
}

DNNModelInfoContainer::DNNModelInfoContainer(
                                             const QString&           _displayName,
                                             const QString&           _fileName,
                                             const DNNModelUsageList& _usage,
                                             const QVersionNumber&    _minVersion,
                                             const QString&           _downloadPath,
                                             const QString&           _sha256,
                                             const qint64&            _fileSize,
                                             int                      _minUsableThreshold,
                                             int                      _maxUsableThreshold,
                                             DNNLoaderType            _loaderType,
                                             const QString&           _configName,
                                             const cv::Scalar&        _meanValToSubtract,
                                             int                      _imageSize
                                            )
    : displayName       (_displayName),
      fileName          (_fileName),
      usage             (_usage),
      minVersion        (_minVersion),
      downloadPath      (_downloadPath),
      sha256            (_sha256),
      configName        (_configName),
      fileSize          (_fileSize),
      minUsableThreshold(_minUsableThreshold),
      maxUsableThreshold(_maxUsableThreshold),
      loaderType        (_loaderType),
      meanValToSubtract (_meanValToSubtract),
      imageSize         (_imageSize)
{
}

bool DNNModelInfoContainer::operator==(const DNNModelInfoContainer& o) const
{
    bool b = true;
    b     &= (displayName         == o.displayName);
    b     &= (fileName            == o.fileName);
    b     &= (usage               == o.usage);
    b     &= (minVersion          == o.minVersion);
    b     &= (downloadPath        == o.downloadPath);
    b     &= (sha256              == o.sha256);
    b     &= (configName          == o.configName);
    b     &= (fileSize            == o.fileSize);
    b     &= (minUsableThreshold  == o.minUsableThreshold);
    b     &= (loaderType          == o.loaderType);
    b     &= (imageSize           == o.imageSize);

    return b;
}

} // namespace Digikam
