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
#include "VideoRenderer.h"

// Qt includes

#include <QGLWidget>

// TODO: QGLFunctions is in Qt4.8+. meego is 4.7

#define QTAV_HAVE_QGLFUNCTIONS QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)

#if QTAV_HAVE(QGLFUNCTIONS)
#   include <QGLFunctions>
#endif

namespace QtAV
{

class GLWidgetRendererPrivate;

class QTAV_WIDGETS_EXPORT GLWidgetRenderer : public QGLWidget
                                          , public VideoRenderer

#if QTAV_HAVE(QGLFUNCTIONS) // TODO: why use QT_VERSION will result in moc error?

                                          , public QGLFunctions

#endif

{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(GLWidgetRenderer)

public:

    GLWidgetRenderer(QWidget* parent = 0, const QGLWidget* shareWidget = 0, Qt::WindowFlags f = Qt::WindowFlags(Qt::Widget));
    virtual VideoRendererId id() const;
    virtual bool isSupported(VideoFormat::PixelFormat pixfmt) const;

    virtual QWidget* widget()
    {
        return this;
    }

protected:

    virtual bool receiveFrame(const VideoFrame& frame);
    virtual bool needUpdateBackground() const;

    // called in paintEvent before drawFrame() when required

    virtual void drawBackground();

    // draw the current frame using the current paint engine. called by paintEvent()

    virtual void drawFrame();
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);
    virtual void resizeEvent(QResizeEvent *);
    virtual void showEvent(QShowEvent *);

private:

    virtual void onSetOutAspectRatioMode(OutAspectRatioMode mode);
    virtual void onSetOutAspectRatio(qreal ratio);
    virtual bool onSetOrientation(int value);

    /*!
     * \brief onSetBrightness
     *  only works for GLSL. otherwise return false, means that do nothing, brightness() does not change.
     * \return
     */
    virtual bool onSetBrightness(qreal b);
    virtual bool onSetContrast(qreal c);
    virtual bool onSetHue(qreal h);
    virtual bool onSetSaturation(qreal s);

};

typedef GLWidgetRenderer VideoRendererGLWidget;

} // namespace QtAV

#endif // QTAV_WIDGETS_GLWIDGET_RENDERER_H
