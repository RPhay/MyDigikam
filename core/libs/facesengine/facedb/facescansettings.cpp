/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-10-09
 * Description : Face scan settings contaner.
 *
 * SPDX-FileCopyrightText: 2010-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "facescansettings.h"

namespace Digikam
{

FaceScanSettings::FaceScanSettings(const FaceScanSettings& other)
    : wholeAlbums           (other.wholeAlbums),
      useFullCpu            (other.useFullCpu),
      detectModel           (other.detectModel),
      detectSize            (other.detectSize),
      detectAccuracy        (other.detectAccuracy),
      recognizeModel        (other.recognizeModel),
      recognizeAccuracy     (other.recognizeAccuracy),
      albums                (other.albums),
      infos                 (other.infos),
      task                  (other.task),
      alreadyScannedHandling(other.alreadyScannedHandling)
{
}

FaceScanSettings& FaceScanSettings::operator=(const FaceScanSettings& other)
{
    wholeAlbums            = other.wholeAlbums;
    useFullCpu             = other.useFullCpu;
    detectModel            = other.detectModel;
    detectSize             = other.detectSize;
    detectAccuracy         = other.detectAccuracy;
    recognizeModel         = other.recognizeModel;
    recognizeAccuracy      = other.recognizeAccuracy;
    albums                 = other.albums;
    infos                  = other.infos;
    task                   = other.task;
    alreadyScannedHandling = other.alreadyScannedHandling;

    return *this;
}

QDebug operator<<(QDebug dbg, const FaceScanSettings& s)
{
    dbg.nospace()                                                            << Qt::endl;
    dbg.nospace() << "Whole Albums            :" << s.wholeAlbums            << Qt::endl;
    dbg.nospace() << "Use Full CPU            :" << s.useFullCpu             << Qt::endl;
    dbg.nospace() << "Detection Model         :" << s.detectModel            << Qt::endl;
    dbg.nospace() << "Detection Size          :" << s.detectSize             << Qt::endl;
    dbg.nospace() << "Detection Accuracy      :" << s.detectAccuracy         << Qt::endl;
    dbg.nospace() << "Recognition Model       :" << s.recognizeModel         << Qt::endl;
    dbg.nospace() << "Recognition Accuracy    :" << s.recognizeAccuracy      << Qt::endl;
    dbg.nospace() << "Albums                  :" << s.albums                 << Qt::endl;
    dbg.nospace() << "Image Infos             :" << s.infos                  << Qt::endl;
    dbg.nospace() << "Scan Task               :" << s.task                   << Qt::endl;
    dbg.nospace() << "Already Scanned Handling:" << s.alreadyScannedHandling << Qt::endl;

    return dbg.space();
}

} // namespace Digikam

#include "moc_facescansettings.cpp"
