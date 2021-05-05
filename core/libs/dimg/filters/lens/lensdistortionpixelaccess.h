/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-12-27
 * Description : access pixels method for lens distortion algorithm.
 *
 * Copyright (C) 2004-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DIGIKAM_LENS_DISTORTION_PIXEL_ACCESS_H
#define DIGIKAM_LENS_DISTORTION_PIXEL_ACCESS_H

#define LensDistortionPixelAccessRegions 20
#define LensDistortionPixelAccessWidth   40
#define LensDistortionPixelAccessHeight  20
#define LensDistortionPixelAccessXOffset 3
#define LensDistortionPixelAccessYOffset 3

// Local includes

#include "dimg.h"
#include "digikam_export.h"

namespace Digikam
{

/**
 * LensDistortionPixelAccess class: solving the eternal problem: random, cubic-interpolated,
 * sub-pixel coordinate access to an image.
 * Assuming that accesses are at least slightly coherent,
 * LensDistortionPixelAccess keeps LensDistortionPixelAccessRegions buffers, each containing a
 * LensDistortionPixelAccessWidth x LensDistortionPixelAccessHeight region of pixels.
 * Buffer[0] is always checked first, so move the last accessed
 * region into that position.
 * When a request arrives which is outside all the regions,
 * get a new region.
 * The new region is placed so that the requested pixel is positioned
 * at [LensDistortionPixelAccessXOffset, LensDistortionPixelAccessYOffset] in the region.
 */

class DIGIKAM_EXPORT LensDistortionPixelAccess
{

public:

    explicit LensDistortionPixelAccess(DImg* srcImage);
    ~LensDistortionPixelAccess();

    void pixelAccessGetCubic(double srcX, double srcY, double brighten, uchar* dst);

protected:

    inline uchar* pixelAccessAddress(int i, int j);
    void pixelAccessSelectRegion(int n);
    void pixelAccessDoEdge(int i, int j);
    void pixelAccessReposition(int xInt, int yInt);
    void cubicInterpolate(uchar* src, int rowStride, uchar* dst,
                          bool sixteenBit, double dx, double dy, double brighten);

private:

    DImg* m_image;

    DImg* m_buffer[LensDistortionPixelAccessRegions];

    int   m_width;
    int   m_height;
    int   m_depth;
    int   m_imageWidth;
    int   m_imageHeight;
    bool  m_sixteenBit;
    int   m_tileMinX[LensDistortionPixelAccessRegions];
    int   m_tileMaxX[LensDistortionPixelAccessRegions];
    int   m_tileMinY[LensDistortionPixelAccessRegions];
    int   m_tileMaxY[LensDistortionPixelAccessRegions];
};

} // namespace Digikam

#endif // DIGIKAM_LENS_DISTORTION_PIXEL_ACCESS_H
