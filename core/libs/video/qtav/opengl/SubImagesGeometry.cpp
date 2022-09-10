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

#include "SubImagesGeometry.h"

// Local includes*

#include "digikam_debug.h"

namespace QtAV
{

#define U8COLOR 0

static const int kMaxTexWidth = 4096; // FIXME: glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);

// if texture1d, we can directly copy ASS_Image.bitmap without line by line copy, i.e. tiled, and upload only once

typedef struct Q_DECL_HIDDEN
{
    float x;            // depends on target rect
    float y;            // depends on target rect
    float tx;           // depends on texture size and rects layout
    float ty;           // depends on texture size and rects layout

#if U8COLOR

    union
    {
        quint8  r, g, b, a;  // to be normalized
        quint32 rgba;
    };

#else

    float r, g, b, a;

#endif

} VertexData;

static VertexData* SetUnnormalizedVertexData(VertexData* v, int tx, int ty, int tw, int th,
                                             quint32 color, bool useIndecies)
{

#if U8COLOR

    union
    {
        quint8  r;
        quint8  g;
        quint8  b;
        quint8  a;
        quint32 rgba;
    };

    r = color  >> 24;
    g = (color >> 16) & 0xff;
    b = (color >> 8)  & 0xff;
    a = 255 - (color & 0xff);

#else

    float r;
    float g;
    float b;
    float a;
    r = (float)(color  >> 24)         / 255.0;
    g = (float)((color >> 16) & 0xff) / 255.0;
    b = (float)((color >> 8)  & 0xff) / 255.0;
    a = (float)(255 - (color & 0xff)) / 255.0;

#endif

    // normalize later

    v[0].tx = tx;
    v[0].ty = ty;
    v[1].tx = tx;
    v[1].ty = ty + th;
    v[2].tx = tx + tw;
    v[2].ty = ty;
    v[3].tx = tx + tw;
    v[3].ty = ty + th;

#if U8COLOR

    v[0].rgba = rgba;
    v[1].rgba = rgba;
    v[2].rgba = rgba;
    v[3].rgba = rgba;

#else

#   define SETC(x) x.r = r; x.g=g; x.b=b; x.a=a;

    SETC(v[0]);
    SETC(v[1]);
    SETC(v[2]);
    SETC(v[3]);

#endif

    if (!useIndecies)
    {
        v[4] = v[1];
        v[5] = v[2];

        return (v + 6);
    }

    return (v + 4);
}

static VertexData* SetVertexPositionAndNormalize(VertexData* v, float x, float y, float w, float h,
                                                 float texW, float texH, bool useIndecies)
{
    v[0].x   = x;
    v[0].y   = y;
    v[1].x   = x;
    v[1].y   = y + h;
    v[2].x   = x + w;
    v[2].y   = y;
    v[3].x   = x + w;
    v[3].y   = y + h;
    v[0].tx /= texW;
    v[0].ty /= texH;
    v[1].tx /= texW;
    v[1].ty /= texH;
    v[2].tx /= texW;
    v[2].ty /= texH;
    v[3].tx /= texW;
    v[3].ty /= texH;
/*
    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("%f,%f<=%f,%f; %u,%u,%u,%u",
            v[3].x, v[3].y, v[3].tx, v[3].ty, v[3].r, v[3].g, v[3].b, v[3].a);
*/
    if (!useIndecies)
    {
        v[4] = v[1];
        v[5] = v[2];

        return (v + 6);
    }

    return (v + 4);
}

SubImagesGeometry::SubImagesGeometry()
    : Geometry    (),
      m_normalized(false),
      m_w         (0),
      m_h         (0)
{
    setPrimitive(Geometry::Triangles);
    m_attributes << Attribute(TypeF32, 2)
                 << Attribute(TypeF32, 2, 2 * sizeof(float))

#if U8COLOR

                 << Attribute(TypeU8, 4, 4 * sizeof(float), true);

#else

                 << Attribute(TypeF32, 4, 4 * sizeof(float));

#endif

}

bool SubImagesGeometry::setSubImages(const SubImageSet& images)
{
    // TODO: operator ==

    if (m_images == images)
        return false;

    m_images = images;

    return true;
}

bool SubImagesGeometry::generateVertexData(const QRect& rect, bool useIndecies, int maxWidth)
{
    if (maxWidth < 0)
        maxWidth = kMaxTexWidth;

    if (useIndecies)
        allocate(4 * m_images.images.size(), 6 * m_images.images.size());
    else
        allocate(6 * m_images.images.size());

    qCDebug(DIGIKAM_QTAV_LOG) << "images:"
                              << m_images.isValid() << "/" << m_images.images.size() << ","
                              << m_images.width()   << "/" << m_images.height();

    m_rects_upload.clear();
    m_w          = 0;
    m_h          = 0;
    m_normalized = false;

    if (!m_images.isValid())
        return false;

    int W          = 0;
    int H          = 0;
    int x          = 0;
    int h          = 0;
    VertexData* vd = (VertexData*)vertexData();
    int index      = 0;

    Q_FOREACH (const SubImage& i, m_images.images)
    {
        if (((x + i.stride) > maxWidth) && (maxWidth > 0))
        {
            W  = qMax(W, x);
            H += h;
            x  = 0;
            h  = 0;
        }

        // we use w instead of stride even if we must upload stride.
        // when maping texture coordinates and view port coordinates,
        // we can use the visual rects instead of stride, i.e. the geometry vertices are (x, y, w, h),
        // not (x, y, stride, h)

        m_rects_upload.append(QRect(x, H, i.stride, i.h));
        vd = SetUnnormalizedVertexData(vd, x, H, i.w, i.h, i.color, useIndecies);

        if (useIndecies)
        {
            // TODO: set only once because it never changes, use IBO

            const int v0 = index * 4 / 6;
            setIndexValue(index,     v0,     v0 + 1, v0 + 2);
            setIndexValue(index + 3, v0 + 1, v0 + 2, v0 + 3);
            index       += 6;
        }

        x += i.w;
        h  = qMax(h, i.h);
    }

    W   = qMax(W, x);
    H  += h;
    m_w = W;
    m_h = H;
/*
    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("sub texture %dx%d", m_w, m_h);
*/
    const float dx0 = rect.x();
    const float dy0 = rect.y();
    const float sx  = float(rect.width())  / float(m_images.width());
    const float sy  = float(rect.height()) / float(m_images.height());
    vd              = (VertexData*)vertexData();

    Q_FOREACH (const SubImage& i, m_images.images)
    {
/*
        qCDebug(DIGIKAM_QTAV_LOG) << rect;
        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("i: %d,%d", i.x, i.y);
*/
        vd           = SetVertexPositionAndNormalize(vd,
                                                     dx0 + float(i.x) * sx,
                                                     dy0 + float(i.y) * sy,
                                                     i.w * sx,
                                                     i.h * sy,
                                                     m_w,
                                                     m_h,
                                                     useIndecies);
        m_normalized = true;
    }

    return true;
}

int SubImagesGeometry::stride() const
{
    return sizeof(VertexData);
}

} // namespace QtAV
