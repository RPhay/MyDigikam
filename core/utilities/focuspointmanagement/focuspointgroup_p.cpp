/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Managing of focus point items on a GraphicsDImgView
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
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

#include "focuspointgroup_p.h"

namespace Digikam
{

FocusPointGroup::Private::Private(FocusPointGroup* const q)
    : view                  (nullptr),
      exifRotate            (true),
      manuallyAddWrapItem   (nullptr),
      manuallyAddedItem     (nullptr),
      state                 (NoPoints),
      visibilityController  (nullptr),
      q                     (q)
{
}

void FocusPointGroup::Private::applyVisible()
{
    if      (state == NoPoints)
    {
        // If not yet loaded, load. load() will transitionToVisible after loading.

        q->load();
    }
    else if (state == PointsLoaded)
    {
        if (view->previewItem()->isLoaded())
        {
            visibilityController->show();
        }
    }
}

FocusPointItem* FocusPointGroup::Private::createItem(const FocusPoint& point) const
{
    FocusPointItem* const item = new FocusPointItem(view->previewItem());
    item->setPoint(point);

    QRect pointRect            = point.getRectBySize(info.dimensions());

    if (!exifRotate)
    {
        TagRegion::reverseToOrientation(pointRect,
                                        info.orientation(),
                                        info.dimensions());
    }

    item->setOriginalRect(pointRect);
    item->setVisible(false);

    return item;
}

FocusPointItem* FocusPointGroup::Private::addItem(const FocusPoint& point)
{
    FocusPointItem* const item = createItem(point);

    visibilityController->addItem(item);

    items << item;

    return item;
}

} // namspace Digikam
