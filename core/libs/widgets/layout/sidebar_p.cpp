/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-03-22
 * Description : a widget to manage sidebar in GUI.
 *
 * Copyright (C) 2005-2006 by Joern Ahrens <joern dot ahrens at kdemail dot net>
 * Copyright (C) 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2001-2003 by Joseph Wenninger <jowenn at kde dot org>
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

#include "sidebar_p.h"

namespace Digikam
{

DMultiTabBarFrame::Private::Private()
    : mainLayout(nullptr),
      position  (Qt::LeftEdge),
      style     (DMultiTabBar::AllIconsText)
{
}

// -------------------------------------------------------------------------------------

DMultiTabBarTab::Private::Private()
    : position(Qt::LeftEdge),
      style   (DMultiTabBar::AllIconsText)
{
}

// -------------------------------------------------------------------------------------

DMultiTabBar::Private::Private()
    : internal (nullptr),
      layout   (nullptr),
      btnTabSep(nullptr),
      position (Qt::LeftEdge)
{
}

// -------------------------------------------------------------------------------------
    
SidebarState::SidebarState()
    : activeWidget(nullptr),
      size        (0)
{
}

SidebarState::SidebarState(QWidget* const w, int size)
    : activeWidget(w),
      size        (size)
{
}

// -------------------------------------------------------------------------------------

Sidebar::Private::Private()
    : minimizedDefault      (false),
      minimized             (false),
      isMinimized           (false),
      tabs                  (0),
      activeTab             (-1),
      dragSwitchId          (-1),
      restoreSize           (0),
      stack                 (nullptr),
      splitter              (nullptr),
      dragSwitchTimer       (nullptr),
      appendedTabsStateCache(),
      optionActiveTabEntry  (QLatin1String("ActiveTab")),
      optionMinimizedEntry  (QLatin1String("Minimized")),
      optionRestoreSizeEntry(QLatin1String("RestoreSize"))
{
}

} // namespace Digikam
