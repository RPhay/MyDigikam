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

#include "OpenGLWidgetRenderer.h"

// Qt includes

#include <QGuiApplication>
#include <QResizeEvent>
#include <QScreen>

// Local includes

#include "OpenGLRendererBase_p.h"

namespace QtAV
{

class Q_DECL_HIDDEN OpenGLWidgetRendererPrivate : public OpenGLRendererBasePrivate
{
public:

    explicit OpenGLWidgetRendererPrivate(QPaintDevice* const pd)
        : OpenGLRendererBasePrivate(pd)
    {
    }
};

VideoRendererId OpenGLWidgetRenderer::id() const
{
    return VideoRendererId_OpenGLWidget;
}

OpenGLWidgetRenderer::OpenGLWidgetRenderer(QWidget* const parent, Qt::WindowFlags f)
    : QOpenGLWidget     (parent, f),
      OpenGLRendererBase(*new OpenGLWidgetRendererPrivate(this))
{
    setAcceptDrops(true);
    setFocusPolicy(Qt::StrongFocus);
}

void OpenGLWidgetRenderer::initializeGL()
{
    onInitializeGL();
}

void OpenGLWidgetRenderer::paintGL()
{
    onPaintGL();
}

void OpenGLWidgetRenderer::resizeGL(int w, int h)
{
    // QGLWidget uses window()->windowHandle()->devicePixelRatio() for resizeGL(),
    // while QOpenGLWidget does not, so scale here

    if (!context())
        return;

    const qreal dpr = context()->screen()->devicePixelRatio();

    onResizeGL(w * dpr, h * dpr);
}

void OpenGLWidgetRenderer::resizeEvent(QResizeEvent* e)
{
    onResizeEvent(e->size().width(), e->size().height());
    QOpenGLWidget::resizeEvent(e); // will call resizeGL(). TODO: will call paintEvent()?
}

void OpenGLWidgetRenderer::showEvent(QShowEvent*)
{
    onShowEvent(); // TODO: onShowEvent(w, h)?
    resizeGL(width(), height());
}

IMPLEMENT_VIDEO_RENDERER_EMIT_METHODS(OpenGLWidgetRenderer)

} // namespace QtAV
