/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-07-25
 * Description : image region widget item for image editor.
 *
 * SPDX-FileCopyrightText: 2013-2014 by Yiou Wang <geow812 at gmail dot com>
 * SPDX-FileCopyrightText: 2013-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_IMAGE_REGION_ITEM_H
#define DIGIKAM_IMAGE_REGION_ITEM_H

// Qt includes

#include <QStyleOptionGraphicsItem>

// Local includes

#include "graphicsdimgitem.h"
#include "digikam_export.h"
#include "dimg.h"
#include "imageregionwidget.h"

namespace Digikam
{

class DIGIKAM_EXPORT ImageRegionItem : public GraphicsDImgItem
{
    Q_OBJECT

public:

    explicit ImageRegionItem(ImageRegionWidget* const view, bool paintExtras = true);
    ~ImageRegionItem()                                                                     override;

    void setTargetImage(const DImg& img);
    void setHighLightPoints(const QPolygon& pointsList);
    void setRenderingPreviewMode(int mode);

    void  paintExtraData(QPainter* const painter);
    QRect getImageRegion() const;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent*)                                        override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent*)                                        override;

private:

    class Private;
    Private* const dd = nullptr;
};

} // namespace Digikam

#endif // DIGIKAM_IMAGE_REGION_ITEM_H
