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

#include "OpenGLRendererBase_p.h"

// Qt includes

#include <QResizeEvent>

// Local includes

#include "OpenGLVideo.h"
#include "FilterContext.h"
#include "OpenGLHelper.h"
#include "digikam_debug.h"

namespace QtAV
{

OpenGLRendererBasePrivate::OpenGLRendererBasePrivate(QPaintDevice* const pd)
    : painter      (new QPainter()),
      frame_changed(false)
{
    filter_context               = VideoFilterContext::create(VideoFilterContext::QtPainter);
    filter_context->paint_device = pd;
    filter_context->painter      = painter;
}

OpenGLRendererBasePrivate::~OpenGLRendererBasePrivate()
{
    if (painter)
    {
        delete painter;
        painter = nullptr;
    }
}

void OpenGLRendererBasePrivate::setupAspectRatio()
{
    matrix.setToIdentity();
    matrix.scale((GLfloat)out_rect.width()  / (GLfloat)renderer_width,
                 (GLfloat)out_rect.height() / (GLfloat)renderer_height, 1);

    if (rotation())
        matrix.rotate(rotation(), 0, 0, 1); // Z axis
}

OpenGLRendererBase::OpenGLRendererBase(OpenGLRendererBasePrivate& d)
    : VideoRenderer(d)
{
    setPreferredPixelFormat(VideoFormat::Format_YUV420P);
}

OpenGLRendererBase::~OpenGLRendererBase()
{
   d_func().glv.setOpenGLContext(nullptr);
}

bool OpenGLRendererBase::isSupported(VideoFormat::PixelFormat pixfmt) const
{
    return OpenGLVideo::isSupported(pixfmt);
}

OpenGLVideo* OpenGLRendererBase::opengl() const
{
    return const_cast<OpenGLVideo*>(&d_func().glv);
}

bool OpenGLRendererBase::receiveFrame(const VideoFrame& frame)
{
    DPTR_D(OpenGLRendererBase);

    d.video_frame   = frame;
    d.frame_changed = true;

    // can not call updateGL() directly because no event
    // and paintGL() will in video thread

    updateUi();

    return true;
}

void OpenGLRendererBase::drawBackground()
{
    d_func().glv.fill(backgroundColor());
}

void OpenGLRendererBase::drawFrame()
{
    DPTR_D(OpenGLRendererBase);

    QRect roi = realROI();
/*
    d.glv.render(QRectF(-1, 1, 2, -2), roi, d.matrix);
*/
    // QRectF() means the whole viewport

    if (d.frame_changed)
    {
        d.glv.setCurrentFrame(d.video_frame);
        d.frame_changed = false;
    }

    d.glv.render(QRectF(), roi, d.matrix);
}

void OpenGLRendererBase::onInitializeGL()
{
    DPTR_D(OpenGLRendererBase);
/*
    makeCurrent();
*/
    initializeOpenGLFunctions();

    QOpenGLContext* const ctx = const_cast<QOpenGLContext*>(QOpenGLContext::currentContext());
    d.glv.setOpenGLContext(ctx);
}

void OpenGLRendererBase::onPaintGL()
{
    DPTR_D(OpenGLRendererBase);

    /* we can mix gl and qpainter.
     * QPainter painter(this);
     * painter.beginNativePainting();
     * gl functions...
     * painter.endNativePainting();
     * swapBuffers();
     */

    handlePaintEvent();
/*
    context()->swapBuffers(this);
*/
    if (d.painter && d.painter->isActive())
        d.painter->end();
}

void OpenGLRendererBase::onResizeGL(int w, int h)
{
    if (!QOpenGLContext::currentContext())
        return;

    DPTR_D(OpenGLRendererBase);

    d.glv.setProjectionMatrixToRect(QRectF(0, 0, w, h));
    d.setupAspectRatio();
}

void OpenGLRendererBase::onResizeEvent(int w, int h)
{
    DPTR_D(OpenGLRendererBase);

    d.update_background = true;
    resizeRenderer(w, h);
    d.setupAspectRatio();
/*
    QOpenGLWindow::resizeEvent(e); // will call resizeGL(). TODO:will call paintEvent()?
*/
}

// TODO: out_rect not correct when top level changed

void OpenGLRendererBase::onShowEvent()
{
    DPTR_D(OpenGLRendererBase);

    d.update_background = true;

    /*
     * Do something that depends on widget below! e.g. recreate render target for direct2d.
     * When Qt::WindowStaysOnTopHint changed, window will hide first then show. If you
     * don't do anything here, the widget content will never be updated.
     */
}

void OpenGLRendererBase::onSetOutAspectRatio(qreal ratio)
{
    Q_UNUSED(ratio);
    DPTR_D(OpenGLRendererBase);

    d.setupAspectRatio();
}

void OpenGLRendererBase::onSetOutAspectRatioMode(OutAspectRatioMode mode)
{
    Q_UNUSED(mode);
    DPTR_D(OpenGLRendererBase);

    d.setupAspectRatio();
}

bool OpenGLRendererBase::onSetOrientation(int value)
{
    Q_UNUSED(value)
    d_func().setupAspectRatio();

    return true;
}

bool OpenGLRendererBase::onSetBrightness(qreal b)
{
    d_func().glv.setBrightness(b);

    return true;
}

bool OpenGLRendererBase::onSetContrast(qreal c)
{
    d_func().glv.setContrast(c);

    return true;
}

bool OpenGLRendererBase::onSetHue(qreal h)
{
    d_func().glv.setHue(h);

    return true;
}

bool OpenGLRendererBase::onSetSaturation(qreal s)
{
    d_func().glv.setSaturation(s);

    return true;
}

} // namespace QtAV
