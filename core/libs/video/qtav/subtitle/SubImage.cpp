/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "SubImage.h"

// Qt includes

#include <QImage>

namespace QtAV
{

SubImage::SubImage(int x, int y, int w, int h, int stride)
    : x     (x),
      y     (y),
      w     (w),
      h     (h),
      stride(stride),
      color (0)
{
}

SubImageSet::SubImageSet(int width, int height, Format format)
    : fmt   (format),
      w     (width),
      h     (height),
      id    (0)
{
}

#define _r(c)  ((c)>>24)
#define _g(c)  (((c)>>16)&0xFF)
#define _b(c)  (((c)>>8)&0xFF)
#define _a(c)  ((c)&0xFF)

#define qRgba2(r, g, b, a) ((a << 24) | (r << 16) | (g  << 8) | b)

/*
 * ASS_Image: 1bit alpha per pixel + 1 rgb per image. less memory usage
 */

// 0xAARRGGBB

#if (Q_BYTE_ORDER == Q_BIG_ENDIAN)
#   define ARGB32_SET(C, R, G, B, A) \
    C[0] = (A); \
    C[1] = (R); \
    C[2] = (G); \
    C[3] = (B);
#   define ARGB32_ADD(C, R, G, B, A) \
    C[0] += (A); \
    C[1] += (R); \
    C[2] += (G); \
    C[3] += (B);
#   define ARGB32_A(C) (C[0])
#   define ARGB32_R(C) (C[1])
#   define ARGB32_G(C) (C[2])
#   define ARGB32_B(C) (C[3])
#else
#   define ARGB32_SET(C, R, G, B, A) \
    C[0] = (B); \
    C[1] = (G); \
    C[2] = (R); \
    C[3] = (A);
#   define ARGB32_ADD(C, R, G, B, A) \
    C[0] += (B); \
    C[1] += (G); \
    C[2] += (R); \
    C[3] += (A);
#   define ARGB32_A(C) (C[3])
#   define ARGB32_R(C) (C[2])
#   define ARGB32_G(C) (C[1])
#   define ARGB32_B(C) (C[0])
#endif

#define USE_QRGBA 0

// C[i] = C'[i] = (k * c[i] + (255 - k) * C[i]) / 255 = C[i] + k * (c[i] - C[i]) / 255
// min(c[i],C[i]) <= C'[i] <= max(c[i],C[i])

// render 1 ass image into a 32bit QImage with alpha channel.
// use dstX, dstY instead of img->dst_x/y because image size is small then ass renderer size

void RenderASS(QImage* image, const SubImage& img, int dstX, int dstY)
{
    const quint8 a    = 255 - _a(img.color);

    if (a == 0)
        return;

    const quint8 r    = _r(img.color);
    const quint8 g    = _g(img.color);
    const quint8 b    = _b(img.color);
    const quint8* src = (const quint8*)img.data.constData();

    // use QRgb to avoid endian issue

    QRgb* dst         = reinterpret_cast<QRgb*>(image->bits()) + dstY * image->width() + dstX;

    // k*src+(1-k)*dst

    for (int y = 0 ; y < img.h ; ++y)
    {
        for (int x = 0 ; x < img.w ; ++x)
        {
            const unsigned k = ((unsigned)src[x]) * a / 255;

#if USE_QRGBA

            const unsigned A = qAlpha(dst[x]);

#else

            quint8* c        = (quint8*)(&dst[x]);
            const unsigned A = ARGB32_A(c);

#endif

            if      (A == 0)
            {
                // dst color can be ignored

#if USE_QRGBA

                 dst[x] = qRgba(r, g, b, k);

#else

                 ARGB32_SET(c, r, g, b, k);

#endif

            }
            else if (k == 0)
            {
                // no change
                // dst[x] = qRgba(qRed(dst[x])), qGreen(dst[x]), qBlue(dst[x]), qAlpha(dst[x])) == dst[x];
            }
            else if (k == 255)
            {

#if USE_QRGBA

                dst[x] = qRgba(r, g, b, k);

#else

                ARGB32_SET(c, r, g, b, k);

#endif
            }
            else
            {
                // c = k * dc / 255 = k * dc / 256 * (1 - 1 / 256), -1 < err(c) = k * dc / 256 ^ 2 < 1, -1 is bad!

#if USE_QRGBA

                // no need to &0xff because always be 0~255

                dst[x] += qRgba2(k * (r-qRed(dst[x]))   / 255,
                                 k * (g-qGreen(dst[x])) / 255,
                                 k * (b-qBlue(dst[x]))  / 255,
                                 k * (a-A)              / 255);

#else

                const unsigned R = ARGB32_R(c);
                const unsigned G = ARGB32_G(c);
                const unsigned B = ARGB32_B(c);

                ARGB32_ADD(c, r == R ? 0 : k * (r - R) / 255,
                              g == G ? 0 : k * (g - G) / 255,
                              b == B ? 0 : k * (b - B) / 255,
                              a == A ? 0 : k * (a - A) / 255);

#endif

            }
        }

        src += img.stride;
        dst += image->width();
    }
}

} // namespace QtAV
