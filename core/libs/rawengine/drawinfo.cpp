/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-05-02
 * Description : RAW file identification information container
 *
 * Copyright (C) 2007-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Local includes

#include "drawinfo.h"

namespace Digikam
{

DRawInfo::DRawInfo()
{
    sensitivity             = -1.0F;
    exposureTime            = -1.0F;
    aperture                = -1.0F;
    focalLength             = -1.0F;
    pixelAspectRatio        = 1.0F;          ///< Default value. This can be unavailable (depending of camera model).
    baselineExposure        = -999.0F;       ///< -999 is an invalid exposure.

    ambientTemperature      = -1000.0F;      ///< -1000 is an invalid temperature.
    ambientHumidity         = -1000.0F;      ///< -1000 is an invalid humidity.
    ambientPressure         = -1000.0F;      ///< -1000 is an invalid pressure.
    ambientWaterDepth       = 1000.0F;       ///<  1000 is an invalid water depth.
    ambientAcceleration     = -1000.0F;      ///< -1000 is an invalid acceleration.
    ambientElevationAngle   = -1000.0F;      ///< -1000 is an invalid angle.

    exposureIndex           = -1.0F;         ///< Valid value is unsigned.
    flashUsed               = -1;            ///< Valid value is unsigned.
    meteringMode            = -1;            ///< Valid value is unsigned.
    exposureProgram         = -1;            ///< Valid value is unsigned.

    rawColors               = -1;
    rawImages               = -1;
    hasIccProfile           = false;
    isDecodable             = false;
    daylightMult[0]         = 0.0F;
    daylightMult[1]         = 0.0F;
    daylightMult[2]         = 0.0F;
    cameraMult[0]           = 0.0F;
    cameraMult[1]           = 0.0F;
    cameraMult[2]           = 0.0F;
    cameraMult[3]           = 0.0F;
    blackPoint              = 0;

    for (int ch = 0 ; ch < 4 ; ++ch)
    {
        blackPointCh[ch] = 0;
    }

    whitePoint              = 0;
    serialNumber            = 0;
    topMargin               = 0;
    leftMargin              = 0;
    orientation             = ORIENTATION_NONE;

    for (int x = 0 ; x < 3 ; ++x)
    {
        for (int y = 0 ; y < 4 ; ++y)
        {
            cameraColorMatrix1[x][y] = 0.0F;
            cameraColorMatrix2[x][y] = 0.0F;
            cameraXYZMatrix[y][x]    = 0.0F; ///< NOTE: see B.K.O # 253911 : [y][x] not [x][y]
        }
    }

    latitude                = 0.0F;
    longitude               = 0.0F;
    altitude                = 0.0F;
    hasGpsInfo              = false;

    focalLengthIn35mmFilm   = -1;            ///< Valid value is unsigned.
    maxAperture             = -1.0F;         ///< Valid value is unsigned.
}

DRawInfo::~DRawInfo()
{
}

QDebug operator<<(QDebug dbg, const DRawInfo& c)
{
    dbg.nospace() << "DRawInfo::sensitivity: "      << c.sensitivity      << ", ";
    dbg.nospace() << "DRawInfo::exposureTime: "     << c.exposureTime     << ", ";
    dbg.nospace() << "DRawInfo::aperture: "         << c.aperture         << ", ";
    dbg.nospace() << "DRawInfo::focalLength: "      << c.focalLength      << ", ";
    dbg.nospace() << "DRawInfo::pixelAspectRatio: " << c.pixelAspectRatio << ", ";
    dbg.nospace() << "DRawInfo::rawColors: "        << c.rawColors        << ", ";
    dbg.nospace() << "DRawInfo::rawImages: "        << c.rawImages        << ", ";
    dbg.nospace() << "DRawInfo::hasIccProfile: "    << c.hasIccProfile    << ", ";
    dbg.nospace() << "DRawInfo::isDecodable: "      << c.isDecodable      << ", ";
    dbg.nospace() << "DRawInfo::daylightMult: "     << c.daylightMult     << ", ";
    dbg.nospace() << "DRawInfo::cameraMult: "       << c.cameraMult       << ", ";
    dbg.nospace() << "DRawInfo::blackPoint: "       << c.blackPoint       << ", ";
    dbg.nospace() << "DRawInfo::whitePoint: "       << c.whitePoint       << ", ";
    dbg.nospace() << "DRawInfo::topMargin: "        << c.topMargin        << ", ";
    dbg.nospace() << "DRawInfo::leftMargin: "       << c.leftMargin       << ", ";
    dbg.nospace() << "DRawInfo::orientation: "      << c.orientation;

    return dbg.space();
}

} // namespace Digikam
