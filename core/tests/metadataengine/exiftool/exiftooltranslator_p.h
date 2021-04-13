/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-04-13
 * Description : ExifTool tags translator for Exiv2
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

#ifndef DIGIKAM_EXIF_TOOL_TRANSLATOR_P_H
#define DIGIKAM_EXIF_TOOL_TRANSLATOR_P_H

#include "exiftooltranslator.h"

// Qt includes

#include <QHash>
#include <QStringList>

namespace Digikam
{

class Q_DECL_HIDDEN ExifToolTranslator::Private
{
public:

    explicit Private()
    {
        populateExifHashTable();
        populateIptcHashTable();
        populateXmpHashTable();

        // --- Create the inverse map of tag names

        QHash<QString, QString>::const_iterator it = mapETtoExiv2.constBegin();

        while (it != mapETtoExiv2.constEnd())
        {
            mapExiv2toET.insert(it.value(), it.key());
            ++it;
        }

        // --- Init list of ignored groups from ExifTool.

        ignoreGroupsET
                       << QLatin1String("...")                          // Internal info from ExifTool
                       << QLatin1String("ExifTool.ExifTool.ExifTool.")  // Internal info from ExifTool
                       << QLatin1String("File.System.")                 // Info from file system
                       << QLatin1String("File.File.")                   // Info from file system
                       << QLatin1String("MakerNotes.")                  // Exif Maker-note sections
                       << QLatin1String("JFIF.JFIF.")                   // JFIF section                         (do not exist yet in Exiv2)
                       << QLatin1String("ICC_Profile.")                 // Icc profile section                  (do not exist yet in Exiv2)
                       << QLatin1String("Composite.")                   // Exif extended section ?              (do not exist yet in Exiv2)
                       << QLatin1String("APP14.")                       // JPEG APP14 section                   (do not exist yet in Exiv2)
                       << QLatin1String("PrintIM.")                     // JPEG APP6::EPPIM section             (do not exist yet in Exiv2)
                       << QLatin1String("Photoshop.")                   // JPEG APP13::PhotoshopIRB section     (do not exist yet in Exiv2)
                       << QLatin1String("FlashPix.")                    // JPEG APP2::FPXR section              (do not exist yet in Exiv2)
                       << QLatin1String("MPF.")                         // JPEG APP2::MPF section               (do not exist yet in Exiv2)
                      ;
    }

public:

    /**
     * Internal map to translate ExifTool Tag names to Exiv2 Tags
     * Key   = ExifTool tag name properties "group0.group1.group2.name"
     * value = Exiv2 tag name properties "family.group.name"
     */
    QHash<QString, QString> mapETtoExiv2;

    /**
     * Same than previous map but in inverse order.
     */
    QHash<QString, QString> mapExiv2toET;

    /**
     * List of groups to ignore from Exiftool
     */
    QStringList             ignoreGroupsET;

private:

    void populateExifHashTable();
    void populateIptcHashTable();
    void populateXmpHashTable();
};

} // namespace Digikam

#endif // DIGIKAM_EXIF_TOOL_TRANSLATOR_P_H_
