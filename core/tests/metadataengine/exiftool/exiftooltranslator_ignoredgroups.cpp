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

#include "exiftooltranslator_p.h"

namespace Digikam
{

void ExifToolTranslator::Private::populateIgnoredGroups()
{
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

} // namespace Digikam
