/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Extraction of focus points by exiftool data - XMP metadata
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>

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
namespace XmpInternal
{

FocusPoint create_af_point(float afPointWidth,
                           float afPointHeight,
                           float af_x_position,
                           float af_y_position)
{
    return FocusPoint(af_x_position,
                      af_y_position,
                      afPointWidth,
                      afPointHeight,
                      FocusPoint::TypePoint::SelectedInFocus);
}

} // namespace XmpInternal

// Main function to extract af point
FocusPointsExtractor::ListAFPoints FocusPointsExtractor::getAFPoints_xmp() const
{
    setAFPointsReadOnly(false);

    QString TagNameRoot    = QLatin1String("XMP.XMP-mwg-rs.Image");
    QString desc           = findValue(TagNameRoot, QLatin1String("RegionDescription")).toString();

    if (!desc.startsWith(QLatin1String("digikam")))
    {
        qCDebug(DIGIKAM_METAENGINE_LOG) << "FocusPointsExtractor: cannot find digiKam XMP namespace.";

        return ListAFPoints();
    }

    QVariant af_x_position = findValue(TagNameRoot, QLatin1String("RegionAreaX"));
    QVariant af_y_position = findValue(TagNameRoot, QLatin1String("RegionAreaY"));

    if (af_x_position.isNull() || af_y_position.isNull())
    {
        qCDebug(DIGIKAM_METAENGINE_LOG) << "FocusPointsExtractor: invalid positions from XMP.";

        return ListAFPoints();
    }

    QVariant afPointWidth  = findValue(TagNameRoot, QLatin1String("RegionAreaW"));
    QVariant afPointHeight = findValue(TagNameRoot, QLatin1String("RegionAreaH"));

    if (afPointWidth.isNull() || afPointHeight.isNull())
    {
        qCDebug(DIGIKAM_METAENGINE_LOG) << "FocusPointsExtractor: invalid sizes from XMP.";

        return ListAFPoints();
    }

    qCDebug(DIGIKAM_METAENGINE_LOG) << "FocusPointsExtractor: XMP Focus Location:" << afPointWidth << afPointHeight
                                                                                   << af_x_position << af_y_position;

    return
    (
        ListAFPoints
        {
            XmpInternal::create_af_point(
                                         afPointWidth.toFloat(),
                                         afPointHeight.toFloat(),
                                         af_x_position.toFloat(),
                                         af_y_position.toFloat()
                                        )
        }
    );
}

} // namespace Digikam
