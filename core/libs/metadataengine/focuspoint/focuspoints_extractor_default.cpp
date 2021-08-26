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

// Internal function to create af point from meta data
namespace
{

FocusPoint create_af_point(float afPointWidth, float afPointHeight, 
                           float af_x_position, float af_y_position)
{    
    return FocusPoint(af_x_position, af_y_position, 
                      afPointWidth, afPointHeight,
                      FocusPoint::TypePoint::SelectedInFocus);;
}

}

// Main function to extract af point
FocusPointsExtractor::ListAFPoints FocusPointsExtractor::getAFPoints_default() const
{
    setAFPointsReadOnly(false);
    
    QString TagNameRoot = QLatin1String("XMP.XMP-mwg-rs.Image");

    QString desc = findValue(TagNameRoot, QLatin1String("RegionDescription")).toString();

    if (!desc.startsWith(QLatin1String("digikam")))
    {   
        return ListAFPoints();
    }

    QVariant af_x_position = findValue(TagNameRoot, QLatin1String("RegionAreaX"));
    QVariant af_y_position = findValue(TagNameRoot, QLatin1String("RegionAreaY"));

    if (af_x_position.isNull() || af_y_position.isNull())
    {
        return ListAFPoints();
    }

    QVariant afPointWidth = findValue(TagNameRoot,  QLatin1String("RegionAreaW"));
    QVariant afPointHeight = findValue(TagNameRoot, QLatin1String("RegionAreaH"));

    if (afPointWidth.isNull() || afPointHeight.isNull())
    {
        return ListAFPoints();
    }

    return ListAFPoints{create_af_point(afPointWidth.toFloat(), afPointHeight.toFloat(),
                                        af_x_position.toFloat(), af_y_position.toFloat())};
}

}
