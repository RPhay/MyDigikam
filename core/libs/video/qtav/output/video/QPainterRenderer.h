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

#ifndef QTAV_QPAINTER_RENDERER_H
#define QTAV_QPAINTER_RENDERER_H

// Qt includes

#include <QImage>

// Local includes

#include "VideoRenderer.h"

// TODO: not abstract.

namespace QtAV
{

class QPainterRendererPrivate;

class DIGIKAM_EXPORT QPainterRenderer : public VideoRenderer
{
    DPTR_DECLARE_PRIVATE(QPainterRenderer)          // cppcheck-suppress unusedPrivateFunction

public:

    QPainterRenderer();

    bool isSupported(VideoFormat::PixelFormat pixfmt) const override;

protected:

    bool preparePixmap(const VideoFrame& frame);
    void drawBackground()                                   override;

    // draw the current frame using the current paint engine.
    // called by paintEvent()

    void drawFrame()                                        override;

    explicit QPainterRenderer(QPainterRendererPrivate& d);
};

} // namespace QtAV

#endif // QTAV_QPAINTER_RENDERER_H
