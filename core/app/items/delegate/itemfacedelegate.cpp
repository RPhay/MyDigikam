/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-04-19
 * Description : Qt model-view for face item - the delegate
 *
 * Copyright (C) 2002-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2002-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2010 by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright (C) 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "itemfacedelegate.h"
#include "itemdelegate_p.h"

// Local includes

#include "digikam_debug.h"
#include "applicationsettings.h"
#include "facetagsiface.h"
#include "itemmodel.h"
#include "tagregion.h"
#include "digikamitemdelegate_p.h"
#include "faceutils.h"

namespace Digikam
{

ItemFaceDelegate::ItemFaceDelegate(ItemCategorizedView* const parent)
    : DigikamItemDelegate(*new ItemFaceDelegatePrivate, parent)
{
}

ItemFaceDelegate::~ItemFaceDelegate()
{
}

QPixmap ItemFaceDelegate::thumbnailPixmap(const QModelIndex& index) const
{
    QRect rect = largerFaceRect(index);

    if (rect.isNull())
    {
        return DigikamItemDelegate::thumbnailPixmap(index);
    }

    // set requested thumbnail detail

    if (rect.isValid())
    {
        const_cast<QAbstractItemModel*>(index.model())->setData(index, rect, ItemModel::ThumbnailRole);
    }

    // parent implementation already resets the thumb size and rect set on model

    QPixmap pix = DigikamItemDelegate::thumbnailPixmap(index);

    if (!pix.isNull() && face(index).isUnconfirmedName())
    {
        QPainter greenBorder(&pix);
        greenBorder.setPen(QPen(Qt::green, 4));
        greenBorder.drawRect(2, 2, pix.width() - 4, pix.height() - 4);
        greenBorder.end();
    }

    return pix;
}

QRect ItemFaceDelegate::faceRect(const QModelIndex& index) const
{
    return face(index).region().toRect();
}

QRect ItemFaceDelegate::largerFaceRect(const QModelIndex& index) const
{
    QRect rect = faceRect(index);

    if (rect.isNull())
    {
        return rect;
    }

    const int margin = FaceUtils::faceRectDisplayMargin(rect);

    return rect.adjusted(-margin, -margin, margin, margin);
}

FaceTagsIface ItemFaceDelegate::face(const QModelIndex& index)
{
    QVariant extraData = index.data(ItemModel::ExtraDataRole);

    if (extraData.isNull())
    {
        return FaceTagsIface();
    }

    return FaceTagsIface::fromVariant(extraData);
}

void ItemFaceDelegate::updateRects()
{
    Q_D(ItemFaceDelegate);
    DigikamItemDelegate::updateRects();
    d->groupRect = QRect();
}

void ItemFaceDelegate::clearModelDataCaches()
{
    DigikamItemDelegate::clearModelDataCaches();
}

} // namespace Digikam
