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

#ifndef QTAV_SUBIMAGE_H
#define QTAV_SUBIMAGE_H

// Qt includes

#include <QVector>

// Local includes

#include "QtAV_Global.h"

namespace QtAV
{

/*!
 * \brief The SubImage struct
 * (x, y, w, h) is relative to parent SubImageSet (0, 0, SubImageSet.w, SubImageSet.h)
 */
struct SubImageSet;

struct DIGIKAM_EXPORT SubImage
{
    SubImage(int x = 0, int y = 0, int w = 0, int h = 0, int stride = 0);

    bool operator ==(const SubImage& o) const
    {
        return ((x      == o.x)       &&
                (y      == o.y)       &&
                (w      == o.w)       &&
                (h      == o.h)       &&
                (stride == o.stride)  &&
                (color  == o.color)   &&
                (data   == o.data));
    }

    int         x, y;
    int         w, h;
    int         stride;
    quint32     color;      ///< ass only
    QByteArray  data;       ///< size = stride*h
};

struct DIGIKAM_EXPORT SubImageSet
{
    enum Format
    {
        ASS,
        RGBA,
        Unknown
    };

    SubImageSet(int width = 0, int height = 0, Format format = Unknown);

    int width() const
    {
        return w;
    }

    int height() const
    {
        return h;
    }

    Format format() const
    {
        return fmt;
    }

    bool isValid() const
    {
        return (!images.isEmpty() && (w > 0) && (h > 0) && (fmt != Unknown));
    }

    void reset(int width = 0, int height = 0, Format format = Unknown)
    {
        fmt = format;
        w   = width;
        h   = height;
        ++id;
        images.clear();
    }

    bool operator ==(const SubImageSet& other) const
    {
        // TODO: image data

        return ((id     == other.id)    &&
                (w      == other.w)     &&
                (h      == other.h)     &&
                (fmt    == other.fmt)   &&
                (images == other.images));
    }

    QVector<SubImage> images;

private:

    Format            fmt;
    int               w;
    int               h;
    int               id;
};

} // namespace QtAV

#endif // QTAV_SUBIMAGE_H
