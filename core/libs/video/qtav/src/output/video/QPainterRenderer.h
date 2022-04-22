/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef QAV_QPAINTER_RENDERER_H
#define QAV_QPAINTER_RENDERER_H

// Qt includes

#include <QImage>

// Local includes

#include "VideoRenderer.h"

// TODO: not abstract.

namespace QtAV
{

class QPainterRendererPrivate;

class Q_AV_EXPORT QPainterRenderer : public VideoRenderer
{
    DPTR_DECLARE_PRIVATE(QPainterRenderer)

public:

    QPainterRenderer();
    bool isSupported(VideoFormat::PixelFormat pixfmt) const Q_DECL_OVERRIDE;

protected:

    bool preparePixmap(const VideoFrame& frame);
    void drawBackground() Q_DECL_OVERRIDE;

    // draw the current frame using the current paint engine. called by paintEvent()

    void drawFrame() Q_DECL_OVERRIDE;

    QPainterRenderer(QPainterRendererPrivate& d);
};

} // namespace QtAV

#endif // QTAV_QPAINTER_RENDERER_H
