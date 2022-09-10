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

#ifndef QTAV_OPENGL_WINDOW_RENDERER_H
#define QTAV_OPENGL_WINDOW_RENDERER_H

#ifndef QT_NO_OPENGL
#   include <QOpenGLWindow>
#   include "OpenGLRendererBase.h"

namespace QtAV
{

class OpenGLWindowRendererPrivate;

class DIGIKAM_EXPORT OpenGLWindowRenderer : public QOpenGLWindow,
                                            public OpenGLRendererBase
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(OpenGLWindowRenderer)
    Q_PROPERTY(qreal brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged)
    Q_PROPERTY(qreal contrast READ contrast WRITE setContrast NOTIFY contrastChanged)
    Q_PROPERTY(qreal hue READ hue WRITE setHue NOTIFY hueChanged)
    Q_PROPERTY(qreal saturation READ saturation WRITE setSaturation NOTIFY saturationChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(QRectF regionOfInterest READ regionOfInterest WRITE setRegionOfInterest NOTIFY regionOfInterestChanged)
    Q_PROPERTY(qreal sourceAspectRatio READ sourceAspectRatio NOTIFY sourceAspectRatioChanged)
    Q_PROPERTY(qreal outAspectRatio READ outAspectRatio WRITE setOutAspectRatio NOTIFY outAspectRatioChanged)

    // fillMode

    // TODO: how to use enums in base class as property or Q_ENUM

    Q_PROPERTY(OutAspectRatioMode outAspectRatioMode READ outAspectRatioMode WRITE setOutAspectRatioMode NOTIFY outAspectRatioModeChanged)
    Q_PROPERTY(int orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(QRect videoRect READ videoRect NOTIFY videoRectChanged)
    Q_PROPERTY(QSize videoFrameSize READ videoFrameSize NOTIFY videoFrameSizeChanged)

public:

    explicit OpenGLWindowRenderer(UpdateBehavior updateBehavior = NoPartialUpdate,
                                  QWindow* const parent = nullptr);
    virtual VideoRendererId id() const              override;

    QWindow* qwindow()                              override final
    {
        return this;
    }

    DECLARE_VIDEO_RENDERER_SIGNALS

protected:

    virtual void initializeGL()                     override;
    virtual void paintGL()                          override;
    virtual void resizeGL(int w, int h)             override;
    virtual void resizeEvent(QResizeEvent*)         override;
    virtual void showEvent(QShowEvent*)             override;

    DECLARE_VIDEO_RENDERER_EMIT_METHODS
};

typedef OpenGLWindowRenderer VideoRendererOpenGLWindow;

} // namespace QtAV

#endif //QT_NO_OPENGL

#endif // QTAV_OPENGL_WINDOW_RENDERER_H
