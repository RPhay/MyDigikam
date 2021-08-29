/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Extraction of focus points by exiftool data
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

const float RATIO_POINT_IMAGE = 1 / 120; // this is a guess

// Internal function to create af point from meta data
namespace PanasonicInternal
{

FocusPoint create_af_point(float af_x_position,
                           float af_y_position,
                           float afPointWidth,
                           float afPointHeight)
{
    return FocusPoint(af_x_position, af_y_position,
                      afPointWidth  * RATIO_POINT_IMAGE,
                      afPointHeight * RATIO_POINT_IMAGE,
                      FocusPoint::TypePoint::SelectedInFocus);
}

} // namespace PanasonicInternal

FocusPointsExtractor::ListAFPoints FocusPointsExtractor::getAFPoints_panasonic() const
{
    QString TagNameRoot     = QLatin1String("MakerNotes.Panasonic.Camera");

    // Get size image

    QVariant imageWidth     = findValue(QLatin1String("File.File.Image.ImageWidth"));
    QVariant imageHeight    = findValue(QLatin1String("File.File.Image.ImageHeight"));

    if (imageWidth.isNull() || imageHeight.isNull())
    {
        return getAFPoints_default();
    }

    // Get af point

    QStringList af_position = findValue(TagNameRoot,QLatin1String("AFPointPosition")).toString().split(QLatin1String(" "));

    if (af_position.isEmpty() || (af_position.count() == 1))
    {
        return getAFPoints_default();
    }

    float af_x_position = af_position[0].toFloat();
    float af_y_position = af_position[1].toFloat();

    // Get size of af points

    float afPointWidth  = imageWidth.toFloat()  * RATIO_POINT_IMAGE;
    float afPointHeight = imageHeight.toFloat() * RATIO_POINT_IMAGE;

    // Add point

    return ListAFPoints{PanasonicInternal::create_af_point(af_x_position, af_y_position,
                                                           afPointWidth, afPointHeight)};
}

} // namespace Digikam
