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

void GraphicsDImgView::setMagnifierZoomLevel(qreal level)
{
    d->magnifier->setZoomFactor(level);
}

qreal GraphicsDImgView::magnifierZoomLevel() const
{
    return d->magnifier->zoomFactor();
}

void GraphicsDImgView::updateMagnifier()
{
    QPointF position = mapToScene(mapFromGlobal(QCursor::pos()));

    d->magnifier->setPos(position);

    QPointF imagePos = d->item->zoomSettings()->mapZoomToImage(position);

    // Use the zoom factor from the magnifier to adjust the source size area

    qreal magnifierZoomFactor = d->magnifier->zoomFactor();
    qreal imageZoomFactor = d->item->zoomSettings()->zoomFactor();

    // Compute the size of the source area accordingly with the magnifier zoom factor

    int halfSize = (d->magnifier->magnifierSize() / 2) / magnifierZoomFactor;

    QRectF sourceRect(
        imagePos.x() - halfSize,
        imagePos.y() - halfSize,
        d->magnifier->magnifierSize() / magnifierZoomFactor,
        d->magnifier->magnifierSize() / magnifierZoomFactor
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
