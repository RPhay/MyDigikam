/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2025-12-22
 * Description : a magnifier tool for the GraphicsDImgView canvas.
 *
 * SPDX-FileCopyrightText: 2025-2026 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "magnifieritem.h"

// Qt includes

#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace Digikam
{

class Q_DECL_HIDDEN MagnifierItem::Private
{
public:

    Private() = default;

public:

    QPixmap sourcePixmap;
    QRectF  sourceRect;
    qreal   zoomFactor   = 2.0;
    int     size         = 150;
};

MagnifierItem::MagnifierItem(QGraphicsItem* const parent)
    : QGraphicsItem(parent),
      d            (new Private)

{
    setZValue(10); // Be sure that the loop is always behind the image
}

MagnifierItem::~MagnifierItem()
{
    delete d;
}

void MagnifierItem::setSourcePixmap(const QPixmap& pixmap, const QRectF& rect)
{
    d->sourcePixmap = pixmap;
    d->sourceRect   = rect;
    update();
}

void MagnifierItem::setZoomFactor(qreal factor)
{
    d->zoomFactor = factor;
    update();
}

void MagnifierItem::setMagnifierSize(int magnifierSize)
{
    d->size = magnifierSize;
    update();
}

int MagnifierItem::magnifierSize() const
{
    return d->size;
}

QRectF MagnifierItem::boundingRect() const
{
    return QRectF(-d->size / 2, -d->size / 2, d->size, d->size);
}

void MagnifierItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (d->sourcePixmap.isNull())
    {
        return;
    }

    // Create a ellipsis path for the clipping.

    QPainterPath clipPath;
    clipPath.addEllipse(boundingRect());

    // Apply clipping.

    painter->setClipPath(clipPath);
    painter->setRenderHint(QPainter::Antialiasing, true);

    // Draw the ellipsis.

    QRadialGradient gradient(0, 0, d->size / 2);
    gradient.setColorAt(0, QColor(255, 255, 255, 220)); // blur center
    gradient.setColorAt(1, QColor(255, 255, 255, 100)); // surround transparency
    painter->setBrush(gradient);
    painter->drawEllipse(boundingRect());

    QPixmap zoomed = d->sourcePixmap.copy(d->sourceRect.toRect())
                                   .scaled(d->size, d->size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    painter->drawPixmap(QRectF(-d->size / 2, -d->size / 2, d->size, d->size),
                        zoomed,
                        zoomed.rect());

    // Reset clipping.

    painter->setClipping(false);
}

} // namespace Digikam
