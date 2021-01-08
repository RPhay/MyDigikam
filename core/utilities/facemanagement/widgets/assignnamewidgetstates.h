/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-17
 * Description : Visibility states controller for assignment and confirmation of names for faces
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

#ifndef DIGIKAM_ASSIGN_NAME_WIDGET_STATES_H
#define DIGIKAM_ASSIGN_NAME_WIDGET_STATES_H

// Local includes

#include "itemvisibilitycontroller.h"

namespace Digikam
{

class FaceItem;

class AssignNameWidgetStates : public HidingStateChanger
{
    Q_OBJECT

public:

    explicit AssignNameWidgetStates(FaceItem* const item);
    ~AssignNameWidgetStates() override;

protected Q_SLOTS:

    void slotStateChanged();
};

} // namespace Digikam

#endif // DIGIKAM_ASSIGN_NAME_WIDGET_STATES_H
