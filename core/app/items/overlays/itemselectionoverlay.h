/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-04-30
 * Description : selection icon view item at mouse hover
 *
 * Copyright (C) 2009-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DIGIKAM_ITEM_SELECTION_OVERLAY_H
#define DIGIKAM_ITEM_SELECTION_OVERLAY_H

// Qt includes

#include <QAbstractItemView>

// Local includes

#include "itemviewhoverbutton.h"
#include "itemdelegateoverlay.h"

namespace Digikam
{

class ItemSelectionOverlayButton : public ItemViewHoverButton
{
    Q_OBJECT

public:

    explicit ItemSelectionOverlayButton(QAbstractItemView* const parentView);
    QSize sizeHint() const  override;

protected:

    QIcon icon()            override;
    void updateToolTip()    override;
};

// --------------------------------------------------------------------

class ItemSelectionOverlay : public HoverButtonDelegateOverlay
{
    Q_OBJECT

public:

    explicit ItemSelectionOverlay(QObject* const parent);
    void setActive(bool active)                 override;

protected:

    ItemViewHoverButton* createButton()         override;
    void updateButton(const QModelIndex& index) override;

protected Q_SLOTS:

    void slotClicked(bool checked);
    void slotSelectionChanged(const QItemSelection&, const QItemSelection&);
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_SELECTION_OVERLAY_H
