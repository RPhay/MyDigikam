/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Writer of focus points to exiftool data
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

#include "focuspoints_writer.h"

// Qt includes

#include <QStringList>
#include <QScopedPointer>

// Local includes

#include "exiftoolparser.h"
#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN FocusPointsWriter::Private
{
public:

    explicit Private()
      : exifTool(nullptr)
    {
    };

    ExifToolParser* exifTool;
    QString         path;
};

FocusPointsWriter::FocusPointsWriter(QObject* const parent, const QString& path)
    : QObject(parent),
      d      (new Private)
{
    d->exifTool = new ExifToolParser(this);
    d->exifTool->load(path);
    d->path     = path;
}

FocusPointsWriter::~FocusPointsWriter()
{
    delete d;
}

void FocusPointsWriter::writeFocusPoint(const FocusPoint& point)
{
    ExifToolParser::ExifToolData newTags;

    QPointF pos = point.getCenterPosition();
    QSizeF size = point.getSize();

    newTags.insert(QLatin1String("xmp:RegionAreaX"),
                   QVariantList() << QString::number(pos.x())
                                  << QString()
                                  << QString::fromUtf8("Region Area X"));

    newTags.insert(QLatin1String("xmp:RegionAreaY"),
                   QVariantList() << QString::number(pos.y())
                                  << QString()
                                  << QString::fromUtf8("Region Area Y"));

    newTags.insert(QLatin1String("xmp:RegionAreaH"),
                   QVariantList() << QString::number(size.height())
                                  << QString()
                                  << QString::fromUtf8("Region Area Height"));

    newTags.insert(QLatin1String("xmp:RegionAreaW"),
                   QVariantList() << QString::number(size.width())
                                  << QString()
                                  << QString::fromUtf8("Region Area Width"));

    newTags.insert(QLatin1String("xmp:RegionDescription"),
                   QVariantList() << QString::fromUtf8("digikam customized auto focus point")
                                  << QString()
                                  << QString::fromUtf8("Region description"));

    d->exifTool->applyChanges(d->path, newTags);
}

void FocusPointsWriter::writeFocusPoint(const QRectF& rectF)
{
    FocusPoint point
    {
        static_cast<float> (rectF.topLeft().x() + 0.5 * rectF.width()),
        static_cast<float> (rectF.topLeft().y() + 0.5 * rectF.height()),
        static_cast<float> (rectF.width()), static_cast<float>(rectF.height())
    };

    writeFocusPoint(point);
}

} // namespace Digikam
