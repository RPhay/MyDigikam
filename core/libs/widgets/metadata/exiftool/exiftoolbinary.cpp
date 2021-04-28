/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-04-18
 * Description : auto-detect ExifTool program and version.
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "exiftoolbinary.h"

// KDE includes

#include <klocalizedstring.h>

namespace Digikam
{

ExifToolBinary::ExifToolBinary(QObject* const)
    : DBinaryIface(QLatin1String("exiftool"),
                   QLatin1String("10.00"),          // First release for production uses.
                   QLatin1String(),
                   0,
                   QLatin1String("ExifTool"),
                   QLatin1String("https://exiftool.org/"),
                   QString(),
                   QStringList(QLatin1String("-ver")),
                   i18n("ExifTool is a platform-independent command-line application for reading, "
                        "writing and editing meta information in a wide variety of files.")
                  )
{
    setup();
}

ExifToolBinary::~ExifToolBinary()
{
}

} // namespace Digikam
