/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-11-22
 * Description : digiKam light table - Extra tools
 *
 * Copyright (C) 2007-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "lighttablewindow_p.h"

namespace Digikam
{

void LightTableWindow::slotEditItem()
{
    if (!d->thumbView->currentInfo().isNull())
    {
        slotEditItem(d->thumbView->currentInfo());
    }
}

void LightTableWindow::slotEditItem(const ItemInfo& info)
{
    ImageWindow* const im = ImageWindow::imageWindow();
    ItemInfoList list     = d->thumbView->allItemInfos();

    im->loadItemInfos(list, info, i18n("Light Table"));

    if (im->isHidden())
    {
        im->show();
    }
    else
    {
        im->raise();
    }

    im->setFocus();
}

//FIXME
void LightTableWindow::slotLeftSlideShowManualFromCurrent()
{
    QList<DPluginAction*> actions = DPluginLoader::instance()->
                                        pluginActions(QLatin1String("org.kde.digikam.plugin.generic.SlideShow"), this);

    if (actions.isEmpty())
    {
        return;
    }

    // set current image to SlideShow Plugin

    actions[0]->setData(d->previewView->leftItemInfo().fileUrl());

    actions[0]->trigger();

    d->fromLeftPreview = true;
}

void LightTableWindow::slotRightSlideShowManualFromCurrent()
{
    QList<DPluginAction*> actions = DPluginLoader::instance()->
                                       pluginActions(QLatin1String("org.kde.digikam.plugin.generic.SlideShow"), this);

    if (actions.isEmpty())
    {
        return;
    }

    // set current image to SlideShow Plugin

    actions[0]->setData(d->previewView->rightItemInfo().fileUrl());
    actions[0]->trigger();

    d->fromLeftPreview = false;
}

void LightTableWindow::slotSlideShowLastItemUrl()
{
    QList<DPluginAction*> actions = DPluginLoader::instance()->
                                       pluginActions(QLatin1String("org.kde.digikam.plugin.generic.SlideShow"), this);

    if (actions.isEmpty())
    {
        return;
    }

    // get last image to SlideShow Plugin

    QUrl url = actions[0]->data().toUrl();

    if (d->fromLeftPreview && !d->navigateByPairAction->isChecked())
    {
        d->thumbView->blockSignals(true);
        d->thumbView->setCurrentUrl(url);
        d->thumbView->blockSignals(false);
        slotSetItemLeft();
    }
    else
    {
        d->thumbView->setCurrentUrl(url);
    }
}

} // namespace Digikam
