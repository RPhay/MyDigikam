/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-17
 * Description : Face tag region item
 *
 * Copyright (C) 2010      by Aditya Bhatt <adityabhatt1991 at gmail dot com>
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

#include "faceitem.h"

// Local includes

#include "digikam_debug.h"
#include "assignnamewidgetstates.h"

namespace Digikam
{

FaceItem::FaceItem(QGraphicsItem* const parent)
    : RegionFrameItem(parent),
      m_widget       (nullptr),
      m_changer      (nullptr)
{
}

FaceItem::~FaceItem()
{
}

void FaceItem::setFace(const FaceTagsIface& face)
{
    m_face = face;
    updateCurrentTag();
    setEditable(!m_face.isConfirmedName());
}

FaceTagsIface FaceItem::face() const
{
    return m_face;
}

void FaceItem::setHudWidget(AssignNameWidget* const widget)
{
    m_widget = widget;
    updateCurrentTag();
    RegionFrameItem::setHudWidget(widget);

    // Ensure that all HUD widgets are stacked before the frame items

    hudWidget()->setZValue(1);
}

AssignNameWidget* FaceItem::widget() const
{
    return m_widget;
}

void FaceItem::switchMode(AssignNameWidget::Mode mode)
{
    if (!m_widget || (m_widget->mode() == mode))
    {
        return;
    }

    if (!m_changer)
    {
        m_changer = new AssignNameWidgetStates(this);
    }

    m_changer->changeValue(mode);
}

void FaceItem::setEditable(bool allowEdit)
{
    changeFlags(ShowResizeHandles | MoveByDrag, allowEdit);
}

void FaceItem::updateCurrentTag()
{
    if (m_widget)
    {
        m_widget->setCurrentFace(m_face);
    }
}

} // namespace Digikam
