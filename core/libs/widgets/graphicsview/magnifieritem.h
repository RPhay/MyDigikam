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

#pragma once

// Qt includes

#include <QGraphicsItem>
#include <QPixmap>

namespace Digikam
{

class MagnifierItem : public QGraphicsItem
{
public:

    explicit MagnifierItem(QGraphicsItem* const parent = nullptr);
    ~MagnifierItem()                                                          override = default;

    void setSourcePixmap(const QPixmap& pixmap, const QRectF& rect);
    void setZoomFactor(qreal factor);
    void setMagnifierSize(int magnifierSize);
    int  magnifierSize()                                                const;

protected:

    QRectF boundingRect()                                               const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget)                                               override;

private:

    QPixmap m_sourcePixmap;
    QRectF  m_sourceRect;
    qreal   m_zoomFactor   = 2.0;
    int     m_size         = 150;
};

} // namespace Digikam
