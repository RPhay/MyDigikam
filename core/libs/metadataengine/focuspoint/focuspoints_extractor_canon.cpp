/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Extraction of focus points by exiftool data - Canon devices
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

namespace CanonInternal
{

void set_point_position(FocusPoint& point,
                        float imageWidth,
                        float imageHeight,
                        float af_x_position,
                        float af_y_position,
                        int yDirection)
{
    point.setCenterPosition(0.5 + af_x_position              / imageWidth,
                            0.5 + af_y_position * yDirection / imageHeight);
}

void set_point_size(FocusPoint& point,
                    float imageWidth,
                    float imageHeight,
                    float afPointWidth,
                    float afPointHeight)
{
    point.setSize(afPointWidth / imageWidth, afPointHeight / imageHeight);
}

void set_point_type(FocusPoint& point,
                    const QStringList& af_selected,
                    const QStringList& af_infocus,
                    int index)
{
    point.setType(FocusPoint::TypePoint::Inactive);

    if (af_selected.contains(QString::number(index + 1)))
    {
        point.setType( FocusPoint::TypePoint::Selected);
    }

    if (af_infocus.contains(QString::number(index + 1)))
    {
        point.setType(point.getType() | FocusPoint::TypePoint::Selected);
    }
}

FocusPoint create_af_point(float imageWidth,
                           float imageHeight,
                           float afPointWidth,
                           float afPointHeight,
                           float af_x_position,
                           float af_y_position,
                           const QStringList& af_selected,
                           const QStringList& af_infocus,
                           int   yDirection,
                           int   index)
{
    FocusPoint point;

    set_point_position(point,
                       imageWidth,
                       imageHeight,
                       af_x_position,
                       af_y_position,
                       yDirection);

    set_point_size(point,
                   imageWidth,
                   imageHeight,
                   afPointWidth,
                   afPointHeight);

    set_point_type(point,
                   af_selected,
                   af_infocus,
                   index);

    return point;
}

} // namespace CanonInternal

// Main function to extract af point
FocusPointsExtractor::ListAFPoints FocusPointsExtractor::getAFPoints_canon() const
{
    QString TagNameRoot = QLatin1String("MakerNotes.Canon.Camera");

    // Get size image

    QString model       = findValue(QLatin1String("EXIF.IFD0.Camera.Make")).toString();

    QVariant imageWidth, imageHeight;

    if (model.toLower() == QLatin1String("canon eos 5d"))
    {
        imageWidth  = findValueFirstMatch(QStringList()
                                          << QLatin1String("MakerNotes.Canon.Image.CanonImageWidth")
                                          << QLatin1String("EXIF.ExifIFD.Image.ExifImageWidth")
        );

        imageHeight = findValueFirstMatch(QStringList()
                                          << QLatin1String("MakerNotes.Canon.Image.CanonImageHeight")
                                          << QLatin1String("EXIF.ExifIFD.Image.ExifImageHeight")
        );
    }
    else
    {
        imageWidth = findValueFirstMatch(QStringList()
                                         << QLatin1String("MakerNotes.Canon.Camera.AFImageWidth")
                                         << QLatin1String("EXIF.ExifIFD.Image.ExifImageWidth")
        );

        imageHeight = findValueFirstMatch(QStringList()
                                          << QLatin1String("MakerNotes.Canon.Camera.AFImageHeight")
                                          << QLatin1String("EXIF.ExifIFD.Image.ExifImageHeight")
        );
    }

    if (imageWidth.isNull() || imageHeight.isNull())
    {
        qCDebug(DIGIKAM_METAENGINE_LOG) << "FocusPointsExtractor: invalid Canon Camera image sizes.";

        return getAFPoints_exif();
    }

    setOriginalSize(QSize(imageWidth.toInt(), imageHeight.toInt()));

    // Get size of af points

    QVariant afPointWidth      = findValue(TagNameRoot,QLatin1String("AFAreaWidth"));
    QVariant afPointHeight     = findValue(TagNameRoot,QLatin1String("AFAreaHeight"));
    QStringList afPointWidths  = findValue(TagNameRoot,QLatin1String("AFAreaWidths"),  true).toStringList();
    QStringList afPointHeights = findValue(TagNameRoot,QLatin1String("AFAreaHeights"), true).toStringList();

    if (((afPointWidth.isNull())   || (afPointHeight.isNull())) &&
        ((afPointWidths.isEmpty()) || (afPointHeights.isEmpty())))
    {
        qCDebug(DIGIKAM_METAENGINE_LOG) << "FocusPointsExtractor: invalid sizes from Canon makernotes.";

        return getAFPoints_exif();
    }

    // Get coordinate of af points

    QStringList af_x_positions = findValue(TagNameRoot, QLatin1String("AFAreaXPositions"), true).toStringList();
    QStringList af_y_positions = findValue(TagNameRoot, QLatin1String("AFAreaYPositions"), true).toStringList();

    if (af_x_positions.isEmpty() || af_y_positions.isEmpty())
    {
        qCDebug(DIGIKAM_METAENGINE_LOG) << "FocusPointsExtractor: invalid positions from Canon makernotes.";

        return getAFPoints_exif();
    }

    qCDebug(DIGIKAM_METAENGINE_LOG) << "FocusPointsExtractor: Canon Makernotes Focus Location:" << af_x_positions
                                                                                                << af_y_positions
                                                                                                << afPointWidth
                                                                                                << afPointHeights;

    // Get type of af points

    QStringList af_selected = findValueFirstMatch(TagNameRoot, QStringList() << QLatin1String("AFPointsSelected")
                                                                             << QLatin1String("AFPointsInFocus"),
                                                  true).toStringList();
    QStringList af_infocus  = findValue(TagNameRoot, QLatin1String("AFPointsInFocus"), true).toStringList();

    // Get direction

    QString cameraType      = findValue(TagNameRoot, QLatin1String("CameraType")).toString();

    int yDirection          = (cameraType.toUpper() == QLatin1String("COMPACT")) ? -1 : 1;

    ListAFPoints points;

    for (int i = 0 ; i < af_x_positions.count() ; ++i)
    {
        float afPointWidthUsed  = (afPointWidths.isEmpty()) ? afPointWidth.toFloat()
                                                            : afPointWidths[i].toFloat();

        float afPointHeightUsed = (afPointWidths.isEmpty()) ? afPointHeight.toFloat()
                                                            : afPointWidths[i].toFloat();

        FocusPoint point        = CanonInternal::create_af_point(
                                                                 imageWidth.toFloat(),
                                                                 imageHeight.toFloat(),
                                                                 afPointWidthUsed,
                                                                 afPointHeightUsed,
                                                                 af_x_positions[i].toFloat(),
                                                                 af_y_positions[i].toFloat(),
                                                                 af_selected,
                                                                 af_infocus,
                                                                 yDirection,
                                                                 i
                                                                );
        points.append(point);
    }

    return points;
}

} // namespace Digikam
