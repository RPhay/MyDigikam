/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Managing of focus point items on a GraphicsDImgView
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_FOCUSPOINT_GROUP_P_H
#define DIGIKAM_FOCUSPOINT_GROUP_P_H

#include "focuspointgroup.h"

// Local includes

#include "digikam_debug.h"
#include "focuspointitem.h"

namespace Digikam
{

enum FocusPointGroupState
{
    NoPoints,
    LoadingPoints,
    PointsLoaded
};

class Q_DECL_HIDDEN FocusPointGroup::Private
{

public:

    explicit Private(FocusPointGroup* const q);

    FocusPointItem* createItem(const FocusPoint& point) const;
    FocusPointItem* addItem(const FocusPoint& point);
    void applyVisible();

public:

    GraphicsDImgView*           view;
    ItemInfo                    info;
    bool                        exifRotate;

    QList<FocusPointItem*>      items;

    ClickDragReleaseItem*       manuallyAddWrapItem;
    FocusPointItem*             manuallyAddedItem;

    FocusPointGroupState        state;
    ItemVisibilityController*   visibilityController;

    FocusPointGroup* const      q;
};

} // namespace Digikam

#endif // DIGIKAM_FOCUSPOINT_GROUP_P_H
