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

DMultiTabBarButton::DMultiTabBarButton(const QPixmap& pic, const QString& text,
                                       int id, QWidget* const parent)
    : QPushButton(QIcon(pic), text, parent),
      m_id       (id)
{
    // --- NOTE: use dynamic binding as slotClicked() is a virtual method which can be re-implemented in derived classes.

    connect(this, &QPushButton::clicked,
            this, &DMultiTabBarButton::slotClicked);

    // we can't see the focus, so don't take focus. #45557
    // If keyboard navigation is wanted, then only the bar should take focus,
    // and arrows could change the focused button; but generally, tabbars don't take focus anyway.

    setFocusPolicy(Qt::NoFocus);

    // See RB #128005

    setAttribute(Qt::WA_LayoutUsesWidgetRect);
}

DMultiTabBarButton::~DMultiTabBarButton()
{
}

void DMultiTabBarButton::setText(const QString& text)
{
    QPushButton::setText(text);
}

void DMultiTabBarButton::slotClicked()
{
    updateGeometry();
    Q_EMIT signalClicked(m_id);
}

int DMultiTabBarButton::id() const
{
    return m_id;
}

void DMultiTabBarButton::hideEvent(QHideEvent* e)
{
    QPushButton::hideEvent(e);
    DMultiTabBar* const tb = dynamic_cast<DMultiTabBar*>(parentWidget());

    if (tb)
    {
        tb->updateSeparator();
    }
}

void DMultiTabBarButton::showEvent(QShowEvent* e)
{
    QPushButton::showEvent(e);
    DMultiTabBar* const tb = dynamic_cast<DMultiTabBar*>(parentWidget());

    if (tb)
    {
        tb->updateSeparator();
    }
}

void DMultiTabBarButton::paintEvent(QPaintEvent*)
{
    QStyleOptionButton opt;
    opt.initFrom(this);
    opt.icon     = icon();
    opt.iconSize = iconSize();

    // removes the QStyleOptionButton::HasMenu ButtonFeature

    opt.features = QStyleOptionButton::Flat;
    QPainter painter(this);
    style()->drawControl(QStyle::CE_PushButton, &opt, &painter, this);
}

} // namespace Digikam
