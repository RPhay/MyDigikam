/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-24-01
 * Description : pixels antialiasing filter
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

#ifndef DIGIKAM_DPIXELS_ALIAS_FILTER_H
#define DIGIKAM_DPIXELS_ALIAS_FILTER_H

// C++ includes

#include <cmath>

// Local includes

#include "digikam_export.h"
#include "digikam_globals.h"
#include "dimgthreadedfilter.h"

namespace Digikam
{

class DIGIKAM_EXPORT DPixelsAliasFilter
{
public:

    explicit DPixelsAliasFilter();
    ~DPixelsAliasFilter();

public:   // Public methods.

    void pixelAntiAliasing(uchar* const data,
                           int Width,
                           int Height,
                           double X,
                           double Y,
                           uchar* const A,
                           uchar* const R,
                           uchar* const G,
                           uchar* const B);

    void pixelAntiAliasing16(unsigned short* const data,
                             int Width,
                             int Height,
                             double X,
                             double Y,
                             unsigned short* const A,
                             unsigned short* const R,
                             unsigned short* const G,
                             unsigned short* const B);

private:

    inline int setPositionAdjusted(int Width,
                                   int Height,
                                   int X,
                                   int Y);
};

} // namespace Digikam

#endif // DIGIKAM_DPIXELS_ALIAS_FILTER_H
