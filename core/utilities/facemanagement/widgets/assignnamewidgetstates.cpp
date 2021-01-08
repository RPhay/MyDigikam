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
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "assignnamewidgetstates.h"

// Local includes

#include "digikam_debug.h"
#include "faceitem.h"
#include "assignnamewidget.h"

namespace Digikam
{

AssignNameWidgetStates::AssignNameWidgetStates(FaceItem* const item)
    : HidingStateChanger(item->widget(), "mode", item)
{
    // The WidgetProxyItem

    addItem(item->hudWidget());

    connect(this, SIGNAL(stateChanged()),
            this, SLOT(slotStateChanged()));
}

AssignNameWidgetStates::~AssignNameWidgetStates()
{
}

void AssignNameWidgetStates::slotStateChanged()
{
    FaceItem* const item = static_cast<FaceItem*>(parent());

    // Show resize handles etc. only in edit modes

    item->setEditable(item->widget()->mode() != AssignNameWidget::ConfirmedMode);
}

} // namespace Digikam
