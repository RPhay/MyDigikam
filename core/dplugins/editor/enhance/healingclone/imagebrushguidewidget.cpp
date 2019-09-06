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
#include "overlaywidget.h"

// Local includes

#include "digikam_debug.h"

// KDE includes
#include <klocalizedstring.h>
#include <QScrollBar>


namespace DigikamEditorHealingCloneToolPlugin
{

 ImageBrushGuideWidget::ImageBrushGuideWidget(QWidget* const parent):
     ImageRegionWidget(parent)
{

    activateState(HealingCloneState::SELECT_SOURCE);
    updateSourceCursor(src,10);
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

      if(this->currentState == HealingCloneState ::DO_NOTHING)
      {
          ImageRegionWidget::mousePressEvent(e);
          return;
      }

      if( (this->currentState == HealingCloneState::PAINT || this->currentState == HealingCloneState::LASSO_CLONE))
      {
          if(this->cloneVectorChanged)
          {
              this->setCloneVectorChanged(false);
              emit signalPushToUndoStack();
          }

      }

      if (this->currentState == HealingCloneState::MOVE_IMAGE && (e->buttons() & Qt::LeftButton))
      {
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


    bool cursorOutsideScene = checkPointOutsideScene(e->pos());



    if(cursorOutsideScene && this->currentState != HealingCloneState::DO_NOTHING)
    {
        activateState(HealingCloneState::DO_NOTHING);
    }
    else if(!cursorOutsideScene && this->currentState == HealingCloneState::DO_NOTHING) {
        activateState(this->previousState);
    }

    if(this->currentState == HealingCloneState ::DO_NOTHING)
    {
        ImageRegionWidget::mouseMoveEvent(e);
        return;
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

    if(this->currentState == HealingCloneState ::DO_NOTHING)
    {
        return;
    }
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


void ImageBrushGuideWidget::focusOutEvent(QFocusEvent *)
{
    this->amIFocused = false;
    proceedInMoveEvent = false;
}

void ImageBrushGuideWidget::focusInEvent(QFocusEvent *)
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
void ImageBrushGuideWidget::changeCursorShape(const QColor& color)
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
    p.setBrush(Qt::SolidPattern);
    p.drawEllipse((size-2)/2,(size-2)/2,2,2);

    setCursor(QCursor(pix));

    QPointF tempCursorPosition = mapToScene(mapFromImageCoordinates(src));
    updateSourceCursor(tempCursorPosition, 2*this->brushRadius);

}

void ImageBrushGuideWidget::changeCursorShape(const QPixmap& pixMap, float x = 0.5 , float y = 0.5)
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





void ImageBrushGuideWidget::setIsLassoPointsVectorEmpty(bool isEmpty)
{
    this->isLassoPointsVectorEmpty = isEmpty;
}
void ImageBrushGuideWidget :: activateState(HealingCloneState state)
{

    this->previousState = this->currentState;

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
       if(QGraphicsView::dragMode() != QGraphicsView::ScrollHandDrag)
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

        QPixmap pix = QPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                     QLatin1String("digikam/data/healing_clone_SRC.png")));
        changeCursorShape(pix,0.5,0.5);

    }
    else if(state ==HealingCloneState::DO_NOTHING)
    {
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
}

void ImageBrushGuideWidget::setCloneVectorChanged(bool changed)
{
    this->cloneVectorChanged = changed;
}

QPoint ImageBrushGuideWidget::mapToImageCoordinates(const QPoint& point)
{

    ImageRegionItem* item = (ImageRegionItem*)this->item();
    QPointF temp = item->zoomSettings()->mapZoomToImage(mapToScene(point)) ;
    return QPoint((int) temp.x(), (int) temp.y());
}

QPoint ImageBrushGuideWidget::mapFromImageCoordinates(const QPoint& point)
{

    ImageRegionItem* item = (ImageRegionItem*)this->item();
    return mapFromScene(item->zoomSettings()->mapImageToZoom(point));
}

void ImageBrushGuideWidget::updateSourceCursor(const QPointF& pos, int diameter)
{
   if(this->sourceCursor != nullptr)
    {
        this->scene()->removeItem(this->sourceCursor);
        this->scene()->removeItem(this->sourceCursorCenter);
        delete this->sourceCursor;
        delete this->sourceCursorCenter;
    }
   this->sourceCursor = new QGraphicsEllipseItem(0, 0, diameter, diameter);
   this->sourceCursorCenter = new QGraphicsEllipseItem(0, 0, 2, 2);
   this->sourceCursor->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
   this->sourceCursorCenter->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);

   QPen pen(Qt::DashDotDotLine);
   pen.setWidth(2);
   pen.setColor(Qt::black);
   this->sourceCursor->setPen(pen);
   this->sourceCursor->setBrush(QBrush(Qt::transparent));
   this->sourceCursor->setOpacity(1);
   this->scene()->addItem(this->sourceCursor);

   pen.setStyle(Qt::SolidLine);
   this->sourceCursorCenter->setPen(pen);
   this->sourceCursorCenter->setBrush(QBrush(Qt::black));
   this->sourceCursorCenter->setOpacity(1);
   this->scene()->addItem(this->sourceCursorCenter);

   setSourceCursorPosition(pos);
}

void ImageBrushGuideWidget::setSourceCursorPosition(const QPointF& topLeftPos)
{

    double dx = this->sourceCursor->rect().width()/2.0;
    double dy = this->sourceCursor->rect().width()/2.0;
    QPointF shiftedPos = QPointF(topLeftPos.x()-dx, topLeftPos.y()-dy);

    double dx2 = this->sourceCursorCenter->rect().width()/2.0;
    double dy2 = this->sourceCursorCenter->rect().width()/2.0;
    QPointF shiftedPos2 = QPointF(topLeftPos.x()-dx2, topLeftPos.y()-dy2);

    this->sourceCursor->setPos(shiftedPos);
    this->sourceCursorCenter->setPos(shiftedPos2);

    // check if source is outside scene

    bool sourceCursorOutsideScene =
            topLeftPos.x() < 0 || topLeftPos.x() + dx > scene()->width() ||
            topLeftPos.y()<0 || topLeftPos.y() + dy > scene()->height();
    if(sourceCursorOutsideScene)
    {
        this->sourceCursor->setVisible(false);
        this->sourceCursorCenter->setVisible(false);
    }
    else {
        this->sourceCursor->setVisible(true);
        this->sourceCursorCenter->setVisible(true);
    }


}

bool ImageBrushGuideWidget :: checkPointOutsideScene(QPoint globalPoint)
{
  bool pointOutsideScene;
  QPointF temp = mapToScene(globalPoint);

 if(viewport()->width() > scene()->width())
 {
    pointOutsideScene = temp.x()  < 0 || temp.x()  > scene()->width() ||
                               temp.y()  <0 || temp.y()  > scene()->height();
 }
 else {
     QPoint bottomRight = QPoint(viewport()->width()-1,viewport()->height()-1);
     int right = mapToScene(bottomRight).x() ;
     int bottom = mapToScene(bottomRight).y();

     pointOutsideScene = temp.x()  <= 0 || temp.x()  >= right ||
                                temp.y()  <=0 || temp.y()  >= bottom;
 }

 return pointOutsideScene;
}

} // namespace DigikamEditorHealingCloneToolPlugin
