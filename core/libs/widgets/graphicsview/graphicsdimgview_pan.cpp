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

void GraphicsDImgView::installPanIcon()
{
    d->cornerButton = PanIconWidget::button();
    setCornerWidget(d->cornerButton);

    connect(d->cornerButton, SIGNAL(pressed()),
            this, SLOT(slotCornerButtonPressed()));
}

void GraphicsDImgView::slotCornerButtonPressed()
{
    if (d->panIconPopup)
    {
        d->panIconPopup->hide();
        d->panIconPopup->deleteLater();
        d->panIconPopup = nullptr;
    }

    d->panIconPopup          = new PanIconFrame(this);
    PanIconWidget* const pan = new PanIconWidget(d->panIconPopup);
/*
    connect(pan, SIGNAL(signalSelectionTakeFocus()),
            this, SIGNAL(signalContentTakeFocus()));
*/
    connect(pan, SIGNAL(signalSelectionMoved(QRect,bool)),
            this, SLOT(slotPanIconSelectionMoved(QRect,bool)));

    connect(pan, SIGNAL(signalHidden()),
            this, SLOT(slotPanIconHidden()));

    pan->setImage(180, 120, item()->image());
    QRectF sceneRect(mapToScene(viewport()->rect().topLeft()), mapToScene(viewport()->rect().bottomRight()));
    pan->setRegionSelection(item()->zoomSettings()->sourceRect(sceneRect).toRect());
    pan->setMouseFocus();
    d->panIconPopup->setMainWidget(pan);
/*
    slotContentTakeFocus();
*/
    QPoint g = mapToGlobal(viewport()->pos());
    g.setX(g.x()+ viewport()->size().width());
    g.setY(g.y()+ viewport()->size().height());
    d->panIconPopup->popup(QPoint(g.x() - d->panIconPopup->width(),
                                  g.y() - d->panIconPopup->height()));

    pan->setCursorToLocalRegionSelectionCenter();
}

void GraphicsDImgView::slotPanIconHidden()
{
    d->cornerButton->blockSignals(true);
    d->cornerButton->animateClick();
    d->cornerButton->blockSignals(false);
}

void GraphicsDImgView::slotPanIconSelectionMoved(const QRect& imageRect, bool b)
{
    QRectF zoomRect = item()->zoomSettings()->mapImageToZoom(imageRect);
    qCDebug(DIGIKAM_WIDGETS_LOG) << imageRect << zoomRect;
    centerOn(item()->mapToScene(zoomRect.center()));
    viewport()->update();

    if (b)
    {
        d->panIconPopup->hide();
        d->panIconPopup->deleteLater();
        d->panIconPopup = nullptr;
        slotPanIconHidden();
        //slotContentLeaveFocus();
    }
}

void GraphicsDImgView::startPanning(const QPoint& pos)
{
    if (horizontalScrollBar()->maximum() || verticalScrollBar()->maximum())
    {
        d->movingInProgress = true;
        d->mousePressPos    = pos;
        d->panningScrollPos = QPoint(horizontalScrollBar()->value(),
                                     verticalScrollBar()->value());
        viewport()->setCursor(Qt::SizeAllCursor);
    }
}

void GraphicsDImgView::continuePanning(const QPoint& pos)
{
    QPoint delta = pos - d->mousePressPos;
    horizontalScrollBar()->setValue(d->panningScrollPos.x() + (isRightToLeft() ? delta.x() : -delta.x()));
    verticalScrollBar()->setValue(d->panningScrollPos.y() - delta.y());

    Q_EMIT contentsMoved(false);

    viewport()->update();
}

void GraphicsDImgView::finishPanning()
{
    Q_EMIT contentsMoved(true);

    viewport()->unsetCursor();
}

} // namespace Digikam
