/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Managing of focus point items on a GraphicsDImgView
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
