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

#include "previewtoolbar.h"
#include "imageregionwidget.h"
#include "imageregionitem.h"

using namespace Digikam;

namespace DigikamEditorHealingCloneToolPlugin
{

class ImageBrushGuideWidget : public ImageRegionWidget
{
    Q_OBJECT
    Q_ENUMS(HealingCloneState)

public:

    enum   HealingCloneState
    {
        SELECT_SOURCE,
        PAINT,
        LASSO_DRAW_BOUNDARY,
        LASSO_CLONE,
        MOVE_IMAGE,
        DO_NOTHING
    };

public:

    /**
     * Using the parent's constructor
     * Should be changed to get rid of the inheritance
     */
    explicit ImageBrushGuideWidget(QWidget* const parent = nullptr);
    ~ImageBrushGuideWidget();

    void setBrushRadius(int value);
    void setIsLassoPointsVectorEmpty(bool);
    void setCloneVectorChanged(bool);
    void setSourceCursorPosition(const QPointF& topLeftPos);

    void changeCursorShape(const QColor& color);
    void changeCursorShape(const QPixmap&pixMap, float x = 0.5 , float y = 0.5);
    void updateCursor();

    bool checkPointOutsideScene(const QPoint& point) const;
    void updateSourceCursor(const QPointF& pos = QPoint(), int diamter = 10);

    QPoint mapToImageCoordinates(const QPoint& point);
    QPoint mapFromImageCoordinates(const QPoint& point);

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
    void signalLasso(const QPoint& dst);
    void signalResetLassoPoint();
    void signalContinuePolygon();
    void signalIncreaseBrushRadius();
    void signalDecreaseBrushRadius();
    void signalPushToUndoStack();
    void signalUndoClone();
    void signalRedoClone();

protected:

    void mouseReleaseEvent(QMouseEvent*)override;
    void mousePressEvent(QMouseEvent*)override;
    void mouseMoveEvent(QMouseEvent*)override;
    void mouseDoubleClickEvent(QMouseEvent*) override;
    void keyPressEvent(QKeyEvent*) override;
    void keyReleaseEvent(QKeyEvent*) override;
    void wheelEvent(QWheelEvent*) override;
    void focusOutEvent(QFocusEvent*) override;
    void focusInEvent(QFocusEvent*) override;
    bool event(QEvent*) override;
    void undoSlotSetSourcePoint();
    void activateState(HealingCloneState state);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamEditorHealingCloneToolPlugin

#endif // DIGIKAM_IMAGE_BRUSH_GUIDE_WIDGET_H
