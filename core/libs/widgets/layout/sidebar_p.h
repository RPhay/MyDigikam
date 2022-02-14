/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-03-22
 * Description : a widget to manage sidebar in GUI - Private headers.
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

#ifndef DIGIKAM_SIDE_BAR_P_H
#define DIGIKAM_SIDE_BAR_P_H

#include "sidebar.h"

// Qt includes

#include <QDragEnterEvent>
#include <QEvent>
#include <QPixmap>
#include <QSplitter>
#include <QStackedWidget>
#include <QTimer>
#include <QHash>
#include <QScrollArea>
#include <QFrame>
#include <QActionEvent>
#include <QLayout>
#include <QPainter>
#include <QFontMetrics>
#include <QStyle>
#include <QStyleOptionButton>

// KDE includes

#include <kconfiggroup.h>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN DMultiTabBarFrame::Private
{
public:

    explicit Private();

    QBoxLayout*             mainLayout;
    QList<DMultiTabBarTab*> tabs;
    Qt::Edge                position;
    DMultiTabBar::TextStyle style;
};

// -------------------------------------------------------------------------------------

class Q_DECL_HIDDEN DMultiTabBarTab::Private
{
public:

    explicit Private();

    Qt::Edge                position;
    DMultiTabBar::TextStyle style;
};

// -------------------------------------------------------------------------------------

class Q_DECL_HIDDEN DMultiTabBar::Private
{
public:

    explicit Private();

    DMultiTabBarFrame*         internal;
    QBoxLayout*                layout;
    QFrame*                    btnTabSep;
    QList<DMultiTabBarButton*> buttons;
    Qt::Edge                   position;
};

// -------------------------------------------------------------------------------------

class Q_DECL_HIDDEN SidebarState
{
public:

    SidebarState();
    SidebarState(QWidget* const w, int size);

    QWidget* activeWidget;
    int      size;
};

// -------------------------------------------------------------------------------------

class Q_DECL_HIDDEN Sidebar::Private
{

public:

    explicit Private();

    bool                          minimizedDefault;
    bool                          minimized;

    /**
     * Backup of shrinked status before backup(), restored by restore()
     * NOTE: when sidebar is hidden, only icon bar is affected. If sidebar view is
     * visible, this one must be shrink and restored accordingly.
     */
    bool                          isMinimized;

    int                           tabs;
    int                           activeTab;
    int                           dragSwitchId;
    int                           restoreSize;

    QStackedWidget*               stack;
    SidebarSplitter*              splitter;
    QTimer*                       dragSwitchTimer;

    QHash<QWidget*, SidebarState> appendedTabsStateCache;

    const QString                 optionActiveTabEntry;
    const QString                 optionMinimizedEntry;
    const QString                 optionRestoreSizeEntry;
};

// -------------------------------------------------------------------------------------

class Q_DECL_HIDDEN SidebarSplitter::Private
{
public:

    QList<Sidebar*> sidebars;
};

} // namespace Digikam

#endif // DIGIKAM_SIDE_BAR_P_H
