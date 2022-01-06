/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-15
 * Description : contextmenu helper class - Actions methods.
 *
 * Copyright (C) 2009-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright (C) 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "contextmenuhelper_p.h"

namespace Digikam
{

QAction* ContextMenuHelper::exec(const QPoint& pos, QAction* const at)
{
    QAction* const choice = d->parent->exec(pos, at);

    if (choice)
    {
        if (d->selectedIds.count() == 1)
        {
            ItemInfo selectedItem(d->selectedIds.first());

            if      (choice == d->gotoAlbumAction)
            {
                emit signalGotoAlbum(selectedItem);
            }
            else if (choice == d->gotoDateAction)
            {
                emit signalGotoDate(selectedItem);
            }
            else if (choice == d->setThumbnailAction)
            {
                emit signalSetThumbnail(selectedItem);
            }
        }

        // check if a BQM action has been triggered

        for (QMap<int, QAction*>::const_iterator it = d->queueActions.constBegin() ;
             it != d->queueActions.constEnd() ; ++it)
        {
            if (choice == it.value())
            {
                emit signalAddToExistingQueue(it.key());

                return choice;
            }
        }
    }

    return choice;
}

void ContextMenuHelper::addAction(const QString& name, bool addDisabled)
{
    QAction* const action = d->stdActionCollection->action(name);
    addAction(action, addDisabled);
}

void ContextMenuHelper::addAction(QAction* const action, bool addDisabled)
{
    if (!action)
    {
        return;
    }

    if (action->isEnabled() || addDisabled)
    {
        d->parent->addAction(action);
    }
}

void ContextMenuHelper::addSubMenu(QMenu* const subMenu)
{
    d->parent->addMenu(subMenu);
}

void ContextMenuHelper::addSeparator()
{
    d->parent->addSeparator();
}

void ContextMenuHelper::addAction(QAction* const action, QObject* const recv, const char* const slot, bool addDisabled)
{
    if (!action)
    {
        return;
    }

    connect(action, SIGNAL(triggered()),
            recv, slot);

    addAction(action, addDisabled);
}

void ContextMenuHelper::addStandardActionLightTable()
{
    QAction* action = nullptr;
    QStringList ltActionNames;
    ltActionNames << QLatin1String("image_add_to_lighttable")
                  << QLatin1String("image_lighttable");

    if (LightTableWindow::lightTableWindowCreated() &&
        !LightTableWindow::lightTableWindow()->isEmpty())
    {
        action = d->stdActionCollection->action(ltActionNames.at(0));
    }
    else
    {
        action = d->stdActionCollection->action(ltActionNames.at(1));
    }

    addAction(action);
}

void ContextMenuHelper::addStandardActionThumbnail(const imageIds& ids, Album* const album)
{
    if (d->setThumbnailAction)
    {
        return;
    }

    setSelectedIds(ids);

    if (album && (ids.count() == 1))
    {
        if      (album->type() == Album::PHYSICAL)
        {
            d->setThumbnailAction = new QAction(i18nc("@action: context menu", "Set as Album Thumbnail"), this);
        }
        else if (album->type() == Album::TAG)
        {
            d->setThumbnailAction = new QAction(i18nc("@action: context menu", "Set as Tag Thumbnail"), this);
        }

        addAction(d->setThumbnailAction);
        d->parent->addSeparator();
    }
}

void ContextMenuHelper::addStandardActionCut(QObject* const recv, const char* const slot)
{
    QAction* const cut = DXmlGuiWindow::buildStdAction(StdCutAction, recv, slot, d->parent);
    addAction(cut);
}

void ContextMenuHelper::addStandardActionCopy(QObject* const recv, const char* const slot)
{
    QAction* const copy = DXmlGuiWindow::buildStdAction(StdCopyAction, recv, slot, d->parent);
    addAction(copy);
}

void ContextMenuHelper::addStandardActionPaste(QObject* const recv, const char* const slot)
{
    QAction* const paste        = DXmlGuiWindow::buildStdAction(StdPasteAction, recv, slot, d->parent);
    const QMimeData* const data = qApp->clipboard()->mimeData(QClipboard::Clipboard);

    if (!data || !data->hasUrls())
    {
        paste->setEnabled(false);
    }

    addAction(paste, true);
}

void ContextMenuHelper::addStandardActionItemDelete(QObject* const recv, const char* const slot, int quantity)
{
    QAction* const trashAction = new QAction(QIcon::fromTheme(QLatin1String("user-trash")),
                                             i18ncp("@action:inmenu Pluralized",
                                                    "Move to Trash", "Move %1 Files to Trash",
                                                    quantity), d->parent);
    connect(trashAction, SIGNAL(triggered()),
            recv, slot);

    addAction(trashAction);
}

void ContextMenuHelper::addIQSAction(QObject* const recv, const char* const slot)
{
    QAction* const IQSAction = new QAction(QIcon::fromTheme(QLatin1String("")),
                                           i18ncp("@action:inmenu Pluralized",
                                                  "Image Quality Sort", "Image Quality Sort",
                                                  1), d->parent);
    connect(IQSAction, SIGNAL(triggered()),
            recv, slot);

    addAction(IQSAction);
}

} // namespace Digikam
