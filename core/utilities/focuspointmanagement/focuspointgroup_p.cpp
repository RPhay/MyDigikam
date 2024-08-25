/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Managing of focus point items on a GraphicsDImgView
 *
 * SPDX-FileCopyrightText: 2021-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "focuspointgroup_p.h"

namespace Digikam
{

FocusPointGroup::Private::Private(FocusPointGroup* const qq)
    : q(qq)
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

    QSize size                 = info.dimensions();

    if  (size.width() < size.height())
    {
        size.transpose();
    }

    QRect pointRect            = point.getRectBySize(size);

    if (exifRotate)
    {
        TagRegion::adjustToOrientation(pointRect,
                                       info.orientation(),
                                       size);
    }

    item->setOriginalRect(pointRect);
    item->setVisible(false);

    return item;
}

FocusPointItem* FocusPointGroup::Private::addItem(const FocusPoint& point)
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "FocusPointsGroup: create item with point" << point;

    FocusPointItem* const item = createItem(point);

    visibilityController->addItem(item);

    items << item;

    return item;
}

} // namspace Digikam
