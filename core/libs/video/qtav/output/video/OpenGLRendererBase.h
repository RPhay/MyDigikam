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

#ifndef QTAV_OPENGL_RENDERER_BASE_H
#define QTAV_OPENGL_RENDERER_BASE_H

#include "VideoRenderer.h"

// Qt includes

#include <QOpenGLFunctions>

namespace QtAV
{

/*!
 * \brief The OpenGLRendererBase class
 * Renderering video frames using GLSL.
 * A more generic high level class OpenGLVideo is used internally.
 * TODO: for Qt5, no QtOpenGL, use QWindow instead.
 */
class OpenGLRendererBasePrivate;

class DIGIKAM_EXPORT OpenGLRendererBase : public VideoRenderer,
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

    // draw the current frame using the current paint engine.
    // called by paintEvent()

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
