/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-11-22
 * Description : stand alone digiKam image editor
 *
 * Copyright (C) 2004-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "showfoto_p.h"

namespace ShowFoto
{

void Showfoto::slotClearThumbBar()
{
    d->infoList.clear();
    d->model->clearShowfotoItemInfos();
}

void Showfoto::slotRemoveItemInfos(const QList<ShowfotoItemInfo>& infos)
{
    foreach (const ShowfotoItemInfo& inf, infos)
    {
        d->infoList.removeAll(inf);
    }

    d->model->removeShowfotoItemInfos(infos);
}

void Showfoto::slotShowfotoItemInfoActivated(const ShowfotoItemInfo& info)
{
    if (!d->thumbBar->currentInfo().isNull() && !promptUserSave(d->currentLoadedUrl))
    {
        d->thumbBar->setCurrentUrl(d->currentLoadedUrl);

        return;
    }

    slotOpenUrl(info);
}

Digikam::ThumbBarDock* Showfoto::thumbBar() const
{
    return d->thumbBarDock;
}

void Showfoto::slotFirst()
{
    if (!d->thumbBar->currentInfo().isNull() && !promptUserSave(d->thumbBar->currentUrl()))
    {
        return;
    }

    d->thumbBar->toFirstIndex();
    d->thumbBar->setCurrentInfo(d->thumbBar->showfotoItemInfos().first());
    slotOpenUrl(d->thumbBar->showfotoItemInfos().first());
}

void Showfoto::slotLast()
{
    if (!d->thumbBar->currentInfo().isNull() && !promptUserSave(d->thumbBar->currentUrl()))
    {
        return;
    }

    d->thumbBar->toLastIndex();
    d->thumbBar->setCurrentInfo(d->thumbBar->showfotoItemInfos().last());
    slotOpenUrl(d->thumbBar->showfotoItemInfos().last());
}

void Showfoto::slotForward()
{
    if (!d->thumbBar->currentInfo().isNull() && !promptUserSave(d->thumbBar->currentUrl()))
    {
        return;
    }

    bool currentIsNull = d->thumbBar->currentInfo().isNull();

    if (!currentIsNull)
    {
         d->thumbBar->toNextIndex();
         slotOpenUrl(d->thumbBar->currentInfo());
    }
}

void Showfoto::slotBackward()
{
    if (!d->thumbBar->currentInfo().isNull() && !promptUserSave(d->thumbBar->currentUrl()))
    {
        return;
    }

    bool currentIsNull = d->thumbBar->currentInfo().isNull();

    if (!currentIsNull)
    {
         d->thumbBar->toPreviousIndex();
         slotOpenUrl(d->thumbBar->currentInfo());
    }
}

} // namespace ShowFoto
