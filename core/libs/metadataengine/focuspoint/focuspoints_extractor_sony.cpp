/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        :
 * Description : Extraction of focus points by exiftool data
 *
 * Copyright (C) 2020-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes

#include <QDebug>

// Local includes

namespace Digikam
{

const float RATIO_POINT_IMAGE = 120; // this is a guess

// Internal function to create af point from meta data
namespace SonyInternal
{

FocusPoint create_af_point(float imageWidth, float imageHeight,
                                                 float afPointWidth, float afPointHeight,
                                                 float af_x_position, float af_y_position)
{
    return FocusPoint(af_x_position / imageWidth,
                      af_y_position / imageHeight,
                      afPointWidth, afPointHeight,
                      FocusPoint::TypePoint::SelectedInFocus);
}

}

// Main function to extract af point
FocusPointsExtractor::ListAFPoints FocusPointsExtractor::getAFPoints_sony() const
{
    QString TagNameRoot = QLatin1String("MakerNotes.Sony.Camera");

    QStringList af_info = findValue(TagNameRoot,QLatin1String("FocusLocation")).toString()
                                                                               .split(QLatin1String(" "));

    if (af_info.isEmpty())
    {
        return getAFPoints_default();
    }

    // Get size image
    float imageWidth  = af_info[0].toFloat();
    float imageHeight = af_info[1].toFloat();

    // Get size of af points

    float afPointWidth = imageWidth * RATIO_POINT_IMAGE;
    float afPointHeight = imageHeight * RATIO_POINT_IMAGE;

    // Get coordinate of af points
    float af_x_position = af_info[3].toFloat();
    float af_y_position = af_info[4].toFloat();

    return ListAFPoints{SonyInternal::create_af_point(imageWidth, imageHeight,
                                                      afPointWidth, afPointHeight,
                                                      af_x_position, af_y_position)};
}

}
