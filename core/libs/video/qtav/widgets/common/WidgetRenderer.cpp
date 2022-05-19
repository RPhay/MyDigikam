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

#include "WidgetRenderer.h"

// Qt includes

#include <QFont>
#include <QPainter>
#include <QApplication>
#include <QResizeEvent>

// Local includes

#include "QPainterRenderer_p.h"
#include "Filter.h"
#include "digikam_debug.h"

namespace QtAV
{

class Q_DECL_HIDDEN WidgetRendererPrivate : public QPainterRendererPrivate
{
public:

    virtual ~WidgetRendererPrivate()
    {
    }
};

VideoRendererId WidgetRenderer::id() const
{
    return VideoRendererId_Widget;
}

WidgetRenderer::WidgetRenderer(QWidget* parent, Qt::WindowFlags f)
    : QWidget(parent, f),
      QPainterRenderer(*new WidgetRendererPrivate())
{
    DPTR_D(WidgetRenderer);

    d.painter = new QPainter();
    setAcceptDrops(true);
    setFocusPolicy(Qt::StrongFocus);

    /* To rapidly update custom widgets that constantly paint over their entire areas with
     * opaque content, e.g., video streaming widgets, it is better to set the widget's
     * Qt::WA_OpaquePaintEvent, avoiding any unnecessary overhead associated with repainting the
     * widget's background
     */
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAutoFillBackground(false);
    QPainterFilterContext* ctx = static_cast<QPainterFilterContext*>(d.filter_context);

    if (ctx)
    {
        ctx->painter = d.painter;
    }
    else
    {
        qCWarning(DIGIKAM_QTAVWIDGETS_LOG_WARN).noquote() << QString::asprintf("FilterContext not available!");
    }
}

WidgetRenderer::WidgetRenderer(WidgetRendererPrivate &d, QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f),
      QPainterRenderer(d)
{
    d.painter = new QPainter();
    setAcceptDrops(true);
    setFocusPolicy(Qt::StrongFocus);
    setAutoFillBackground(false);
    QPainterFilterContext *ctx = static_cast<QPainterFilterContext*>(d.filter_context);

    if (ctx)
    {
        ctx->painter = d.painter;
    }
    else
    {
        qCWarning(DIGIKAM_QTAVWIDGETS_LOG_WARN).noquote() << QString::asprintf("FilterContext not available!");
    }
}

bool WidgetRenderer::receiveFrame(const VideoFrame& frame)
{
    preparePixmap(frame);
    updateUi();

    /*
     * workaround for the widget not updated if has parent. don't know why it works and why update() can't
     * Thanks to Vito Covito and Carlo Scarpato
     * Now it's fixed by posting a QUpdateLaterEvent
     */
    Q_EMIT imageReady();

    return true;
}

void WidgetRenderer::resizeEvent(QResizeEvent *e)
{
    DPTR_D(WidgetRenderer);

    d.update_background = true;
    resizeRenderer(e->size());
    update();
}

void WidgetRenderer::paintEvent(QPaintEvent *)
{
    DPTR_D(WidgetRenderer);

    d.painter->begin(this); // Widget painting can only begin as a result of a paintEvent
    handlePaintEvent();

    if (d.painter->isActive())
        d.painter->end();
}

bool WidgetRenderer::onSetOrientation(int value)
{
    Q_UNUSED(value);
    update();

    return true;
}

void WidgetRenderer::emitSourceAspectRatioChanged(qreal v)
{
   Q_EMIT sourceAspectRatioChanged(v);
}

void WidgetRenderer::emitOutAspectRatioChanged()
{
   Q_EMIT outAspectRatioChanged();
}

void WidgetRenderer::emitOutAspectRatioModeChanged()
{
   Q_EMIT outAspectRatioModeChanged();
}

void WidgetRenderer::emitOrientationChanged()
{
   Q_EMIT orientationChanged();
}

void WidgetRenderer::emitVideoRectChanged()
{
   Q_EMIT videoRectChanged();
}

void WidgetRenderer::emitContentRectChanged()
{
   Q_EMIT contentRectChanged();
}

void WidgetRenderer::emitRegionOfInterestChanged()
{
   Q_EMIT regionOfInterestChanged();
}

void WidgetRenderer::emitVideoFrameSizeChanged()
{
   Q_EMIT videoFrameSizeChanged();
}

void WidgetRenderer::emitRendererSizeChanged()
{
   Q_EMIT rendererSizeChanged();
}

void WidgetRenderer::emitBrightnessChanged(qreal v)
{
   Q_EMIT brightnessChanged(v);
}

void WidgetRenderer::emitContrastChanged(qreal v)
{
   Q_EMIT contrastChanged(v);
}

void WidgetRenderer::emitHueChanged(qreal v)
{
   Q_EMIT hueChanged(v);
}

void WidgetRenderer::emitSaturationChanged(qreal v)
{
   Q_EMIT saturationChanged(v);
}

void WidgetRenderer::emitBackgroundColorChanged()
{
   Q_EMIT backgroundColorChanged();
}

} // namespace QtAV
