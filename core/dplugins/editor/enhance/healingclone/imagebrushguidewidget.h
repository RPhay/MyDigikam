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

#ifndef DIGIKAM_IMAGE_BRUSH_GUIDE_WIDGET_H
#define DIGIKAM_IMAGE_BRUSH_GUIDE_WIDGET_H

// Local includes

#include "imageguidewidget.h"
#include "previewtoolbar.h"
#include <QPainter>
#include<QStandardPaths>
#include<QCursor>
#include<imageregionwidget.h>
#include<imageregionitem.h>

using namespace Digikam;



namespace DigikamEditorHealingCloneToolPlugin
{

class ImageBrushGuideWidget : public ImageRegionWidget
{
    Q_OBJECT
    Q_ENUMS(HealingCloneState)

public:

    enum   HealingCloneState {
        SELECT_SOURCE,
        PAINT,
        LASSO_DRAW_BOUNDARY,
        LASSO_CLONE,
        MOVE_IMAGE
    };



    /**
     * Using the parent's constructor
     * Should be changed to get rid of the inheritance
     */
  //  using ImageGuideWidget::ImageGuideWidget;
    void setDefaults();
    void zoomImage(int zoomPercent);
    void zoomPlus();
    void zoomMinus();
    double getScaleRatio();
    void resetPixelsAndReclone();
    void resetPixels();
    void setBrushRadius(int value);
    void setIsLassoPointsVectorEmpty(bool);
    void setCloneVectorChanged(bool);
    void recenterOnMousePosition();
    void changeCursorShape(QColor color);
    void changeCursorShape(QPixmap,float,float);
    void updateCursor();
    QPoint mapToImageCoordinates(QPoint point);
    explicit ImageBrushGuideWidget(QWidget* const parent = nullptr);

public Q_SLOTS:

    /**
     * @brief slotSrcSet toggles the fixing of the brush source center
     */
    void slotSetSourcePoint();
    void slotMoveImage();
    void slotLassoSelect();

Q_SIGNALS:

    /**
     * @brief signalClone emitted when the src is set and the user initiated a brush click
     * and keeps emitting with motion
     */
    void signalClone(const QPoint& currentSrc, const QPoint& currentDst);
    void signalReclone();
    void signalLasso(const QPoint& dst);
    void signalResetLassoPoint();
    void signalContinuePolygon();
    void signalIncreaseBrushRadius();
    void signalDecreaseBrushRadius();
    void signalZoomPercentChanged(int z);
    void signalPushToUndoStack();
    void signalUndoClone();
    void signalRedoClone();

protected:

    void mouseReleaseEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseDoubleClickEvent(QMouseEvent*) override;
    void keyPressEvent(QKeyEvent *event) ;
    void keyReleaseEvent(QKeyEvent *event) ;
    void resizeEvent(QResizeEvent *) override;
    void wheelEvent(QWheelEvent *event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void focusInEvent(QFocusEvent * event) override;
    bool event(QEvent*) override;
    void undoSlotSetSourcePoint();
    void showEvent( QShowEvent* event ) override;
    void activateState(HealingCloneState state);







private:

    bool   srcSet = true;
    bool isLassoPointsVectorEmpty = true;
    QPoint src;
    QPoint dst;
    QPoint oldPos;
    double default_w;
    double default_h;
    double float_w;
    double float_h;
    bool amIFocused = false;
    bool cloneVectorChanged = true;
    int brushRadius;
    QColor brushColor = QColor(Qt::red);
    HealingCloneState currentState = HealingCloneState::SELECT_SOURCE;
};

} // namespace DigikamEditorHealingCloneToolPlugin

#endif // DIGIKAM_IMAGE_BRUSH_GUIDE_WIDGET_H
