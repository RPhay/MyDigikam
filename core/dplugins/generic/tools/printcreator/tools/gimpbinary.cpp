/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-07-04
 * Description : Autodetect gimp binary program
 *
 * Copyright (C) 2017-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "gimpbinary.h"

// KDE includes

#include <klocalizedstring.h>

namespace DigikamGenericPrintCreatorPlugin
{

GimpBinary::GimpBinary(QObject* const)
    : DBinaryIface(

#ifdef Q_OS_MACOS

                   QLatin1String("GIMP-bin"),

#elif defined Q_OS_WIN

                   QLatin1String("gimp-2.10"),

#else

                   QLatin1String("gimp"),

#endif

                   QLatin1String("The Gimp"),
                   QLatin1String("https://www.gimp.org/downloads/"),
                   QLatin1String("PrintCreator"),
                   QStringList(QLatin1String("-v")),
                   i18n("The GNU Image Manipulation Program.")
                  )
{
    setup();
}

GimpBinary::~GimpBinary()
{
}

} // namespace DigikamGenericPrintCreatorPlugin
