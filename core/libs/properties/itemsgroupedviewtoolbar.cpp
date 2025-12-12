/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-04-11
 * Description : A list view to show grouped items
 *
 * SPDX-FileCopyrightText: 2008-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "itemsgroupedviewtoolbar.h"

// Qt includes

#include <QIcon>
#include <QToolButton>
#include <QHBoxLayout>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "itemsgroupedview.h"

namespace Digikam
{

ItemsGroupedViewToolbar::ItemsGroupedViewToolbar(QWidget* const parent)
    : QWidget(parent)
{
    auto* const layout       = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);

    auto* const deleteButton = new QToolButton(this);
    deleteButton->setIcon(QIcon::fromTheme(QLatin1String("edit-delete")));
    deleteButton->setToolTip(i18n("Remove from Group"));

    connect(deleteButton, &QToolButton::clicked,
            this, &ItemsGroupedViewToolbar::signalDeleteRequested);

    layout->addWidget(deleteButton);
    setLayout(layout);

    setVisible(false);
}

void ItemsGroupedViewToolbar::enterEvent(QEnterEvent* event)
{
    setVisible(true);
    QWidget::enterEvent(event);
}

void ItemsGroupedViewToolbar::leaveEvent(QEvent* event)
{
    setVisible(false);
    QWidget::leaveEvent(event);
}

} // namespace Digikam

#include "moc_itemsgroupedviewtoolbar.cpp"
