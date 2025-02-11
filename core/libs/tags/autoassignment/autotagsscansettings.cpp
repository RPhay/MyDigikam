/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2025-01-06
 * Description : autotags scanning settings container
 *
 * SPDX-FileCopyrightText: 2024-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "autotagsscansettings.h"

namespace Digikam
{

AutotagsScanSettings::AutotagsScanSettings(const AutotagsScanSettings& other)
    : scanMode                (other.scanMode),
      wholeAlbums             (other.wholeAlbums),
      tagMode                 (other.tagMode),
      useFullCpu              (other.useFullCpu),
      objectDetectModel       (other.objectDetectModel),
      imageClassificationModel(other.imageClassificationModel),
      albums                  (other.albums),
      uiConfidenceThreshold   (other.uiConfidenceThreshold),
      languages               (other.languages),
      bqmMode                 (other.bqmMode)
{
}

AutotagsScanSettings& AutotagsScanSettings::operator=(const AutotagsScanSettings& other)
{
    scanMode                 = other.scanMode;
    wholeAlbums              = other.wholeAlbums;
    tagMode                  = other.tagMode;
    useFullCpu               = other.useFullCpu;
    objectDetectModel        = other.objectDetectModel;
    imageClassificationModel = other.imageClassificationModel;
    albums                   = other.albums;
    uiConfidenceThreshold    = other.uiConfidenceThreshold;
    languages                = other.languages;
    bqmMode                  = other.bqmMode;

    return *this;
}

QDebug operator<<(QDebug dbg, const AutotagsScanSettings& s)
{
    dbg.nospace()                                                                << Qt::endl;
    dbg.nospace() << "Scan Mode                 :" << s.scanMode                 << Qt::endl;
    dbg.nospace() << "Whole Albums              :" << s.wholeAlbums              << Qt::endl;
    dbg.nospace() << "Tag Mode                  :" << s.tagMode                  << Qt::endl;
    dbg.nospace() << "Use Full CPU              :" << s.useFullCpu               << Qt::endl;
    dbg.nospace() << "Object Detect Model       :" << s.objectDetectModel        << Qt::endl;
    dbg.nospace() << "Image Classification Model:" << s.imageClassificationModel << Qt::endl;
    dbg.nospace() << "Albums                    :" << s.albums                   << Qt::endl;
    dbg.nospace() << "Confidence Threshold      :" << s.uiConfidenceThreshold    << Qt::endl;
    dbg.nospace() << "Languages                 :" << s.languages                << Qt::endl;
    dbg.nospace() << "Bqm Mode                  :" << s.bqmMode                  << Qt::endl;

    return dbg.space();
}

} // namespace Digikam

#include "moc_autotagsscansettings.cpp"
