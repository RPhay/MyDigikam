/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-06-15
 * Description : a brush for use with tool to replace part of the image using another
 *
 * Copyright (C) 2017 by Shaza Ismail Kaoud <shaza dot ismail dot k at gmail dot com>
 * Copyright (C) 2019 by Ahmed Fathi <ahmed dot fathi dot abdelmageed at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "healingclonetoolwidget.h"

// Qt includes

#include <QScrollBar>
#include <QPainter>
#include <QCursor>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "overlaywidget.h"
#include "previewlayout.h"

namespace DigikamEditorHealingCloneToolPlugin
{

class Q_DECL_HIDDEN HealingCloneToolWidget::Private
{
public:

    explicit Private()
      : srcSet                  (true),
        isLassoPointsVectorEmpty(true),
        src                     (QPoint(0, 0)),
        amIFocused              (false),
        proceedInMoveEvent      (false),
        cloneVectorChanged      (true),
        brushRadius             (1),
        brushValue              (1),
        currentState            (HealingCloneState::SELECT_SOURCE),
        previousState           (HealingCloneState::DO_NOTHING),
        drawCursor              (nullptr),
        sourceCursor            (nullptr),
        sourceCursorCenter      (nullptr)
    {
    }

    bool                  srcSet;
    bool                  isLassoPointsVectorEmpty;
    QPointF               lastCursorPosition;
    QPoint                src;
    QPoint                dst;
    bool                  amIFocused;
    bool                  proceedInMoveEvent;
    bool                  cloneVectorChanged;
    int                   brushRadius;
    int                   brushValue;
    HealingCloneState     currentState;
    HealingCloneState     previousState;
    QGraphicsEllipseItem* drawCursor;
    QGraphicsEllipseItem* sourceCursor;
    QGraphicsEllipseItem* sourceCursorCenter;
};

HealingCloneToolWidget::HealingCloneToolWidget(QWidget* const parent)
    : ImageRegionWidget(parent, false),
      d                (new Private)
{
    activateState(HealingCloneState::SELECT_SOURCE);
    updateSourceCursor(d->src, 10);

    connect(this, SIGNAL(viewportRectChanged(QRectF)),
            this, SLOT(slotImageRegionChanged()));
}

HealingCloneToolWidget::~HealingCloneToolWidget()
{
    delete d->drawCursor;
    delete d->sourceCursor;
    delete d->sourceCursorCenter;
    delete d;
}

void HealingCloneToolWidget::mousePressEvent(QMouseEvent* e)
{
    if      (!d->amIFocused                                 &&
             ((d->currentState == HealingCloneState::PAINT) ||
              (d->currentState == HealingCloneState::LASSO_CLONE)))
    {
        d->amIFocused = true;
        return;
    }
    else if (!d->amIFocused)
    {
        d->amIFocused = true;
    }

    d->proceedInMoveEvent = true;

    if (d->currentState == HealingCloneState::DO_NOTHING)
    {
        ImageRegionWidget::mousePressEvent(e);
        return;
    }

    if ((d->currentState == HealingCloneState::PAINT) ||
        (d->currentState == HealingCloneState::LASSO_CLONE))
    {
        if (d->cloneVectorChanged)
        {
            setCloneVectorChanged(false);

            emit signalPushToUndoStack();
        }
    }

    if      ((d->currentState == HealingCloneState::MOVE_IMAGE) &&
             (e->buttons() & Qt::LeftButton))
    {
        ImageRegionWidget::mousePressEvent(e);
    }
    else if (d->srcSet)
    {
        ImageRegionWidget::mousePressEvent(e);
    }
    else if ((d->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY) &&
             (e->buttons() & Qt::LeftButton))
    {
        QPoint dst = QPoint(e->x(), e->y());

        emit signalLasso(mapToImageCoordinates(dst));
    }
    else
    {
        if (e->button() == Qt::LeftButton)
        {
            d->dst = mapToImageCoordinates(e->pos());

            emit signalClone(d->src, d->dst);
        }
    }
}

void HealingCloneToolWidget::mouseMoveEvent(QMouseEvent* e)
{
    bool cursorOutsideScene = checkPointOutsideScene(e->pos());
    d->lastCursorPosition   = mapToScene(e->pos());

    if      (cursorOutsideScene &&
             (d->currentState != HealingCloneState::DO_NOTHING))
    {
        activateState(HealingCloneState::DO_NOTHING);
    }
    else if (!cursorOutsideScene &&
             (d->currentState == HealingCloneState::DO_NOTHING))
    {
        activateState(d->previousState);
    }

    setDrawCursorPosition(d->lastCursorPosition);

    if (d->currentState == HealingCloneState::DO_NOTHING)
    {
        ImageRegionWidget::mouseMoveEvent(e);
        return;
    }

    if (!d->proceedInMoveEvent)
    {
        return;
    }

    if      ((d->currentState == HealingCloneState::MOVE_IMAGE) &&
             (e->buttons() & Qt::LeftButton))
    {
        ImageRegionWidget::mouseMoveEvent(e);
    }
    else if ((d->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY) &&
             (e->buttons() & Qt::LeftButton))
    {
        QPoint dst = QPoint(e->x(), e->y());

        emit signalLasso(mapToImageCoordinates(dst));
    }
    else if ((e->buttons() & Qt::LeftButton) && !d->srcSet)
    {
        QPoint currentDst = mapToImageCoordinates(e->pos());
        QPoint currentSrc = d->src;
        QPoint orgDst     = d->dst;
        currentSrc        = QPoint(currentSrc.x() + currentDst.x() - orgDst.x(),
                                   currentSrc.y() + currentDst.y() - orgDst.y());

        // Source Cursor Update in scene coordinates

        QPointF tempCursorPosition = mapToScene(mapFromImageCoordinates(currentSrc));
        setSourceCursorPosition(tempCursorPosition);

        emit signalClone(currentSrc, currentDst);
    }

    if (d->srcSet)
    {
        ImageRegionWidget::mouseMoveEvent(e);
    }
}

void HealingCloneToolWidget::mouseReleaseEvent(QMouseEvent* e)
{
    ImageRegionWidget::mouseReleaseEvent(e);

    if (d->currentState == HealingCloneState::DO_NOTHING)
    {
        return;
    }

    if      (d->currentState == HealingCloneState::MOVE_IMAGE)
    {
/*
        setCursor(Qt::OpenHandCursor);
*/
        ImageRegionWidget::mouseReleaseEvent(e);
    }

    else if (d->srcSet)
    {
        d->src = mapToImageCoordinates(e->pos());
        setSourceCursorPosition(mapToScene(e->pos()));

        undoSlotSetSourcePoint();
    }
    else
    {
        QPointF tempCursorPosition = mapToScene(mapFromImageCoordinates(d->src));
        setSourceCursorPosition(tempCursorPosition);
    }
}

void HealingCloneToolWidget::mouseDoubleClickEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        if (d->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY)
        {
            slotLassoSelect();
        }
    }
}

void HealingCloneToolWidget::keyPressEvent(QKeyEvent* e)
{
    if      (e->key() == Qt::Key_M)
    {
        slotMoveImage();
    }
    else if (e->key() == Qt::Key_L)
    {
        slotLassoSelect();
    }

    if (e->key() == Qt::Key_BracketLeft)
    {
        emit signalDecreaseBrushRadius();
    }

    if (e->key() == Qt::Key_BracketRight)
    {
        emit signalIncreaseBrushRadius();
    }

    if (e->matches(QKeySequence::Undo))
    {
        emit signalUndoClone();
    }

    if (e->matches(QKeySequence::Redo))
    {
        emit signalRedoClone();
    }

    ImageRegionWidget::keyPressEvent(e);
}

bool HealingCloneToolWidget::event(QEvent* e)
{
    QKeyEvent* const keyEvent = static_cast<QKeyEvent*>(e);

    if (keyEvent && (keyEvent->key() == Qt::Key_Escape) &&
        (d->currentState != HealingCloneState::PAINT))
    {
        keyEvent->accept();

        if      (d->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY)
        {
            if (!d->isLassoPointsVectorEmpty)
            {
                slotLassoSelect();
            }

            slotLassoSelect();
        }
        else if (d->currentState == HealingCloneState::LASSO_CLONE)
        {
            slotLassoSelect();
        }

        return true;
    }

    return ImageRegionWidget::event(e);
}

void HealingCloneToolWidget::keyReleaseEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_S)
    {
        if (d->currentState == HealingCloneState::SELECT_SOURCE)
        {
            undoSlotSetSourcePoint();
        }
        else
        {
            slotSetSourcePoint();
        }
    }
}

void HealingCloneToolWidget::focusOutEvent(QFocusEvent*)
{
    d->amIFocused         = false;
    d->proceedInMoveEvent = false;
}

void HealingCloneToolWidget::focusInEvent(QFocusEvent*)
{
}

void HealingCloneToolWidget::slotSetSourcePoint()
{
    d->srcSet = true;
    activateState(HealingCloneState::SELECT_SOURCE);
}

void HealingCloneToolWidget::slotMoveImage()
{
    if (d->currentState == HealingCloneState::MOVE_IMAGE)
    {

        if (d->isLassoPointsVectorEmpty)
        {
            activateState(HealingCloneState::PAINT);
        }
        else
        {
            activateState(HealingCloneState::LASSO_CLONE);
            emit signalContinuePolygon();
        }
    }
    else
    {
        activateState(HealingCloneState::MOVE_IMAGE);
    }
}

void HealingCloneToolWidget::slotLassoSelect()
{
    if      ((d->currentState != HealingCloneState::LASSO_DRAW_BOUNDARY) &&
             (d->currentState != HealingCloneState::LASSO_CLONE))
    {
        activateState(HealingCloneState::LASSO_DRAW_BOUNDARY);

        emit signalResetLassoPoint();
    }
    else if (d->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY)
    {
        if (d->isLassoPointsVectorEmpty)
        {
            activateState(HealingCloneState::PAINT);
        }
        else
        {
            activateState(HealingCloneState::LASSO_CLONE);

            emit signalContinuePolygon();
        }
    }
    else if (d->currentState == HealingCloneState::LASSO_CLONE)
    {
        activateState(HealingCloneState::PAINT);

        emit signalResetLassoPoint();
    }
}

void HealingCloneToolWidget::undoSlotSetSourcePoint()
{
    d->srcSet = false;

    if (d->isLassoPointsVectorEmpty)
    {
        activateState(HealingCloneState::PAINT);
    }
    else
    {
        activateState(HealingCloneState::LASSO_CLONE);

        emit signalContinuePolygon();
    }
}

void HealingCloneToolWidget::changeCursorShape(const QColor& color)
{
    if (d->drawCursor)
    {
        scene()->removeItem(d->drawCursor);
        delete d->drawCursor;
    }

    int diameter = d->brushRadius * 2;

    d->drawCursor = new QGraphicsEllipseItem(0, 0, diameter, diameter);
    d->drawCursor->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);

    QPen pen(Qt::SolidLine);
    pen.setWidth(2);
    pen.setColor(color);
    d->drawCursor->setPen(pen);
    d->drawCursor->setBrush(QBrush(Qt::transparent));
    d->drawCursor->setOpacity(1);
    scene()->addItem(d->drawCursor);

    QPointF tempCursorPosition = mapToScene(mapFromImageCoordinates(d->src));
    updateSourceCursor(tempCursorPosition, diameter);
}

void HealingCloneToolWidget::setBrushValue(int value)
{
    d->brushValue = value;
    slotImageRegionChanged();
}

void HealingCloneToolWidget::setIsLassoPointsVectorEmpty(bool isEmpty)
{
    d->isLassoPointsVectorEmpty = isEmpty;
}

void HealingCloneToolWidget::activateState(HealingCloneState state)
{
    d->previousState = d->currentState;

    if (state != HealingCloneState::MOVE_IMAGE)
    {
        setDragMode(QGraphicsView::NoDrag);
    }

    if ((d->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY) &&
        (state != HealingCloneState::LASSO_CLONE))
    {
        emit signalContinuePolygon();
    }

    d->currentState = state;

    if      (state == HealingCloneState::PAINT)
    {
        changeCursorShape(Qt::blue);
        setCursor(QCursor(Qt::BlankCursor));
        setDrawCursorPosition(d->lastCursorPosition);
    }
    else if (state == HealingCloneState::MOVE_IMAGE)
    {
        if (QGraphicsView::dragMode() != QGraphicsView::ScrollHandDrag)
        {
            setDragMode(QGraphicsView::ScrollHandDrag);
        }
    }
    else if (state == HealingCloneState::LASSO_DRAW_BOUNDARY)
    {
        setCursor(QCursor(Qt::PointingHandCursor));
    }
    else if (state == HealingCloneState::LASSO_CLONE)
    {
        changeCursorShape(Qt::blue);
        setCursor(QCursor(Qt::BlankCursor));
        setDrawCursorPosition(d->lastCursorPosition);
    }
    else if (state == HealingCloneState::SELECT_SOURCE)
    {
        setCursor(QCursor(Qt::CrossCursor));
    }
    else if (state == HealingCloneState::DO_NOTHING)
    {
        setCursor(QCursor(Qt::ArrowCursor));
    }
}

void HealingCloneToolWidget::setCloneVectorChanged(bool changed)
{
    d->cloneVectorChanged = changed;
}

QPoint HealingCloneToolWidget::mapToImageCoordinates(const QPoint& point) const
{
    QPoint ret;
    ImageRegionItem* const region = dynamic_cast<ImageRegionItem*>(item());

    if (region)
    {
        QPointF temp = region->zoomSettings()->mapZoomToImage(mapToScene(point)) ;
        ret          = QPoint((int)temp.x(), (int)temp.y());
    }

    return ret;
}

QPoint HealingCloneToolWidget::mapFromImageCoordinates(const QPoint& point) const
{
    QPoint ret;
    ImageRegionItem* const region = dynamic_cast<ImageRegionItem*>(item());

    if (region)
    {
        ret = mapFromScene(region->zoomSettings()->mapImageToZoom(point));
    }

    return ret;
}

void HealingCloneToolWidget::updateSourceCursor(const QPointF& pos, int diameter)
{
    if (d->sourceCursor)
    {
        scene()->removeItem(d->sourceCursor);
        scene()->removeItem(d->sourceCursorCenter);
        delete d->sourceCursor;
        delete d->sourceCursorCenter;
    }

    d->sourceCursor       = new QGraphicsEllipseItem(0, 0, diameter, diameter);
    d->sourceCursorCenter = new QGraphicsEllipseItem(0, 0, 2, 2);
    d->sourceCursor->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
    d->sourceCursorCenter->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);

    QPen pen(Qt::DashDotDotLine);
    pen.setWidth(2);
    pen.setColor(Qt::black);
    d->sourceCursor->setPen(pen);
    d->sourceCursor->setBrush(QBrush(Qt::transparent));
    d->sourceCursor->setOpacity(1);
    scene()->addItem(d->sourceCursor);

    pen.setStyle(Qt::SolidLine);
    d->sourceCursorCenter->setPen(pen);
    d->sourceCursorCenter->setBrush(QBrush(Qt::black));
    d->sourceCursorCenter->setOpacity(1);
    scene()->addItem(d->sourceCursorCenter);

    setSourceCursorPosition(pos);
}

void HealingCloneToolWidget::setDrawCursorPosition(const QPointF& topLeftPos)
{
    if (!d->drawCursor)
    {
        return;
    }

    double d1          = d->drawCursor->rect().width() / 2.0;
    QPointF shiftedPos = QPointF(topLeftPos.x() - d1, topLeftPos.y() - d1);

    // Check if source is outside scene

    bool drawCursorOutsideScene = ((topLeftPos.x() < 0)                ||
                                   (topLeftPos.x() > scene()->width()) ||
                                   (topLeftPos.y() < 0)                ||
                                   (topLeftPos.y() > scene()->height()));

    if (drawCursorOutsideScene                         ||
        ((d->currentState != HealingCloneState::PAINT) &&
         (d->currentState != HealingCloneState::LASSO_CLONE)))
    {
        d->drawCursor->setVisible(false);
    }
    else
    {
        d->drawCursor->setPos(shiftedPos);

        d->drawCursor->setVisible(true);
    }
}

void HealingCloneToolWidget::setSourceCursorPosition(const QPointF& topLeftPos)
{
    double d1           = d->sourceCursor->rect().width() / 2.0;
    QPointF shiftedPos  = QPointF(topLeftPos.x() - d1, topLeftPos.y() - d1);

    double d2           = d->sourceCursorCenter->rect().width() / 2.0;
    QPointF shiftedPos2 = QPointF(topLeftPos.x() - d2, topLeftPos.y() - d2);

    // Check if source is outside scene

    bool sourceCursorOutsideScene = ((topLeftPos.x() < 0)                ||
                                     (topLeftPos.x() > scene()->width()) ||
                                     (topLeftPos.y() < 0)                ||
                                     (topLeftPos.y() > scene()->height()));

    if (sourceCursorOutsideScene)
    {
        d->sourceCursor->setVisible(false);
        d->sourceCursorCenter->setVisible(false);
    }
    else
    {
        d->sourceCursor->setPos(shiftedPos);
        d->sourceCursorCenter->setPos(shiftedPos2);

        d->sourceCursor->setVisible(true);
        d->sourceCursorCenter->setVisible(true);
    }
}

bool HealingCloneToolWidget::checkPointOutsideScene(const QPoint& globalPoint) const
{
    bool pointOutsideScene;
    QPointF temp = mapToScene(globalPoint);

    if (viewport()->width() > scene()->width())
    {
        pointOutsideScene  = ((temp.x() < 0)                ||
                              (temp.x() > scene()->width()) ||
                              (temp.y() < 0)                ||
                              (temp.y() > scene()->height()));
    }
    else
    {
        QPoint bottomRight = QPoint(viewport()->width(),
                                    viewport()->height());

        int right          = mapToScene(bottomRight).x();
        int left           = right - viewport()->width();
        int bottom         = mapToScene(bottomRight).y();
        int top            = bottom - viewport()->height();

        pointOutsideScene  = ((temp.x()     < left)  ||
                              (temp.x() + 1 > right) ||
                              (temp.y()     < top)   ||
                              (temp.y() + 1 > bottom));
    }

    return pointOutsideScene;
}

void HealingCloneToolWidget::slotImageRegionChanged()
{
    double zoom    = layout()->realZoomFactor();
    d->brushRadius = qRound(d->brushValue * zoom);

    activateState(d->currentState);

    if (!d->lastCursorPosition.isNull())
    {
        setDrawCursorPosition(d->lastCursorPosition);
    }
}

} // namespace DigikamEditorHealingCloneToolPlugin
