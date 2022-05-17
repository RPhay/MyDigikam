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

class QTAV_EXPORT OpenGLRendererBase : public VideoRenderer,
                                       public QOpenGLFunctions
{
    DPTR_DECLARE_PRIVATE(OpenGLRendererBase)

public:

    virtual ~OpenGLRendererBase();

    bool isSupported(VideoFormat::PixelFormat pixfmt) const override;
    OpenGLVideo* opengl() const                             override;

protected:

    virtual bool receiveFrame(const VideoFrame& frame)      override;

    // called in paintEvent before drawFrame() when required

    virtual void drawBackground()                           override;

    // draw the current frame using the current paint engine. called by paintEvent()

    virtual void drawFrame()                                override;
    void onInitializeGL();
    void onPaintGL();
    void onResizeGL(int w, int h);
    void onResizeEvent(int w, int h);
    void onShowEvent();

private:

    void onSetOutAspectRatioMode(OutAspectRatioMode mode)   override;
    void onSetOutAspectRatio(qreal ratio)                   override;
    bool onSetOrientation(int value)                        override;
    bool onSetBrightness(qreal b)                           override;
    bool onSetContrast(qreal c)                             override;
    bool onSetHue(qreal h)                                  override;
    bool onSetSaturation(qreal s)                           override;

protected:

    explicit OpenGLRendererBase(OpenGLRendererBasePrivate& d);
};

} // namespace QtAV

#endif // QTAV_OPENGL_RENDERER_BASE_H
