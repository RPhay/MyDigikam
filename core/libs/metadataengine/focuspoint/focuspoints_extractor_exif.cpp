/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Extraction of focus points by exiftool data - Exif metadata
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
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

#include "focuspoints_extractor.h"

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

// Internal function to create af point from meta data
namespace ExifInternal
{

FocusPoint create_af_point(float af_x_position,
                           float af_y_position,
                           float afPointWidth,
                           float afPointHeight
                           )
{
    return FocusPoint(af_x_position,
                      af_y_position,
                      afPointWidth,
                      afPointHeight,
                      FocusPoint::TypePoint::SelectedInFocus);
}

} // namespace ExifInternal

// Main function to extract af point
FocusPointsExtractor::ListAFPoints FocusPointsExtractor::getAFPoints_exif() const
{
    QStringList af_info = findValue(QLatin1String("EXIF.ExifIFD.Camera.SubjectArea")).toString()
                                                                                     .split(QLatin1String(" "));

    if (af_info.size() < 4)
    {
        qCDebug(DIGIKAM_METAENGINE_LOG) << "FocusPointsExtractor: Invalid Exif Subject Area.";

        // Fail-back to XMP metadata if exists.

        return getAFPoints_xmp();
    }

    qCDebug(DIGIKAM_METAENGINE_LOG) << "FocusPointsExtractor: Exif Subject Area:" << af_info;

    // Get coordinate of af points

    float af_x_position = af_info[0].toFloat();
    float af_y_position = af_info[1].toFloat();

    // Get size of af area

    float afPointWidth  = af_info[2].toFloat();
    float afPointHeight = af_info[3].toFloat();

    return
    (
        ListAFPoints
        {
            ExifInternal::create_af_point(
                                          af_x_position,
                                          af_y_position,
                                          afPointWidth,
                                          afPointHeight
                                         )
        }
    );
}

} // namespace Digikam
