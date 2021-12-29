/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-09-27
 * Description : Tool tip for Showfoto stack view item.
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "showfotostackviewtooltip.h"

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "showfotosettings.h"
#include "showfotostackviewlist.h"
#include "showfototooltipfiller.h"
#include "showfotostackviewitem.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoStackViewToolTip::Private
{
public:

    explicit Private()
      : view        (nullptr)
    {
    }

    ShowfotoStackViewList* view;
    QModelIndex            index;
};

ShowfotoStackViewToolTip::ShowfotoStackViewToolTip(ShowfotoStackViewList* const view)
    : DItemToolTip(),
      d           (new Private)
{
    d->view = view;
}

ShowfotoStackViewToolTip::~ShowfotoStackViewToolTip()
{
    delete d;
}

void ShowfotoStackViewToolTip::setIndex(const QModelIndex& index)
{
    d->index = index;

    if (!d->index.isValid() || !ShowfotoSettings::instance()->getShowToolTip())
    {
        hide();
    }
    else
    {
        updateToolTip();
        reposition();

        if (isHidden() && !toolTipIsEmpty())
        {
            show();
        }
    }
}

QRect ShowfotoStackViewToolTip::repositionRect()
{
    if (!d->index.isValid())
    {
        return QRect();
    }

    QRect rect = d->view->visualRect(d->index);
    rect.moveTopLeft(d->view->viewport()->mapToGlobal(rect.topLeft()));

    return rect;
}

QString ShowfotoStackViewToolTip::tipContents()
{
    if (!d->index.isValid())
    {
        return QString();
    }

    ShowfotoItemInfo iteminfo = d->view->infoFromIndex(d->index);

    if (iteminfo.isNull())
    {
        return QString();
    }

    return ShowfotoToolTipFiller::ShowfotoItemInfoTipContents(iteminfo);
}

} // namespace ShowFoto
