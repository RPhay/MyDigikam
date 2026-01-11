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

MagnifierItem::MagnifierItem(QGraphicsItem* const parent)
    : QGraphicsItem(parent)
{
    setZValue(10); // Be sure that the loop is always behind the image
}

void MagnifierItem::setSourcePixmap(const QPixmap& pixmap, const QRectF& rect)
{
    m_sourcePixmap = pixmap;
    m_sourceRect   = rect;
    update();
}

void MagnifierItem::setZoomFactor(qreal factor)
{
    m_zoomFactor = factor;
    update();
}

void MagnifierItem::setMagnifierSize(int magnifierSize)
{
    m_size = magnifierSize;
    update();
}

int MagnifierItem::magnifierSize() const
{
    return m_size;
}

QRectF MagnifierItem::boundingRect() const
{
    return QRectF(-m_size / 2, -m_size / 2, m_size, m_size);
}

void MagnifierItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (m_sourcePixmap.isNull())
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

    QRadialGradient gradient(0, 0, m_size / 2);
    gradient.setColorAt(0, QColor(255, 255, 255, 220)); // blur center
    gradient.setColorAt(1, QColor(255, 255, 255, 100)); // surround transparency
    painter->setBrush(gradient);
    painter->drawEllipse(boundingRect());

    QPixmap zoomed = m_sourcePixmap.copy(m_sourceRect.toRect())
                                   .scaled(m_size, m_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    painter->drawPixmap(QRectF(-m_size / 2, -m_size / 2, m_size, m_size),
                        zoomed,
                        zoomed.rect());

    // Reset clipping.

    painter->setClipping(false);
}

} // namespace Digikam
