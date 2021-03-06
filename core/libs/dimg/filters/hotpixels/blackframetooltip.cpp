/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-08-20
 * Description : black frame list view tool tip
 *
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "blackframetooltip.h"

// Qt includes

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QString>
#include <QRect>

namespace Digikam
{

class Q_DECL_HIDDEN BlackFrameToolTip::Private
{
public:

    explicit Private()
      : view(nullptr),
        item(nullptr)
    {
    }

    QString          tip;

    QTreeWidget*     view;
    QTreeWidgetItem* item;
};

BlackFrameToolTip::BlackFrameToolTip(QTreeWidget* const view)
    : DItemToolTip(),
      d           (new Private)
{
    d->view = view;
}

BlackFrameToolTip::~BlackFrameToolTip()
{
    delete d;
}

void BlackFrameToolTip::setToolTipString(const QString& tip)
{
    d->tip = tip;
}

void BlackFrameToolTip::setItem(QTreeWidgetItem* const item)
{
    d->item = item;

    if (!d->item)
    {
        hide();
    }
    else
    {
        show();
    }
}

void BlackFrameToolTip::show()
{
    updateToolTip();
    reposition();

    if (isHidden() && !toolTipIsEmpty())
    {
        DItemToolTip::show();
    }
}

QRect BlackFrameToolTip::repositionRect()
{
    if (!d->item)
    {
        return QRect();
    }

    QRect rect = d->view->visualItemRect(d->item);
    rect.moveTopLeft(d->view->viewport()->mapToGlobal(rect.topLeft()));

    return rect;
}

QString BlackFrameToolTip::tipContents()
{
    return d->tip;
}

} // namespace Digikam
