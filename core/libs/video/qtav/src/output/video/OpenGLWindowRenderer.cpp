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

    explicit OpenGLWindowRendererPrivate(QPaintDevice* pd)
        : OpenGLRendererBasePrivate(pd)
    {
    }
};

OpenGLWindowRenderer::OpenGLWindowRenderer(UpdateBehavior updateBehavior, QWindow* parent)
    : QOpenGLWindow(updateBehavior, parent)
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

void OpenGLWindowRenderer::emitSourceAspectRatioChanged(qreal v)
{
   Q_EMIT sourceAspectRatioChanged(v); 
}

void OpenGLWindowRenderer::emitOutAspectRatioChanged()        
{
   Q_EMIT outAspectRatioChanged(); 
}

void OpenGLWindowRenderer::emitOutAspectRatioModeChanged()    
{
   Q_EMIT outAspectRatioModeChanged(); 
}

void OpenGLWindowRenderer::emitOrientationChanged()           
{
   Q_EMIT orientationChanged(); 
}

void OpenGLWindowRenderer::emitVideoRectChanged()             
{
   Q_EMIT videoRectChanged(); 
}

void OpenGLWindowRenderer::emitContentRectChanged()           
{
   Q_EMIT contentRectChanged(); 
}

void OpenGLWindowRenderer::emitRegionOfInterestChanged()      
{
   Q_EMIT regionOfInterestChanged(); 
}

void OpenGLWindowRenderer::emitVideoFrameSizeChanged()        
{
   Q_EMIT videoFrameSizeChanged(); 
}

void OpenGLWindowRenderer::emitRendererSizeChanged()          
{
   Q_EMIT rendererSizeChanged(); 
}

void OpenGLWindowRenderer::emitBrightnessChanged(qreal v)       
{
   Q_EMIT brightnessChanged(v); 
}

void OpenGLWindowRenderer::emitContrastChanged(qreal v)         
{
   Q_EMIT contrastChanged(v); 
}

void OpenGLWindowRenderer::emitHueChanged(qreal v)              
{
   Q_EMIT hueChanged(v); 
}

void OpenGLWindowRenderer::emitSaturationChanged(qreal v)       
{
   Q_EMIT saturationChanged(v); 
}

void OpenGLWindowRenderer::emitBackgroundColorChanged()       
{
   Q_EMIT backgroundColorChanged(); 
}

} // namespace QtAV
