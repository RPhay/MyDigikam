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

bool DNNModelInfoContainer::operator==(const DNNModelInfoContainer& t) const
{
    bool b = true;
    b  &= (displayName         == t.displayName);
    b  &= (fileName            == t.fileName);
    b  &= (usage               == t.usage);
    b  &= (minVersion          == t.minVersion);
    b  &= (downloadPath        == t.downloadPath);
    b  &= (sha256              == t.sha256);
    b  &= (configName          == t.configName);
    b  &= (fileSize            == t.fileSize);
    b  &= (minUsableThreshold  == t.minUsableThreshold);
    b  &= (loaderType          == t.loaderType);
    b  &= (imageSize           == t.imageSize);

    return b;
}


} // namespace Digikam
