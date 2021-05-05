/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-04-30
 * Description : rejection icon view item on mouse hover
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

#ifndef DIGIKAM_FACE_REJECTION_OVERLAY_H
#define DIGIKAM_FACE_REJECTION_OVERLAY_H

// Qt includes

#include <QAbstractItemView>

// Local includes

#include "itemviewhoverbutton.h"
#include "itemdelegateoverlay.h"
#include "iteminfo.h"

namespace Digikam
{

class FaceRejectionOverlayButton : public ItemViewHoverButton
{
    Q_OBJECT

public:

    explicit FaceRejectionOverlayButton(QAbstractItemView* const parentView);
    QSize sizeHint() const  override;

protected:

    QIcon icon()            override;
    void updateToolTip()    override;
};

// --------------------------------------------------------------------

class FaceRejectionOverlay : public HoverButtonDelegateOverlay
{
    Q_OBJECT

public:

    explicit FaceRejectionOverlay(QObject* const parent);
    void setActive(bool active)                     override;

Q_SIGNALS:

    void rejectFaces(const QList<QModelIndex>& indexes);

protected:

    ItemViewHoverButton* createButton()             override;
    void updateButton(const QModelIndex& index)     override;
    bool checkIndex(const QModelIndex& index) const override;
    void widgetEnterEvent()                         override;
    void widgetLeaveEvent()                         override;

protected Q_SLOTS:

    void slotClicked();
};

} // namespace Digikam

#endif // DIGIKAM_FACE_REJECTION_OVERLAY_H
