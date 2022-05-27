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

#include "OpenGLWindowRenderer.h"

// Qt includes

#include <QResizeEvent>

// Local includes

#include "OpenGLRendererBase_p.h"
#include "QtAV_factory.h"
#include "digikam_debug.h"

namespace QtAV
{

FACTORY_REGISTER(VideoRenderer, OpenGLWindow, "OpenGLWindow")

class Q_DECL_HIDDEN OpenGLWindowRendererPrivate : public OpenGLRendererBasePrivate
{
public:

    explicit OpenGLWindowRendererPrivate(QPaintDevice* const pd)
        : OpenGLRendererBasePrivate(pd)
    {
    }
};

OpenGLWindowRenderer::OpenGLWindowRenderer(UpdateBehavior updateBehavior, QWindow* const parent)
    : QOpenGLWindow     (updateBehavior, parent)
    , OpenGLRendererBase(*new OpenGLWindowRendererPrivate(this))
{
}

VideoRendererId OpenGLWindowRenderer::id() const
{
    return VideoRendererId_OpenGLWindow;
}

// MUST call update() on gui(main) thread that the window belongs to because update() will finally call startTimer

void OpenGLWindowRenderer::initializeGL()
{
    onInitializeGL();
}

void OpenGLWindowRenderer::paintGL()
{
    onPaintGL();
}

void OpenGLWindowRenderer::resizeGL(int w, int h)
{
    onResizeGL(w, h);
}

void OpenGLWindowRenderer::resizeEvent(QResizeEvent* e)
{
    onResizeEvent(e->size().width(), e->size().height());
    QOpenGLWindow::resizeEvent(e);                          // will call resizeGL(). TODO: will call paintEvent()?
}

void OpenGLWindowRenderer::showEvent(QShowEvent*)
{
    onShowEvent();
    resizeGL(width(), height());
}

IMPLEMENT_VIDEO_RENDERER_EMIT_METHODS(OpenGLWindowRenderer)

} // namespace QtAV
