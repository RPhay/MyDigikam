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

        // Vérifier si scenePos est dans les limites de l'image
        QRectF imageSceneRect = d->item->sceneBoundingRect();
        if (imageSceneRect.contains(scenePos))
        {
            // Positionner la loupe en bas à droite du curseur
            QPointF magnifierPos = scenePos + QPointF(d->magnifier->magnifierSize() / 2, d->magnifier->magnifierSize() / 2);
            d->magnifier->setPos(magnifierPos);
            d->magnifier->setVisible(true);

            // Convertir cette position en coordonnées de l'image source
            QPointF imagePos = d->item->zoomSettings()->mapZoomToImage(scenePos);

            // Obtenir le zoomFactor de la loupe
            qreal magnifierZoomFactor = d->magnifier->zoomFactor();

            // Calculer la taille de la zone source en fonction du zoom de la loupe
            int halfSize = (d->magnifier->magnifierSize() / 2) / magnifierZoomFactor;

            QRectF sourceRect(
                imagePos.x() - halfSize,
                imagePos.y() - halfSize,
                d->magnifier->magnifierSize() / magnifierZoomFactor,
                d->magnifier->magnifierSize() / magnifierZoomFactor
            );

            // Clipping aux bords de l'image
            QRectF imageBounds;
            QSize size = d->item->image().size();
            imageBounds.setSize(size);
            sourceRect = sourceRect.intersected(imageBounds);

            // Vérifier si le rectangle source est valide
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
