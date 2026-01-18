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

    enum ZoomFactor
    {
        Zoom15 = 15,    // x1.5
        Zoom20 = 20,    // x2.0
        Zoom25 = 25,    // x2.5
        Zoom30 = 30,    // x3.0
        Zoom35 = 35,    // x3.5
        Zoom40 = 40     // x4.0
    };

public:

    explicit MagnifierItem(QGraphicsItem* const parent = nullptr);
    ~MagnifierItem()                                                          override;

    void setSourcePixmap(const QPixmap& pixmap, const QRectF& rect);

    void setZoomFactor(qreal factor);
    qreal zoomFactor()                                                  const;

    void setMagnifierSize(int magnifierSize);
    int  magnifierSize()                                                const;

protected:

    QRectF boundingRect()                                               const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget)                                               override;

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
