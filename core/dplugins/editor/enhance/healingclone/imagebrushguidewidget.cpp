/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-06-15
 * Description : a brush for use with tool to replace part of the image using another
 *
 * Copyright (C) 2017      by Shaza Ismail Kaoud <shaza dot ismail dot k at gmail dot com>
 * Copyright (C) 2019      by Ahmed Fathi <ahmed dot fathi dot abdelmageed at gmail dot com>
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

#include "imagebrushguidewidget.h"

// Qt includes

#include <QScrollBar>
#include <QPainter>
#include <QStandardPaths>
#include <QCursor>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "overlaywidget.h"

namespace DigikamEditorHealingCloneToolPlugin
{

class Q_DECL_HIDDEN ImageBrushGuideWidget::Private
{

public:

    explicit Private()
      : srcSet(true),
        isLassoPointsVectorEmpty(true),
        default_w(0.0),
        default_h(0.0),
        float_w(0.0),
        float_h(0.0),
        amIFocused(false),
        proceedInMoveEvent(false),
        cloneVectorChanged(true),
        brushRadius(1),
        currentState(HealingCloneState::SELECT_SOURCE),
        previousState(HealingCloneState::DO_NOTHING),
        sourceCursor(nullptr),
        sourceCursorCenter(nullptr)
        
    {
        src        = QPoint(0, 0);
        brushColor = QColor(Qt::red);
    }

    bool                  srcSet;
    bool                  isLassoPointsVectorEmpty;
    QPoint                src;
    QPoint                dst;
    double                default_w;
    double                default_h;
    double                float_w;
    double                float_h;
    bool                  amIFocused;
    bool                  proceedInMoveEvent;
    bool                  cloneVectorChanged;
    int                   brushRadius;
    QColor                brushColor;
    HealingCloneState     currentState;
    HealingCloneState     previousState;
    QGraphicsEllipseItem* sourceCursor;
    QGraphicsEllipseItem* sourceCursorCenter;
    QCursor               prevCursor;
};

ImageBrushGuideWidget::ImageBrushGuideWidget(QWidget* const parent)
    : ImageRegionWidget(parent),
      d(new Private)
{
    activateState(HealingCloneState::SELECT_SOURCE);
    updateSourceCursor(d->src, 10);
}

ImageBrushGuideWidget::~ImageBrushGuideWidget()
{
    delete d;
}

void ImageBrushGuideWidget::mousePressEvent(QMouseEvent* e)
{
    if (!d->amIFocused &&
        (d->currentState == HealingCloneState::PAINT ||
         d->currentState == HealingCloneState::LASSO_CLONE))
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

    if ((d->currentState == HealingCloneState::PAINT ||
         d->currentState == HealingCloneState::LASSO_CLONE))
    {
        if (d->cloneVectorChanged)
        {
            setCloneVectorChanged(false);
            emit signalPushToUndoStack();
        }
    }

    if (d->currentState == HealingCloneState::MOVE_IMAGE &&
        (e->buttons() & Qt::LeftButton))
    {
        ImageRegionWidget::mousePressEvent(e);
    }
    else if (d->srcSet)
    {
        ImageRegionWidget::mousePressEvent(e);
    }
    else if (d->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY &&
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

void ImageBrushGuideWidget::mouseMoveEvent(QMouseEvent* e)
{
    bool cursorOutsideScene = checkPointOutsideScene(e->pos());

    if (cursorOutsideScene &&
        d->currentState != HealingCloneState::DO_NOTHING)
    {
        activateState(HealingCloneState::DO_NOTHING);
    }
    else if (!cursorOutsideScene &&
             d->currentState == HealingCloneState::DO_NOTHING)
    {
        activateState(d->previousState);
    }

    if (d->currentState == HealingCloneState::DO_NOTHING)
    {
        ImageRegionWidget::mouseMoveEvent(e);
        return;
    }

    if (!d->proceedInMoveEvent)
    {
        return;
    }

    if (d->currentState == HealingCloneState::MOVE_IMAGE &&
        (e->buttons() & Qt::LeftButton))
    {
        ImageRegionWidget::mouseMoveEvent(e);
    }
    else if (d->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY &&
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

        // Source Cursor Update
        QPointF tempCursorPosition = mapToScene(mapFromImageCoordinates(currentSrc)); // sceneCoordinates
        setSourceCursorPosition(tempCursorPosition);

        emit signalClone(currentSrc, currentDst);
    }

    if (d->srcSet)
    {
        ImageRegionWidget::mouseMoveEvent(e);
    }
}

void ImageBrushGuideWidget::mouseReleaseEvent(QMouseEvent* e)
{
    ImageRegionWidget::mouseReleaseEvent(e);

    if (d->currentState == HealingCloneState ::DO_NOTHING)
    {
        return;
    }

    if (d->currentState == HealingCloneState::MOVE_IMAGE)
    {
//      setCursor(Qt::OpenHandCursor);
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

void ImageBrushGuideWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (d->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY)
        {
            slotLassoSelect();
        }
    }
}

void ImageBrushGuideWidget :: keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_M)
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

    QWidget::keyPressEvent(e);
}

bool ImageBrushGuideWidget::event(QEvent *event)
{
    QKeyEvent* const keyEvent = static_cast<QKeyEvent*>(event);
    
    if (keyEvent && keyEvent->key() == Qt::Key_Escape &&
        d->currentState != HealingCloneState::PAINT)
    {
        keyEvent->accept();

        if (d->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY)
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

    return QWidget::event(event);
}

void ImageBrushGuideWidget::keyReleaseEvent(QKeyEvent* e)
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

void ImageBrushGuideWidget:: wheelEvent(QWheelEvent* e)
{
    ImageRegionWidget::wheelEvent(e);
}

void ImageBrushGuideWidget::focusOutEvent(QFocusEvent*)
{
    d->amIFocused         = false;
    d->proceedInMoveEvent = false;
}

void ImageBrushGuideWidget::focusInEvent(QFocusEvent*)
{
}

void ImageBrushGuideWidget::slotSetSourcePoint()
{
    d->srcSet = true;
    activateState(HealingCloneState::SELECT_SOURCE);
}

void ImageBrushGuideWidget::slotMoveImage()
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

void ImageBrushGuideWidget::slotLassoSelect()
{
    if (d->currentState != HealingCloneState::LASSO_DRAW_BOUNDARY &&
        d->currentState != HealingCloneState::LASSO_CLONE)
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

void ImageBrushGuideWidget::undoSlotSetSourcePoint()
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

void ImageBrushGuideWidget::changeCursorShape(const QColor& color)
{
    int radius    = d->brushRadius;
    int size      = radius * 2;
    d->brushColor = color;
    int penSize   = 2;
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setPen(QPen(color, penSize));
    p.setRenderHint(QPainter::Antialiasing, true);
    p.drawEllipse(1, 1, size - 2, size - 2);
    p.setBrush(Qt::SolidPattern);
    p.drawEllipse((size - 2) / 2, (size - 2) / 2, 2, 2);

    setCursor(QCursor(pix));

    QPointF tempCursorPosition = mapToScene(mapFromImageCoordinates(d->src));
    updateSourceCursor(tempCursorPosition, 2 * d->brushRadius);
}

void ImageBrushGuideWidget::changeCursorShape(const QPixmap& pixMap, float x, float y)
{
    setCursor(QCursor(pixMap, x * pixMap.width(), y * pixMap.height()));
}

void ImageBrushGuideWidget::updateCursor()
{
    changeCursorShape(d->brushColor);
}

void ImageBrushGuideWidget::setBrushRadius(int value)
{
    d->brushRadius = value;
    activateState(d->currentState);
}

void ImageBrushGuideWidget::setIsLassoPointsVectorEmpty(bool isEmpty)
{
    d->isLassoPointsVectorEmpty = isEmpty;
}

void ImageBrushGuideWidget::activateState(HealingCloneState state)
{
    d->previousState = d->currentState;

    if (state != HealingCloneState::MOVE_IMAGE)
    {
        setDragMode(QGraphicsView::NoDrag);
    }

    if (d->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY &&
        state != HealingCloneState::LASSO_CLONE)
    {
        emit signalContinuePolygon();
    }

    d->currentState = state;

    if (state == HealingCloneState::PAINT)
    {
        changeCursorShape(Qt::blue);
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
        QPixmap pix = QPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                     QLatin1String("digikam/data/healing_clone_LASSO_PEN.png")));
        changeCursorShape(pix, 0.0, 1.0);
    }
    else if (state == HealingCloneState::LASSO_CLONE)
    {
        changeCursorShape(Qt::blue);
    }
    else if (state == HealingCloneState::SELECT_SOURCE)
    {

        QPixmap pix = QPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                     QLatin1String("digikam/data/healing_clone_SRC.png")));
        changeCursorShape(pix, 0.5, 0.5);
    }
    else if (state ==HealingCloneState::DO_NOTHING)
    {
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
}

void ImageBrushGuideWidget::setCloneVectorChanged(bool changed)
{
    d->cloneVectorChanged = changed;
}

QPoint ImageBrushGuideWidget::mapToImageCoordinates(const QPoint& point)
{
    ImageRegionItem* const region = (ImageRegionItem*)item();
    QPointF temp                  = region->zoomSettings()->mapZoomToImage(mapToScene(point)) ;
    return QPoint((int) temp.x(), (int) temp.y());
}

QPoint ImageBrushGuideWidget::mapFromImageCoordinates(const QPoint& point)
{
    ImageRegionItem* const region = (ImageRegionItem*)item();
    return mapFromScene(region->zoomSettings()->mapImageToZoom(point));
}

void ImageBrushGuideWidget::updateSourceCursor(const QPointF& pos, int diameter)
{
    if (d->sourceCursor != nullptr)
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

void ImageBrushGuideWidget::setSourceCursorPosition(const QPointF& topLeftPos)
{
    double dx           = d->sourceCursor->rect().width() / 2.0;
    double dy           = d->sourceCursor->rect().width() / 2.0;
    QPointF shiftedPos  = QPointF(topLeftPos.x() - dx, topLeftPos.y() - dy);

    double dx2          = d->sourceCursorCenter->rect().width() / 2.0;
    double dy2          = d->sourceCursorCenter->rect().width() / 2.0;
    QPointF shiftedPos2 = QPointF(topLeftPos.x() - dx2, topLeftPos.y() - dy2);

    d->sourceCursor->setPos(shiftedPos);
    d->sourceCursorCenter->setPos(shiftedPos2);

    // check if source is outside scene

    bool sourceCursorOutsideScene = topLeftPos.x() < 0 || topLeftPos.x() + dx > scene()->width() ||
                                    topLeftPos.y() < 0 || topLeftPos.y() + dy > scene()->height();

    if (sourceCursorOutsideScene)
    {
        d->sourceCursor->setVisible(false);
        d->sourceCursorCenter->setVisible(false);
    }
    else
    {
        d->sourceCursor->setVisible(true);
        d->sourceCursorCenter->setVisible(true);
    }
}

bool ImageBrushGuideWidget::checkPointOutsideScene(const QPoint& globalPoint)
{
    bool pointOutsideScene;
    QPointF temp = mapToScene(globalPoint);

    if (viewport()->width() > scene()->width())
    {
        pointOutsideScene = temp.x() < 0 || temp.x() > scene()->width() ||
                            temp.y() < 0 || temp.y() > scene()->height();
    }
    else
    {
        QPoint bottomRight = QPoint(viewport()->width() - 1, viewport()->height() - 1);
        int right          = mapToScene(bottomRight).x();
        int bottom         = mapToScene(bottomRight).y();

        pointOutsideScene  = temp.x() <= 0 || temp.x() >= right ||
                             temp.y() <= 0 || temp.y() >= bottom;
    }

    return pointOutsideScene;
}

} // namespace DigikamEditorHealingCloneToolPlugin
