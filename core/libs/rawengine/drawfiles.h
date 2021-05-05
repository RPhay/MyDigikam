/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-11-06
 * Description : list of RAW file extensions supported by libraw
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DRAW_FILES_H
#define DIGIKAM_DRAW_FILES_H

// Qt includes

#include <QString>
#include <QMap>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

DIGIKAM_EXPORT QMap<QString, QString> s_rawFileExtensionsdWithDesc();

DIGIKAM_EXPORT QString                s_rawFileExtensions();

DIGIKAM_EXPORT int                    s_rawFileExtensionsVersion();

} // namespace Digikam

#endif // DIGIKAM_DRAW_FILES_H
