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

#ifndef DIGIKAM_EXIF_TOOL_BINARY_H
#define DIGIKAM_EXIF_TOOL_BINARY_H

// Local includes

#include "digikam_export.h"
#include "dbinaryiface.h"

using namespace Digikam;

namespace Digikam
{

class DIGIKAM_EXPORT ExifToolBinary : public DBinaryIface
{
    Q_OBJECT

public:

    explicit ExifToolBinary(QObject* const parent = nullptr);
    ~ExifToolBinary() override;
};

} // namespace Digikam

#endif // DIGIKAM_EXIF_TOOL_BINARY_H
