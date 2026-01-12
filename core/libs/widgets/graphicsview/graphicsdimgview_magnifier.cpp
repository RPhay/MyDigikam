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

void GraphicsDImgView::setMagnifierVisible(bool b)
{
    d->magnifierEnabled = b;
    d->magnifier->setVisible(d->magnifierEnabled);
}

bool GraphicsDImgView::isMagnifierVisible() const
{
    return d->magnifierEnabled;
}

void GraphicsDImgView::setMagnifierZoomFactor(qreal factor)
{
    d->magnifier->setZoomFactor(factor);
}

qreal GraphicsDImgView::magnifierZoomFactor() const
{
    return d->magnifier->zoomFactor();
}

void GraphicsDImgView::setMagnifierSize(int size)
{
    d->magnifier->setMagnifierSize(size);
}

int GraphicsDImgView::magnifierSize() const
{
    return d->magnifier->magnifierSize();
}
void GraphicsDImgView::updateMagnifier()
{
    QPoint globalPos = QCursor::pos();
    QPoint viewPos = viewport()->mapFromGlobal(globalPos);

    if (viewport()->rect().contains(viewPos))
    {
        QPointF scenePos = mapToScene(viewPos);

        // Check if scenePos is inside the image.

        QRectF imageSceneRect = d->item->sceneBoundingRect();

        if (imageSceneRect.contains(scenePos))
        {
            // Move the magnifier to the bottom right of the cursor.

            QPointF magnifierPos = scenePos + QPointF(d->magnifier->magnifierSize() / 2, d->magnifier->magnifierSize() / 2);
            d->magnifier->setPos(magnifierPos);
            d->magnifier->setVisible(true);

            // Convert this position to the source image coordinates.

            QPointF imagePos          = d->item->zoomSettings()->mapZoomToImage(scenePos);

            // Compute the source area size accordingly to the magnifier zoom factor.
            qreal magnifierZoomFactor = d->magnifier->zoomFactor();
            int halfSize              = (d->magnifier->magnifierSize() / 2) / magnifierZoomFactor;

            QRectF sourceRect(
                imagePos.x() - halfSize,
                imagePos.y() - halfSize,
                d->magnifier->magnifierSize() / magnifierZoomFactor,
                d->magnifier->magnifierSize() / magnifierZoomFactor
            );

            // Clipping to the image borders.

            QRectF imageBounds;
            QSize size = d->item->image().size();
            imageBounds.setSize(size);
            sourceRect = sourceRect.intersected(imageBounds);

            // Check if the rectangle source is valid.

            if (sourceRect.isEmpty())
            {
                d->magnifier->setVisible(false);
            }
            else
            {
                QPixmap currentPixmap = d->item->image().convertToPixmap();
                d->magnifier->setSourcePixmap(currentPixmap, sourceRect);
            }
        }
        else
        {
            d->magnifier->setVisible(false);
        }
    }
    else
    {
        d->magnifier->setVisible(false);
    }
}

} // namespace Digikam
