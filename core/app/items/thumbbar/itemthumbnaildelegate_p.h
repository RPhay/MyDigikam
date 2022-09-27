/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-04-19
 * Description : thumbnail bar for items - the delegate
 *
 * SPDX-FileCopyrightText: 2006-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ITEM_THUMBNAIL_DELEGATE_P_H
#define DIGIKAM_ITEM_THUMBNAIL_DELEGATE_P_H

// Qt includes

#include <QRect>
#include <QCache>

// Local includes

#include "itemdelegate_p.h"

namespace Digikam
{

class Q_DECL_HIDDEN ItemThumbnailDelegatePrivate : public ItemDelegate::ItemDelegatePrivate
{
public:

    explicit ItemThumbnailDelegatePrivate()
        : flow(QListView::LeftToRight)
    {
          // switch off drawing of frames
          drawMouseOverFrame  = false;
          drawFocusFrame      = false;

          // switch off composing rating over background
          ratingOverThumbnail = true;
    }

public:

    QListView::Flow flow;
    QRect           viewSize;

public:

    void init(ItemThumbnailDelegate* const q);
};

} // namespace Digikam

#endif // DIGIKAM_ITEMS_THUMBNAIL_DELEGATE_P_H
