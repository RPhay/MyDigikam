/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-06-14
 * Description : digiKam compile-time settings into CMake-built program.
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

#ifndef DIGIKAM_DBUILD_DATE_H
#define DIGIKAM_DBUILD_DATE_H

// Qt includes

#include <QDateTime>

// Local includes.

#include "digikam_export.h"

namespace Digikam
{

/**
 * Return the time-stamp of the current build.
 */
DIGIKAM_EXPORT const QDateTime digiKamBuildDate();

} // namespace Digikam

#endif // DIGIKAM_DBUILD_DATE_H

