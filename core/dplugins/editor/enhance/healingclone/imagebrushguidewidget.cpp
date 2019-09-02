/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-06-15
 * Description : a brush for use with tool to replace part of the image using another
 *
 * Copyright (C) 2017      by Shaza Ismail Kaoud <shaza dot ismail dot k at gmail dot com>
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

#include "imagebrushguidewidget.h"
#include "overlaywidget.h"

// Local includes

#include "digikam_debug.h"

// KDE includes
#include <klocalizedstring.h>


namespace DigikamEditorHealingCloneToolPlugin
{

 ImageBrushGuideWidget::ImageBrushGuideWidget(QWidget* const parent):
     ImageRegionWidget(parent)
{

    activateState(HealingCloneState::SELECT_SOURCE);
    updateSourceCursor(src,10);
}

 void ImageBrushGuideWidget::setDefaults()
 {
     this->default_h = this->height();
     this->default_w = this->width();
     this->float_h = default_h;
     this->float_w = default_w;
    setFocus();
  //  this->resetPixels();

 }

 double ImageBrushGuideWidget::getScaleRatio()
 {

     return this->width()/this->default_w;

 }


 void ImageBrushGuideWidget::mousePressEvent(QMouseEvent* e)
 {


      if(!this->amIFocused &&
              (this->currentState == HealingCloneState::PAINT || this->currentState == HealingCloneState::LASSO_CLONE))
      {
          this->amIFocused = true;
          return;
      }
      else if(!this->amIFocused)
      {
          this->amIFocused = true;
      }

      proceedInMoveEvent = true;

      if( (this->currentState == HealingCloneState::PAINT || this->currentState == HealingCloneState::LASSO_CLONE))
      {
          if(this->cloneVectorChanged)
          {
              this->setCloneVectorChanged(false);
              qCDebug(DIGIKAM_DIMG_LOG()) << "emitting push to stack";
              emit signalPushToUndoStack();
          }
          else {
              qCDebug(DIGIKAM_DIMG_LOG()) << "clone vec didn't change - not pushing to undo stack";
          }
      }

      if (this->currentState == HealingCloneState::MOVE_IMAGE && (e->buttons() & Qt::LeftButton))
      {
          //setCursor(Qt::ClosedHandCursor);
          ImageRegionWidget::mousePressEvent(e);
      }
     else if (srcSet)
     {
         ImageRegionWidget::mousePressEvent(e);
     }
     else if (this->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY && (e->buttons() & Qt::LeftButton))
      {

         QPoint dst = QPoint(e->x(),e->y());
         emit signalLasso(mapToImageCoordinates(dst));
      }
     else
     {
         if (e->button() == Qt::LeftButton)
         {


             dst = mapToImageCoordinates(e->pos());
             emit signalClone(src, dst);

         }

     }




 }
void ImageBrushGuideWidget::mouseMoveEvent(QMouseEvent* e)
{
    QPointF temp = mapToScene(e->pos());
    bool cursorOutsideScene = temp.x() < 0 || temp.x()  > scene()->width() ||
                               temp.y()<0 || temp.y()  > scene()->height();


    if(cursorOutsideScene && this->cursor().shape() != Qt::ArrowCursor)
    {
        this->prevCursor = this->cursor();
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
    else if(!cursorOutsideScene && this->cursor().shape() == Qt::ArrowCursor) {
        qCDebug(DIGIKAM_DIMG_LOG()) << "SSSSSSS" << this->prevCursor;
        this->setCursor(this->prevCursor);
    }


    if(!proceedInMoveEvent)
        return;

    if ( this->currentState == HealingCloneState::MOVE_IMAGE && (e->buttons() & Qt::LeftButton))
    {

        ImageRegionWidget::mouseMoveEvent(e);

    }
    else if ( this->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY && (e->buttons() & Qt::LeftButton))
     {
        QPoint dst = QPoint(e->x(),e->y());
        emit signalLasso(mapToImageCoordinates(dst));
     }
    else if ((e->buttons() & Qt::LeftButton) && !srcSet)
    {


        QPoint currentDst   =   mapToImageCoordinates(e->pos());
        QPoint currentSrc   =   src;
        QPoint orgDst       =   dst;
        currentSrc          =   QPoint(currentSrc.x() + currentDst.x() - orgDst.x(), currentSrc.y() + currentDst.y() - orgDst.y());

        // Source Cursor Update
        QPointF tempCursorPosition = mapToScene(mapFromImageCoordinates(currentSrc)); // sceneCoordinates
        setSourceCursorPosition(tempCursorPosition);
        //

        emit signalClone(currentSrc, currentDst);

    }

    if (srcSet)
    {
        ImageRegionWidget::mouseMoveEvent(e);

    }


}

void ImageBrushGuideWidget::mouseReleaseEvent(QMouseEvent* e)
{


    ImageRegionWidget::mouseReleaseEvent(e);
    if (this->currentState == HealingCloneState::MOVE_IMAGE)
    {
       // setCursor(Qt::OpenHandCursor);
        ImageRegionWidget::mouseReleaseEvent(e);
    }

    else if (srcSet)
    {
        src = mapToImageCoordinates(e->pos());
        setSourceCursorPosition(mapToScene(e->pos()));

        undoSlotSetSourcePoint();

    }
    else
    {
        QPointF tempCursorPosition = mapToScene(mapFromImageCoordinates(src));
        setSourceCursorPosition(tempCursorPosition);

    }


}

void ImageBrushGuideWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::LeftButton )
    {
        if(this->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY)
        {
            slotLassoSelect();
        }
    }
}

void ImageBrushGuideWidget :: keyPressEvent(QKeyEvent *e)
{

    if(e->key() == Qt :: Key_M)
    {

        slotMoveImage();
    }

    else if(e->key() == Qt :: Key_L)
    {
        slotLassoSelect();
    }

    if(e->key() == Qt :: Key_BracketLeft)
    {
     emit signalDecreaseBrushRadius();
    }

    if(e->key() == Qt :: Key_BracketRight)
    {
        emit signalIncreaseBrushRadius();
    }

    if(e->matches(QKeySequence::Undo))
    {
       emit signalUndoClone();
    }

    if(e->matches(QKeySequence::Redo))
    {
       emit signalRedoClone();
    }

    QWidget::keyPressEvent(e);

}

bool ImageBrushGuideWidget::event(QEvent *event)
{
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent && keyEvent->key() == Qt::Key_Escape
                &&
     this->currentState != HealingCloneState::PAINT)
        {
          keyEvent->accept();

          if(this->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY)
          {
              if(!this->isLassoPointsVectorEmpty)
                slotLassoSelect();
              slotLassoSelect();
          }
          else if(this->currentState == HealingCloneState::LASSO_CLONE)
          {
              slotLassoSelect();
          }

          return true;
        }
        return QWidget::event(event);
}
void ImageBrushGuideWidget::  keyReleaseEvent(QKeyEvent *e)
{
    if(e->key() == Qt :: Key_S)
    {
        if(this->currentState == HealingCloneState::SELECT_SOURCE)
        {
            undoSlotSetSourcePoint();
        }
        else
        {
            slotSetSourcePoint();
        }
    }
}

void ImageBrushGuideWidget:: wheelEvent(QWheelEvent *e)
{

    ImageRegionWidget::wheelEvent(e);

}


void ImageBrushGuideWidget::focusOutEvent(QFocusEvent *event)
{
    this->amIFocused = false;
    proceedInMoveEvent = false;
}

void ImageBrushGuideWidget::focusInEvent(QFocusEvent *event)
{

}
void ImageBrushGuideWidget::slotSetSourcePoint()
{
    srcSet = true;
    activateState(HealingCloneState::SELECT_SOURCE);
}

void ImageBrushGuideWidget :: slotMoveImage()
{
    if(this->currentState == HealingCloneState::MOVE_IMAGE)
    {

        if(this->isLassoPointsVectorEmpty)
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
void ImageBrushGuideWidget :: slotLassoSelect()
{
    if(this->currentState != HealingCloneState::LASSO_DRAW_BOUNDARY &&
            this->currentState != HealingCloneState::LASSO_CLONE)
    {
        activateState(HealingCloneState::LASSO_DRAW_BOUNDARY);
        emit signalResetLassoPoint();

    }
    else if(this->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY) {

        if(this->isLassoPointsVectorEmpty)
        {
            activateState(HealingCloneState::PAINT);
        }
        else
        {
            activateState(HealingCloneState::LASSO_CLONE);
            emit signalContinuePolygon();
        }
      }
    else if(this->currentState == HealingCloneState::LASSO_CLONE)
    {
        activateState(HealingCloneState::PAINT);
        emit signalResetLassoPoint();


    }


}

void ImageBrushGuideWidget::undoSlotSetSourcePoint()
{
    srcSet = false;

    if(this->isLassoPointsVectorEmpty)
    {
        activateState(HealingCloneState::PAINT);
    }
    else
    {
        activateState(HealingCloneState::LASSO_CLONE);
        emit signalContinuePolygon();
    }
}
void ImageBrushGuideWidget::changeCursorShape(QColor color)
{

    int radius =this->brushRadius;
    int size = radius * 2;
    this->brushColor = color;
    int penSize = 2;
    QPixmap pix(size,size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setPen(QPen(color,penSize));
    p.setRenderHint(QPainter::Antialiasing, true);
    p.drawEllipse(1, 1, size-2, size-2);
    setCursor(QCursor(pix));

    QPointF tempCursorPosition = mapToScene(mapFromImageCoordinates(src));
    updateSourceCursor(tempCursorPosition, 2*this->brushRadius);

}

void ImageBrushGuideWidget::changeCursorShape(QPixmap pixMap, float x = 0.5 , float y = 0.5)
{
 setCursor(QCursor(pixMap,x * pixMap.width(),y *pixMap.height()));
}
void ImageBrushGuideWidget :: updateCursor()
{
    changeCursorShape(this->brushColor);
}
void ImageBrushGuideWidget::setBrushRadius(int value)
{
    this->brushRadius = value;
    activateState(this->currentState);
}



/*
void ImageBrushGuideWidget::showEvent( QShowEvent* event ) {
    ImageRegionWidget::showEvent( event );
    activateState(HealingCloneState::SELECT_SOURCE);
}
*/

void ImageBrushGuideWidget::setIsLassoPointsVectorEmpty(bool isEmpty)
{
    this->isLassoPointsVectorEmpty = isEmpty;
}
void ImageBrushGuideWidget :: activateState(HealingCloneState state)
{

    if(state != HealingCloneState::MOVE_IMAGE)
        setDragMode(QGraphicsView::NoDrag);

    if(this->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY &&
            state != HealingCloneState::LASSO_CLONE)
    {
        emit signalContinuePolygon();
    }
    this->currentState = state;
    if(state == HealingCloneState::PAINT)
    {
        changeCursorShape(Qt::blue);
    }
    else if(state == HealingCloneState::MOVE_IMAGE)
    {
       if(this->cursor().shape() != QGraphicsView::ScrollHandDrag)
         setDragMode(QGraphicsView::ScrollHandDrag);
    }
    else if(state == HealingCloneState::LASSO_DRAW_BOUNDARY)
    {
        QPixmap pix = QPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                     QLatin1String("digikam/data/healing_clone_LASSO_PEN.png")));
        changeCursorShape(pix,0,1);
    }
    else if(state == HealingCloneState::LASSO_CLONE)
    {
        changeCursorShape(Qt::blue);
    }
    else if(state == HealingCloneState::SELECT_SOURCE)
    {
  //      this->setSpotVisibleNoUpdate(true);
        QPixmap pix = QPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                     QLatin1String("digikam/data/healing_clone_SRC.png")));
        changeCursorShape(pix,0.5,0.5);

    }
}

void ImageBrushGuideWidget::setCloneVectorChanged(bool changed)
{
    this->cloneVectorChanged = changed;
}

QPoint ImageBrushGuideWidget::mapToImageCoordinates(QPoint point)
{

    ImageRegionItem* item = (ImageRegionItem*)this->item();
    QPointF temp = item->zoomSettings()->mapZoomToImage(mapToScene(point)) ;
    return QPoint((int) temp.x(), (int) temp.y());
}

QPoint ImageBrushGuideWidget::mapFromImageCoordinates(QPoint point)
{

    ImageRegionItem* item = (ImageRegionItem*)this->item();
    return mapFromScene(item->zoomSettings()->mapImageToZoom(point));
}

void ImageBrushGuideWidget::updateSourceCursor(QPointF pos, int diameter)
{
   if(this->sourceCursor != nullptr)
    {
        this->scene()->removeItem(this->sourceCursor);
        delete this->sourceCursor;
    }
    this->sourceCursor = new QGraphicsEllipseItem(0, 0, diameter, diameter);
    this->sourceCursor->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
    this->sourceCursor->setBrush(QBrush(Qt::white));
    this->sourceCursor->setOpacity(.25);
    this->scene()->addItem(this->sourceCursor);
    setSourceCursorPosition(pos);
}

void ImageBrushGuideWidget::setSourceCursorPosition(QPointF topLeftPos)
{


    double dx = this->sourceCursor->rect().width()/2.0;
    double dy = this->sourceCursor->rect().width()/2.0;
    QPointF shiftedPos = QPointF(topLeftPos.x()-dx, topLeftPos.y()-dy);
    this->sourceCursor->setPos(shiftedPos);

    // check if source is outside scene

    bool sourceCursorOutsideScene =
            topLeftPos.x() < 0 || topLeftPos.x() + dx > scene()->width() ||
            topLeftPos.y()<0 || topLeftPos.y() + dy > scene()->height();
    if(sourceCursorOutsideScene)
    {
        this->sourceCursor->setVisible(false);
    }
    else {
        this->sourceCursor->setVisible(true);
    }


}
} // namespace DigikamEditorHealingCloneToolPlugin
