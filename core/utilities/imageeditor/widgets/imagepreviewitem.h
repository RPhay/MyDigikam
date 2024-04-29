/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-04-27
 * Description : image preview item for image editor.
 *
 * SPDX-FileCopyrightText: 2011-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_IMAGE_PREVIEW_ITEM_H
#define DIGIKAM_IMAGE_PREVIEW_ITEM_H

// Local includes

#include "graphicsdimgitem.h"
#include "digikam_export.h"

class QPainter;
class QStyleOptionGraphicsItem;
class QGraphicsSceneContextMenuEvent;

namespace Digikam
{

class DIGIKAM_EXPORT ImagePreviewItem : public GraphicsDImgItem
{
    Q_OBJECT

public:

    ImagePreviewItem();
    ~ImagePreviewItem()         override = default;

    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

private:

    // Disable
    explicit ImagePreviewItem(QObject*) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_IMAGE_PREVIEW_ITEM_H
