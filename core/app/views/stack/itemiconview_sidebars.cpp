/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2002-16-10
 * Description : Item icon view interface - Sidebar methods.
 *
 * Copyright (C) 2002-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2002-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2011 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2010-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright (C) 2011-2013 by Michael G. Hansen <mike at mghansen dot de>
 * Copyright (C) 2014-2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * Copyright (C) 2017      by Simon Frei <freisim93 at gmail dot com>
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

#include "itemiconview_p.h"

namespace Digikam
{

QList<SidebarWidget*> ItemIconView::leftSidebarWidgets() const
{
    return d->leftSideBarWidgets;
}

void ItemIconView::showSideBars()
{
    d->leftSideBar->restore();
    d->rightSideBar->restore();
}

void ItemIconView::hideSideBars()
{
    d->leftSideBar->backup();
    d->rightSideBar->backup();
}

void ItemIconView::toggleLeftSidebar()
{
    d->leftSideBar->isExpanded() ? d->leftSideBar->shrink()
                                 : d->leftSideBar->expand();
}

void ItemIconView::toggleRightSidebar()
{
    d->rightSideBar->isExpanded() ? d->rightSideBar->shrink()
                                  : d->rightSideBar->expand();
}

void ItemIconView::previousLeftSideBarTab()
{
    d->leftSideBar->activePreviousTab();
}

void ItemIconView::nextLeftSideBarTab()
{
    d->leftSideBar->activeNextTab();
}

void ItemIconView::previousRightSideBarTab()
{
    d->rightSideBar->activePreviousTab();
}

void ItemIconView::nextRightSideBarTab()
{
    d->rightSideBar->activeNextTab();
}

void ItemIconView::slotNotificationError(const QString& message, int type)
{
    d->errorWidget->setMessageType((DNotificationWidget::MessageType)type);
    d->errorWidget->setText(message);
    d->errorWidget->animatedShowTemporized(15000);   // Notification will be closed automatically in 15s
}

void ItemIconView::slotLeftSideBarActivateAlbums()
{
    d->leftSideBar->setActiveTab(d->albumFolderSideBar);
}

void ItemIconView::slotLeftSideBarActivateTags()
{
    d->leftSideBar->setActiveTab(d->tagViewSideBar);
}

void ItemIconView::slotLeftSideBarActivate(SidebarWidget* widget)
{
    d->leftSideBar->setActiveTab(widget);
}

void ItemIconView::slotLeftSideBarActivate(QWidget* widget)
{
    slotLeftSideBarActivate(static_cast<SidebarWidget*>(widget));
}

void ItemIconView::slotRightSideBarActivateTitles()
{
    d->rightSideBar->setActiveTab(d->rightSideBar->imageDescEditTab());
    d->rightSideBar->imageDescEditTab()->setFocusToTitlesEdit();
}

void ItemIconView::slotRightSideBarActivateComments()
{
    d->rightSideBar->setActiveTab(d->rightSideBar->imageDescEditTab());
    d->rightSideBar->imageDescEditTab()->setFocusToCommentsEdit();
}

void ItemIconView::slotRightSideBarActivateAssignedTags()
{
    d->rightSideBar->setActiveTab(d->rightSideBar->imageDescEditTab());
    d->rightSideBar->imageDescEditTab()->activateAssignedTagsButton();
}

void ItemIconView::slotLeftSidebarChangedTab(QWidget* w)
{
    // TODO update, temporary cast

    SidebarWidget* const widget = dynamic_cast<SidebarWidget*>(w);

    foreach (SidebarWidget* const sideBarWidget, d->leftSideBarWidgets)
    {
        bool active = (widget && (widget == sideBarWidget));
        sideBarWidget->setActive(active);
    }
}

void ItemIconView::slotSidebarTabTitleStyleChanged()
{
    d->leftSideBar->setStyle(ApplicationSettings::instance()->getSidebarTitleStyle());
    d->rightSideBar->setStyle(ApplicationSettings::instance()->getSidebarTitleStyle());

    /// @todo Which settings actually have to be reloaded?
    //d->rightSideBar->applySettings();
}

void ItemIconView::slotPopupFiltersView()
{
    d->rightSideBar->setActiveTab(d->filterWidget);
    d->filterWidget->setFocusToTextFilter();
}

} // namespace Digikam
