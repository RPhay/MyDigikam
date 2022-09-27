/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-03-22
 * Description : a widget to manage sidebar in GUI.
 *
 * SPDX-FileCopyrightText: 2005-2006 by Joern Ahrens <joern dot ahrens at kdemail dot net>
 * SPDX-FileCopyrightText: 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2008-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2001-2003 by Joseph Wenninger <jowenn at kde dot org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
