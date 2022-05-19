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

#include "GLWidgetRenderer.h"

// Qt includes

#include <QResizeEvent>
#include <QSurface>
#include <QPainter>

// Local includes

#include "OpenGLRendererBase_p.h"

namespace QtAV
{

class Q_DECL_HIDDEN GLWidgetRendererPrivate : public OpenGLRendererBasePrivate
{
public:

    explicit GLWidgetRendererPrivate(QPaintDevice* pd)
        : OpenGLRendererBasePrivate(pd)
    {
    }
};

VideoRendererId GLWidgetRenderer::id() const
{
    return VideoRendererId_GLWidget;
}

GLWidgetRenderer::GLWidgetRenderer(QWidget* const parent, Qt::WindowFlags f)
    : QOpenGLWidget(parent, f),
      OpenGLRendererBase(*new GLWidgetRendererPrivate(this))
{
    setAcceptDrops(true);
    setFocusPolicy(Qt::StrongFocus);

    /* To rapidly update custom widgets that constantly paint over their entire areas with
     * opaque content, e.g., video streaming widgets, it is better to set the widget's
     * Qt::WA_OpaquePaintEvent, avoiding any unnecessary overhead associated with repainting the
     * widget's background
     */
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    setAutoFillBackground(false);
}

void GLWidgetRenderer::initializeGL()
{
    // already current

    onInitializeGL();
}

void GLWidgetRenderer::paintGL()
{
    DPTR_D(GLWidgetRenderer);

    /* we can mix gl and qpainter.
     * QPainter painter(this);
     * painter.beginNativePainting();
     * gl functions...
     * painter.endNativePainting();
     * swapBuffers();
     */
    handlePaintEvent();

    if (d.painter && d.painter->isActive())
    {
        d.painter->end();
    }
}

void GLWidgetRenderer::resizeGL(int w, int h)
{
    onResizeGL(w, h);
}

void GLWidgetRenderer::resizeEvent(QResizeEvent* e)
{
    onResizeEvent(e->size().width(), e->size().height());
    QOpenGLWidget::resizeEvent(e);
}

void GLWidgetRenderer::showEvent(QShowEvent*)
{
    onShowEvent();
    resizeGL(width(), height());
}

IMPLEMENT_VIDEO_RENDERER_EMIT_METHODS(GLWidgetRenderer)

} // namespace QtAV
