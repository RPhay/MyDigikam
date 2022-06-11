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

#ifndef QTAV_WIDGETS_GLWIDGET_RENDERER_H
#define QTAV_WIDGETS_GLWIDGET_RENDERER_H

#include "QtAVWidgets_Global.h"

#if !defined(QT_NO_OPENGL)

#   include <QOpenGLWidget>
#   include "OpenGLRendererBase.h"

namespace QtAV
{

class GLWidgetRendererPrivate;

/*!
 * \brief The GLWidgetRenderer class
 * Renderering video frames using GLSL. A more generic high level class OpenGLVideo is used internally.
 */
class DIGIKAM_EXPORT GLWidgetRenderer : public QOpenGLWidget,
                                        public OpenGLRendererBase
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(GLWidgetRenderer)
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

    explicit GLWidgetRenderer(QWidget* const parent = nullptr,
                              Qt::WindowFlags f = Qt::WindowFlags(Qt::Widget));

    virtual VideoRendererId id() const          override;

    virtual QWidget* widget()                   override
    {
        return this;
    }

    DECLARE_VIDEO_RENDERER_SIGNALS

protected:

    virtual void initializeGL()                 override;
    virtual void paintGL()                      override;
    virtual void resizeGL(int w, int h)         override;
    virtual void resizeEvent(QResizeEvent*)     override;
    virtual void showEvent(QShowEvent*)         override;

    DECLARE_VIDEO_RENDERER_EMIT_METHODS
};

typedef GLWidgetRenderer VideoRendererGLWidget;

} // namespace QtAV

#endif // QT_NO_OPENGL

#endif // QTAV_WIDGETS_GLWIDGET_RENDERER_H
