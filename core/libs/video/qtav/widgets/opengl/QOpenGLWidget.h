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

#ifndef QTAV_WIDGETS_QOPENGL_WIDGET_H
#define QTAV_WIDGETS_QOPENGL_WIDGET_H

#include "QtAVWidgets_Global.h"

// Qt includes

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#   error "Qt5 is required!"
#endif

#include <QOpenGLFunctions>
#include <QOpenGLPaintDevice>
#include <QSurfaceFormat>
#include <QWidget>

namespace QtAV
{

/*!
 * \brief The QOpenGLWidget class
 * A widget for rendering OpenGL graphics without QtOpenGL module
 */
class QTAV_WIDGETS_EXPORT QOpenGLWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(QOpenGLWidget)

public:

    explicit QOpenGLWidget(QWidget* parent = 0, Qt::WindowFlags f = Qt::WindowFlags(Qt::Widget));
    virtual ~QOpenGLWidget();
    void setFormat(const QSurfaceFormat &format);
    QSurfaceFormat format()                   const;
    bool isValid()                            const;
    void makeCurrent();
    void doneCurrent();
    QOpenGLContext *context()                 const;

protected:

    QPaintDevice* redirected(QPoint *offset)  const override;
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
    void paintEvent(QPaintEvent *e)                 override;
    void resizeEvent(QResizeEvent *e)               override;

private:

    void initialize();
    void render();
    void invokeUserPaint();

private:

    bool                m_initialized;
    bool                m_fakeHidden;
    QOpenGLContext*     m_context;
    QOpenGLPaintDevice* m_paintDevice;
    QSurfaceFormat      m_requestedFormat;
};

} // namespace QtAV

#endif //QTAV_WIDGETS_QOPENGL_WIDGET_H
