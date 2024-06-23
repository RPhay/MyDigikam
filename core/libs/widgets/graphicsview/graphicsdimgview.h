/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-04-30
 * Description : Graphics View for DImg preview
 *
 * SPDX-FileCopyrightText: 2010-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2011-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QGraphicsView>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class GraphicsDImgItem;
class DImgPreviewItem;
class SinglePhotoPreviewLayout;

class DIGIKAM_EXPORT GraphicsDImgView : public QGraphicsView
{
    Q_OBJECT

public:

    explicit GraphicsDImgView(QWidget* const parent = nullptr);
    ~GraphicsDImgView()                           override;

    /**
     * Store internal instance of item as GraphicsDImgItem. You can store DImgPreviewItem object also by this method.
     * Use item() or previewItem() to get right version.
     * Note: if you store a GraphicsDImgItem object, previewItem() will return 0.
     */
    void setItem(GraphicsDImgItem* const item);

    /**
     * Return the instance of item set by setItem().
     */
    GraphicsDImgItem*         item()        const;

    /**
     * Return a cast of item instance of item set by setItem() as DImgPreviewItem
     * Note: if you store a GraphicsDImgItem object using setItem(), this method will return 0.
     */
    DImgPreviewItem*          previewItem() const;

    SinglePhotoPreviewLayout* layout()      const;

    /**
     * Scrolls the view such that scenePos (in scene coordinates
     * is displayed on the viewport at viewportPos (in viewport coordinates).
     * E.g., calling scrollPointOnPoint(scenePos, viewport()->rect().center()) is
     * equivalent to calling centerOn(scenePos).
     */
    void scrollPointOnPoint(const QPointF& scenePos, const QPoint& viewportPos);

    // Change from protected to public to be used by ImageRegionWidget and ImageRegionItem
    void drawText(QPainter* p, const QRectF& rect, const QString& text);

    int   contentsX()                       const;
    int   contentsY()                       const;
    QRect visibleArea()                     const;
    void  setContentsPos(int x, int y);
    void  fitToWindow();
    void  toggleFullScreen(bool set);

Q_SIGNALS:

    void contentsMoving(int, int);
    void rightButtonClicked();
    void leftButtonClicked();
    void leftButtonDoubleClicked();
    void activated();
    void toNextImage();
    void toPreviousImage();
    void contentsMoved(bool panningFinished);
    void resized();
/*
    void contentTakeFocus();
*/
    void viewportRectChanged(const QRectF& viewportRect);

protected:

    void drawForeground(QPainter* painter, const QRectF& rect)  override;

    void installPanIcon();

    void mouseDoubleClickEvent(QMouseEvent*)                    override;
    void mousePressEvent(QMouseEvent*)                          override;
    void mouseMoveEvent(QMouseEvent*)                           override;
    void mouseReleaseEvent(QMouseEvent*)                        override;
    void wheelEvent(QWheelEvent*)                               override;
    void resizeEvent(QResizeEvent*)                             override;

    void startPanning(const QPoint& pos);
    void continuePanning(const QPoint& pos);
    void finishPanning();

    void setShowText(bool value);
    void setScaleFitToWindow(bool value);

    virtual bool acceptsMouseClick(QMouseEvent* e);
    void scrollContentsBy(int dx, int dy)                       override;

protected Q_SLOTS:

    void         slotContentsMoved();
    void         slotCornerButtonPressed();
    void         slotPanIconHidden();

    virtual void slotPanIconSelectionMoved(const QRect&, bool);

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
