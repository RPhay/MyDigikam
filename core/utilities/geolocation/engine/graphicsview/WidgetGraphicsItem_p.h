// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Bastian Holst <bastianholst@gmx.de>
//

#pragma once

#include "ScreenGraphicsItem_p.h"
#include "WidgetGraphicsItem.h"

class QWidget;

namespace Marble
{

class WidgetGraphicsItemPrivate : public ScreenGraphicsItemPrivate
{
public:

    WidgetGraphicsItemPrivate(WidgetGraphicsItem *widgetGraphicsItem,
                              MarbleGraphicsItem *parent);

    ~WidgetGraphicsItemPrivate() override;

    QWidget *m_widget       = nullptr;
    QWidget *m_marbleWidget = nullptr;
    QWidget *m_activeWidget = nullptr;
};

} // Namespace Marble
