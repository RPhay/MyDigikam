/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
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

#ifndef QTAV_OPENGL_RENDERER_BASE_H
#define QTAV_OPENGL_RENDERER_BASE_H

#include "VideoRenderer.h"

// Qt includes

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#   include <QOpenGLFunctions>
#elif QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)
#   include <QtOpenGL/QGLFunctions>
#   define QOpenGLFunctions QGLFunctions
#endif

namespace QtAV
{

/*!
 * \brief The OpenGLRendererBase class
 * Renderering video frames using GLSL. A more generic high level class OpenGLVideo is used internally.
 * TODO: for Qt5, no QtOpenGL, use QWindow instead.
 */
class OpenGLRendererBasePrivate;

class Q_AV_EXPORT OpenGLRendererBase : public VideoRenderer
#if QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)
        , public QOpenGLFunctions
#endif
{
    DPTR_DECLARE_PRIVATE(OpenGLRendererBase)

public:

    virtual ~OpenGLRendererBase();

    bool isSupported(VideoFormat::PixelFormat pixfmt) const Q_DECL_OVERRIDE;
    OpenGLVideo* opengl() const Q_DECL_OVERRIDE;

protected:

    virtual bool receiveFrame(const VideoFrame& frame) Q_DECL_OVERRIDE;
    //called in paintEvent before drawFrame() when required
    virtual void drawBackground() Q_DECL_OVERRIDE;
    //draw the current frame using the current paint engine. called by paintEvent()
    virtual void drawFrame() Q_DECL_OVERRIDE;
    void onInitializeGL();
    void onPaintGL();
    void onResizeGL(int w, int h);
    void onResizeEvent(int w, int h);
    void onShowEvent();

private:

    void onSetOutAspectRatioMode(OutAspectRatioMode mode) Q_DECL_OVERRIDE;
    void onSetOutAspectRatio(qreal ratio) Q_DECL_OVERRIDE;
    bool onSetOrientation(int value) Q_DECL_OVERRIDE;
    bool onSetBrightness(qreal b) Q_DECL_OVERRIDE;
    bool onSetContrast(qreal c) Q_DECL_OVERRIDE;
    bool onSetHue(qreal h) Q_DECL_OVERRIDE;
    bool onSetSaturation(qreal s) Q_DECL_OVERRIDE;

protected:

    OpenGLRendererBase(OpenGLRendererBasePrivate &d);
};

} // namespace QtAV

#endif // QTAV_OPENGL_RENDERER_BASE_H
