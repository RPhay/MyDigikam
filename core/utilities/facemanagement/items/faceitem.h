/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-17
 * Description : Face tag region item
 *
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DIGIKAM_FACE_ITEM_H
#define DIGIKAM_FACE_ITEM_H

// Qt includes

#include <QObject>
#include <QGraphicsWidget>

// Local includes

#include "facetagsiface.h"
#include "assignnamewidget.h"
#include "regionframeitem.h"

namespace Digikam
{

class HidingStateChanger;

class Q_DECL_HIDDEN FaceItem : public RegionFrameItem       // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

public:

    explicit FaceItem(QGraphicsItem* const parent);
    ~FaceItem() override;

    void setFace(const FaceTagsIface& face);
    FaceTagsIface face()                                const;
    void setHudWidget(AssignNameWidget* const widget);
    AssignNameWidget* widget()                          const;
    void switchMode(AssignNameWidget::Mode mode);
    void setEditable(bool allowEdit);
    void updateCurrentTag();

protected:

    FaceTagsIface       m_face;
    AssignNameWidget*   m_widget;
    HidingStateChanger* m_changer;

private:

    // Disable
    FaceItem();
    FaceItem(const FaceItem&);
    FaceItem& operator=(const FaceItem&);
};

} // namespace Digikam

#endif // DIGIKAM_FACE_ITEM_H
