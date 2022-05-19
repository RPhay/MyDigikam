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

#include "GLWidgetRenderer2.h"

// Qt includes

#include <QResizeEvent>

// Local includes

#include "OpenGLRendererBase_p.h"

namespace QtAV
{

class Q_DECL_HIDDEN GLWidgetRenderer2Private : public OpenGLRendererBasePrivate
{
public:

    explicit GLWidgetRenderer2Private(QPaintDevice* pd)
        : OpenGLRendererBasePrivate(pd)
    {
    }
};

VideoRendererId GLWidgetRenderer2::id() const
{
    return VideoRendererId_GLWidget2;
}

GLWidgetRenderer2::GLWidgetRenderer2(QWidget *parent, const QGLWidget* shareWidget, Qt::WindowFlags f)
    : QGLWidget(parent, shareWidget, f)
    , OpenGLRendererBase(*new GLWidgetRenderer2Private(this))
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

    // default: swap in qpainter dtor. we should swap before QPainter.endNativePainting()

    setAutoBufferSwap(false);
    setAutoFillBackground(false);
}

void GLWidgetRenderer2::initializeGL()
{
    // already current

    onInitializeGL();
}

void GLWidgetRenderer2::paintGL()
{
    DPTR_D(GLWidgetRenderer2);

    /* we can mix gl and qpainter.
     * QPainter painter(this);
     * painter.beginNativePainting();
     * gl functions...
     * painter.endNativePainting();
     * swapBuffers();
     */
    handlePaintEvent();
    swapBuffers();

    if (d.painter && d.painter->isActive())
        d.painter->end();
}

void GLWidgetRenderer2::resizeGL(int w, int h)
{
    onResizeGL(w, h);
}

void GLWidgetRenderer2::resizeEvent(QResizeEvent *e)
{
    onResizeEvent(e->size().width(), e->size().height());
    QGLWidget::resizeEvent(e);                              // will call resizeGL(). TODO: will call paintEvent()?
}

void GLWidgetRenderer2::showEvent(QShowEvent *)
{
    onShowEvent();
    resizeGL(width(), height());
}

void GLWidgetRenderer2::emitSourceAspectRatioChanged(qreal v)
{
   Q_EMIT sourceAspectRatioChanged(v); 
}

void GLWidgetRenderer2::emitOutAspectRatioChanged()        
{
   Q_EMIT outAspectRatioChanged(); 
}

void GLWidgetRenderer2::emitOutAspectRatioModeChanged()    
{
   Q_EMIT outAspectRatioModeChanged(); 
}

void GLWidgetRenderer2::emitOrientationChanged()           
{
   Q_EMIT orientationChanged(); 
}

void GLWidgetRenderer2::emitVideoRectChanged()             
{
   Q_EMIT videoRectChanged(); 
}

void GLWidgetRenderer2::emitContentRectChanged()           
{
   Q_EMIT contentRectChanged(); 
}

void GLWidgetRenderer2::emitRegionOfInterestChanged()      
{
   Q_EMIT regionOfInterestChanged(); 
}

void GLWidgetRenderer2::emitVideoFrameSizeChanged()        
{
   Q_EMIT videoFrameSizeChanged(); 
}

void GLWidgetRenderer2::emitRendererSizeChanged()          
{
   Q_EMIT rendererSizeChanged(); 
}

void GLWidgetRenderer2::emitBrightnessChanged(qreal v)       
{
   Q_EMIT brightnessChanged(v); 
}

void GLWidgetRenderer2::emitContrastChanged(qreal v)         
{
   Q_EMIT contrastChanged(v); 
}

void GLWidgetRenderer2::emitHueChanged(qreal v)              
{
   Q_EMIT hueChanged(v); 
}

void GLWidgetRenderer2::emitSaturationChanged(qreal v)       
{
   Q_EMIT saturationChanged(v); 
}

void GLWidgetRenderer2::emitBackgroundColorChanged()       
{
   Q_EMIT backgroundColorChanged(); 
}

} // namespace QtAV
