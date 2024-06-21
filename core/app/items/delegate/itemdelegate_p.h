/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-04-19
 * Description : Qt model-view for items - the delegate
 *
 * SPDX-FileCopyrightText: 2002-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2002-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QRect>
#include <QCache>

// Local includes

#include "itemviewdelegate_p.h"

namespace Digikam
{

class ItemCategoryDrawer;

class Q_DECL_HIDDEN ItemDelegate::ItemDelegatePrivate : public ItemViewDelegatePrivate
{
public:

    ItemDelegatePrivate()
    {
        contentWidth        = 0;
        drawFocusFrame      = true;
        drawCoordinates     = false;
        drawImageFormat     = false;
        drawImageFormatTop  = false;
        drawMouseOverFrame  = true;
        ratingOverThumbnail = false;
        categoryDrawer      = nullptr;
        currentView         = nullptr;
        currentModel        = nullptr;

        actualPixmapRectCache.setMaxCost(250);
    }

    int                   contentWidth;

    QRect                 dateRect;
    QRect                 modDateRect;
    QRect                 pixmapRect;
    QRect                 specialInfoRect;
    QRect                 nameRect;
    QRect                 titleRect;
    QRect                 commentsRect;
    QRect                 resolutionRect;
    QRect                 arRect;
    QRect                 sizeRect;
    QRect                 tagRect;
    QRect                 imageInformationRect;
    QRect                 coordinatesRect;
    QRect                 pickLabelRect;
    QRect                 groupRect;

    bool                  drawFocusFrame;
    bool                  drawCoordinates;
    bool                  drawImageFormat;
    bool                  drawImageFormatTop;
    bool                  drawMouseOverFrame;
    bool                  ratingOverThumbnail;

    QCache<int, QRect>    actualPixmapRectCache;
    ItemCategoryDrawer*   categoryDrawer;

    ItemCategorizedView*  currentView;
    QAbstractItemModel*   currentModel;

public:

    void clearRects() override;
};

} // namespace Digikam
