/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-03-22
 * Description : a widget to manage sidebar in GUI - Multi-tab bar frame implementation.
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

DMultiTabBarFrame::DMultiTabBarFrame(QWidget* const parent, Qt::Edge pos)
    : QFrame(parent),
      d     (new Private)
{
    d->position = pos;

    if ((pos == Qt::LeftEdge) || (pos == Qt::RightEdge))
    {
        d->mainLayout = new QVBoxLayout(this);
    }
    else
    {
        d->mainLayout = new QHBoxLayout(this);
    }

    d->mainLayout->setContentsMargins(QMargins());
    d->mainLayout->setSpacing(0);
    d->mainLayout->addStretch();
    setFrameStyle(NoFrame);
    setBackgroundRole(QPalette::Window);
}

DMultiTabBarFrame::~DMultiTabBarFrame()
{
    qDeleteAll(d->tabs);
    d->tabs.clear();
    delete d;
}

void DMultiTabBarFrame::setStyle(DMultiTabBar::TextStyle style)
{
    d->style = style;

    for (int i = 0 ; i < d->tabs.count() ; ++i)
    {
        d->tabs.at(i)->setStyle(d->style);
    }

    updateGeometry();
}

void DMultiTabBarFrame::contentsMousePressEvent(QMouseEvent* e)
{
    e->ignore();
}

void DMultiTabBarFrame::mousePressEvent(QMouseEvent* e)
{
    e->ignore();
}

DMultiTabBarTab* DMultiTabBarFrame::tab(int id) const
{
    QListIterator<DMultiTabBarTab*> it(d->tabs);

    while (it.hasNext())
    {
        DMultiTabBarTab* const tab = it.next();

        if (tab->id() == id)
        {
            return tab;
        }
    }

    return nullptr;
}

void DMultiTabBarFrame::appendTab(const QPixmap& pic, int id, const QString& text)
{
    DMultiTabBarTab* const tab = new DMultiTabBarTab(pic, text, id, this, d->position, d->style);
    d->tabs.append(tab);

    // Insert before the stretch.

    d->mainLayout->insertWidget(d->tabs.size()-1, tab);
    tab->show();
}

void DMultiTabBarFrame::removeTab(int id)
{
    for (int pos = 0 ; pos < d->tabs.count() ; ++pos)
    {
        if (d->tabs.at(pos)->id() == id)
        {
            // remove & delete the tab

            delete d->tabs.takeAt(pos);
            break;
        }
    }
}

void DMultiTabBarFrame::setPosition(Qt::Edge pos)
{
    d->position = pos;

    for (int i = 0 ; i < d->tabs.count() ; ++i)
    {
        d->tabs.at(i)->setPosition(d->position);
    }

    updateGeometry();
}

QList<DMultiTabBarTab*>* DMultiTabBarFrame::tabs()
{
    return &d->tabs;
}

} // namespace Digikam
