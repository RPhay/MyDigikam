/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-04-30
 * Description : Graphics View for DImg preview
 *
 * SPDX-FileCopyrightText: 2010-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2011-2026 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "graphicsdimgview_p.h"

namespace Digikam
{

GraphicsDImgView::GraphicsDImgView(QWidget* const parent)
    : QGraphicsView(parent),
      d            (new Private)
{
    d->scene  = new QGraphicsScene(this);
    d->scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    setScene(d->scene);
    d->layout = new SinglePhotoPreviewLayout(this);
    d->layout->setGraphicsView(this);

    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);

    horizontalScrollBar()->setSingleStep(1);
    horizontalScrollBar()->setPageStep(1);
    verticalScrollBar()->setSingleStep(1);
    verticalScrollBar()->setPageStep(1);

    // Enable the gesture support

    grabGesture(Qt::PinchGesture);
    grabGesture(Qt::PanGesture);
    grabGesture(Qt::SwipeGesture);

    // ---

    d->magnifier = new MagnifierItem();
    d->magnifier->setVisible(false);
    d->magnifier->setZoomFactor(2.0);
    d->magnifier->setMagnifierSize(150);
    d->scene->addItem(d->magnifier);
    d->magnifier->setVisible(d->magnifierEnabled);      // FIXME: to hack.

    connect(d->layout, &SinglePhotoPreviewLayout::zoomFactorChanged,
            this, &GraphicsDImgView::slotZoomFactorChanged);

    // ---

    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(slotContentsMoved()));

    connect(verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(slotContentsMoved()));
}

GraphicsDImgView::~GraphicsDImgView()
{
    if (d->magnifier)
    {
        d->scene->removeItem(d->magnifier);
        delete d->magnifier;
    }

    delete d;
}

int GraphicsDImgView::contentsX() const
{
    return horizontalScrollBar()->value();
}

int GraphicsDImgView::contentsY() const
{
    return verticalScrollBar()->value();
}

void GraphicsDImgView::setContentsPos(int x, int y)
{
    horizontalScrollBar()->setValue(x);
    verticalScrollBar()->setValue(y);
}

void GraphicsDImgView::setShowText(bool val)
{
    d->showText = val;
}

void GraphicsDImgView::setScaleFitToWindow(bool value)
{
    d->layout->setScaleFitToWindow(value);
}

QRect GraphicsDImgView::visibleArea() const
{
    return (mapToScene(viewport()->geometry()).boundingRect().toRect());
}

void GraphicsDImgView::fitToWindow()
{
    d->layout->fitToWindow();
    update();
}

void GraphicsDImgView::toggleFullScreen(bool set)
{
    if (set)
    {
        d->scene->setBackgroundBrush(Qt::black);
        setFrameShape(QFrame::NoFrame);
    }
    else
    {
        d->scene->setBackgroundBrush(Qt::NoBrush);
        setFrameShape(QFrame::StyledPanel);
    }
}

void GraphicsDImgView::setItem(GraphicsDImgItem* const item)
{
    d->item = item;
    d->item->setDisplayWidget(this);

    d->scene->addItem(d->item);
    d->layout->addItem(d->item);
}

GraphicsDImgItem* GraphicsDImgView::item() const
{
    return d->item;
}

DImgPreviewItem* GraphicsDImgView::previewItem() const
{
    return dynamic_cast<DImgPreviewItem*>(item());
}

SinglePhotoPreviewLayout* GraphicsDImgView::layout() const
{
    return d->layout;
}

void GraphicsDImgView::drawForeground(QPainter* p, const QRectF& rect)
{
    QGraphicsView::drawForeground(p, rect);

    QString text = d->item->userLoadingHint();

    if (text.isNull() || !d->showText)
    {
        return;
    }

    QRect viewportRect        = viewport()->rect();
    QRect fontRect            = p->fontMetrics().boundingRect(viewportRect, 0, text);
    QPoint drawingPoint(viewportRect.topRight().x() - fontRect.width() - 10,
                        viewportRect.topRight().y() + 5);

    QPointF sceneDrawingPoint = mapToScene(drawingPoint);
    QRectF sceneDrawingRect(sceneDrawingPoint, fontRect.size());

    if (!rect.intersects(sceneDrawingRect))
    {
        return;
    }

    drawText(p, sceneDrawingRect, text);
}

void GraphicsDImgView::drawText(QPainter* p, const QRectF& rect, const QString& text)
{
    p->save();

    p->setRenderHint(QPainter::Antialiasing, true);
    p->setBackgroundMode(Qt::TransparentMode);

    // increase width by 5 and height by 2

    QRectF textRect    = rect.adjusted(0, 0, 5, 2);

    // Draw background

    p->setPen(Qt::black);
    QColor semiTransBg = palette().color(QPalette::Window);
    semiTransBg.setAlpha(190);
    p->setBrush(semiTransBg);
/*
    p->translate(0.5, 0.5);
*/
    p->drawRoundedRect(textRect, 10.0, 10.0);

    // Draw shadow and text

    p->setPen(palette().color(QPalette::Window).darker(115));
    p->drawText(textRect.translated(3, 1), text);
    p->setPen(palette().color(QPalette::WindowText));
    p->drawText(textRect.translated(2, 0), text);

    p->restore();
}

void GraphicsDImgView::slotZoomFactorChanged()
{
    if (d->magnifierEnabled && d->item)
    {
        updateMagnifier();
    }
}

void GraphicsDImgView::updateMagnifier()
{
    QPointF position = mapToScene(mapFromGlobal(QCursor::pos()));

    d->magnifier->setPos(position);

    QPointF imagePos = d->item->zoomSettings()->mapZoomToImage(position);

    // Compute the source zone depending of zoom

    qreal zoomFactor = d->item->zoomSettings()->zoomFactor();
    int halfSize     = d->magnifier->magnifierSize() / (2 * zoomFactor);

    QRectF sourceRect(
        imagePos.x() - halfSize,
        imagePos.y() - halfSize,
        d->magnifier->magnifierSize() / zoomFactor,
        d->magnifier->magnifierSize() / zoomFactor
    );

    // Clipping at image borders

    QRectF imageBounds;
    QSize size = d->item->image().size();
    imageBounds.setSize(size);
    sourceRect = sourceRect.intersected(imageBounds);

    // Check if the source rectangle is valid
    // Update magnifier with the source pixmap

    if (sourceRect.isEmpty())
    {
        d->magnifier->setVisible(false);
    }
    else
    {
        d->magnifier->setVisible(true);
        QPixmap currentPixmap = d->item->image().convertToPixmap();
        d->magnifier->setSourcePixmap(currentPixmap, sourceRect);
    }
}

} // namespace Digikam

#include "moc_graphicsdimgview.cpp"
