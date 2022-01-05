/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-15
 * Description : contextmenu helper class
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

ContextMenuHelper::ContextMenuHelper(QMenu* const parent)
    : QObject(parent),
      d      (new Private(this))
{
    d->parent              = parent;
    d->stdActionCollection = DigikamApp::instance()->actionCollection();
}

ContextMenuHelper::~ContextMenuHelper()
{
    delete d;
}

void ContextMenuHelper::addAction(const QString& name, bool addDisabled)
{
    QAction* const action = d->stdActionCollection->action(name);
    addAction(action, addDisabled);
}

void ContextMenuHelper::addAction(QAction* action, bool addDisabled)
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

void ContextMenuHelper::addSubMenu(QMenu* subMenu)
{
    d->parent->addMenu(subMenu);
}

void ContextMenuHelper::addSeparator()
{
    d->parent->addSeparator();
}

void ContextMenuHelper::addAction(QAction* action, QObject* recv, const char* slot, bool addDisabled)
{
    if (!action)
    {
        return;
    }

    connect(action, SIGNAL(triggered()), recv, slot);
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

void ContextMenuHelper::addStandardActionThumbnail(const imageIds& ids, Album* album)
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

void ContextMenuHelper::addOpenAndNavigateActions(const imageIds& ids, bool lightTable)
{
    if (lightTable)
    {
        setSelectedIds(ids);
        QAction* const openImageFile = new QAction(QIcon::fromTheme(QLatin1String("quickopen-file")),
                                                   i18nc("@action: file open dialog", "Open..."), this);
        addAction(openImageFile);

        connect(openImageFile, SIGNAL(triggered()),
                this, SLOT(slotOpenImageFile()));
    }
    else
    {
        addAction(QLatin1String("image_edit"));
        addAction(QLatin1String("move_selection_to_album"));
        addAction(QLatin1String("copy_selection_to"));
    }

    addServicesMenu(ItemInfoList(ids).toImageUrlList());

    // addServicesMenu() has stored d->selectedItems

    if (!d->selectedItems.isEmpty())
    {
        QAction* const openFileMngr = new QAction(QIcon::fromTheme(QLatin1String("folder-open")),
                                                  i18nc("@action: context menu", "Open in File Manager"), this);
        addAction(openFileMngr);

        connect(openFileMngr, SIGNAL(triggered()),
                this, SLOT(slotOpenInFileManager()));
    }

    if (!lightTable)
    {
        addGotoMenu(ids);
    }
}

void ContextMenuHelper::slotOpenImageFile()
{
    if (d->selectedIds.isEmpty())
    {
        return;
    }

    ItemInfoList infos = ItemInfoList(d->selectedIds);
    ItemViewUtilities(d->parent).openInfos(infos.first(), infos, nullptr);
}

bool ContextMenuHelper::imageIdsHaveSameCategory(const imageIds& ids, DatabaseItem::Category category)
{
    bool sameCategory = true;
    QVariantList varList;

    foreach (const qlonglong& id, ids)
    {
        varList = CoreDbAccess().db()->getImagesFields(id, DatabaseFields::Category);

        if (varList.isEmpty() ||
            ((DatabaseItem::Category)varList.first().toInt() != category))
        {
            sameCategory = false;
            break;
        }
    }

    return sameCategory;
}

void ContextMenuHelper::addImportMenu()
{
    QMenu* const menuImport       = new QMenu(i18nc("@action: context menu", "Import"), d->parent);
    KXMLGUIClient* const client   = const_cast<KXMLGUIClient*>(d->stdActionCollection->parentGUIClient());
    QList<DPluginAction*> actions = DPluginLoader::instance()->pluginsActions(DPluginAction::GenericImport,
                                    dynamic_cast<KXmlGuiWindow*>(client));

    if (!actions.isEmpty())
    {
        foreach (DPluginAction* const ac, actions)
        {
            menuImport->addActions(QList<QAction*>() << ac);
        }
    }
    else
    {
        QAction* const notools = new QAction(i18nc("@action: context menu", "No import tool available"), this);
        notools->setEnabled(false);
        menuImport->addAction(notools);
    }

    d->parent->addMenu(menuImport);
}

void ContextMenuHelper::addExportMenu()
{
    QMenu* const menuExport       = new QMenu(i18nc("@action: context menu", "Export"), d->parent);
    KXMLGUIClient* const client   = const_cast<KXMLGUIClient*>(d->stdActionCollection->parentGUIClient());
    QList<DPluginAction*> actions = DPluginLoader::instance()->pluginsActions(DPluginAction::GenericExport,
                                    dynamic_cast<KXmlGuiWindow*>(client));

#if 0

    QAction* selectAllAction      = nullptr;
    selectAllAction               = d->stdActionCollection->action("selectAll");

#endif

    if (!actions.isEmpty())
    {
        foreach (DPluginAction* const ac, actions)
        {
            menuExport->addActions(QList<QAction*>() << ac);
        }
    }
    else
    {
        QAction* const notools = new QAction(i18nc("@action: context menu", "No export tool available"), this);
        notools->setEnabled(false);
        menuExport->addAction(notools);
    }

    d->parent->addMenu(menuExport);
}

void ContextMenuHelper::addGotoMenu(const imageIds& ids)
{
    if (d->gotoAlbumAction && d->gotoDateAction)
    {
        return;
    }

    setSelectedIds(ids);

    // the currently selected image is always the first item

    ItemInfo item;

    if (!d->selectedIds.isEmpty())
    {
        item = ItemInfo(d->selectedIds.first());
    }

    if (item.isNull())
    {
        return;
    }

    // when more then one item is selected, don't add the menu

    if (d->selectedIds.count() > 1)
    {
        return;
    }

    d->gotoAlbumAction    = new QAction(QIcon::fromTheme(QLatin1String("folder-pictures")), i18nc("@action: context menu", "Album"), this);
    d->gotoDateAction     = new QAction(QIcon::fromTheme(QLatin1String("view-calendar")),   i18nc("@action: context menu", "Date"),  this);
    QMenu* const gotoMenu = new QMenu(d->parent);
    gotoMenu->addAction(d->gotoAlbumAction);
    gotoMenu->addAction(d->gotoDateAction);

    TagsPopupMenu* const gotoTagsPopup = new TagsPopupMenu(d->selectedIds, TagsPopupMenu::DISPLAY, gotoMenu);
    QAction* const gotoTag             = gotoMenu->addMenu(gotoTagsPopup);
    gotoTag->setIcon(QIcon::fromTheme(QLatin1String("tag")));
    gotoTag->setText(i18nc("@action: context menu", "Tag"));

    // Disable the goto Tag popup menu, if there are no tags at all.

    if (!CoreDbAccess().db()->hasTags(d->selectedIds))
    {
        gotoTag->setEnabled(false);
    }

    /**
     * TODO:tags to be ported to multiple selection
     */

    QList<Album*> albumList = AlbumManager::instance()->currentAlbums();
    Album* currentAlbum     = nullptr;

    if (!albumList.isEmpty())
    {
        currentAlbum = albumList.first();
    }
    else
    {
        return;
    }

    if      (currentAlbum->type() == Album::PHYSICAL)
    {
        // If the currently selected album is the same as album to
        // which the image belongs, then disable the "Go To" Album.
        // (Note that in recursive album view these can be different).

        if (item.albumId() == currentAlbum->id())
        {
            d->gotoAlbumAction->setEnabled(false);
        }
    }
    else if (currentAlbum->type() == Album::DATE)
    {
        d->gotoDateAction->setEnabled(false);
    }

    QAction* const gotoMenuAction = gotoMenu->menuAction();
    gotoMenuAction->setIcon(QIcon::fromTheme(QLatin1String("go-jump")));
    gotoMenuAction->setText(i18nc("@action: context menu", "Go To"));

    connect(gotoTagsPopup, SIGNAL(signalTagActivated(int)),
            this, SIGNAL(signalGotoTag(int)));

    addAction(gotoMenuAction);
}

void ContextMenuHelper::addQueueManagerMenu()
{
    QMenu* const bqmMenu = new QMenu(i18nc("@action: context menu", "Batch Queue Manager"), d->parent);
    bqmMenu->menuAction()->setIcon(QIcon::fromTheme(QLatin1String("run-build")));
    bqmMenu->addAction(d->stdActionCollection->action(QLatin1String("image_add_to_current_queue")));
    bqmMenu->addAction(d->stdActionCollection->action(QLatin1String("image_add_to_new_queue")));

    // if queue list is empty, do not display the queue submenu

    if (QueueMgrWindow::queueManagerWindowCreated() &&
        !QueueMgrWindow::queueManagerWindow()->queuesMap().isEmpty())
    {
        QueueMgrWindow* const qmw = QueueMgrWindow::queueManagerWindow();
        QMenu* const queueMenu    = new QMenu(i18nc("@action: context menu", "Add to Existing Queue"), bqmMenu);

        // queueActions is used by the exec() method to emit an appropriate signal.
        // Reset the map before filling in the actions.

        if (!d->queueActions.isEmpty())
        {
            d->queueActions.clear();
        }

        QList<QAction*> queueList;

        // get queue list from BQM window, do not access it directly, it might crash
        // when the list is changed

        QMap<int, QString> qmwMap = qmw->queuesMap();

        for (QMap<int, QString>::const_iterator it = qmwMap.constBegin() ;
             it != qmwMap.constEnd() ; ++it)
        {
            QAction* const action     = new QAction(it.value(), this);
            queueList << action;
            d->queueActions[it.key()] = action;
        }

        queueMenu->addActions(queueList);
        bqmMenu->addMenu(queueMenu);
    }

    d->parent->addMenu(bqmMenu);

    // NOTE: see bug #252130 : we need to disable new items to add on BQM is this one is running.

    bqmMenu->setDisabled(QueueMgrWindow::queueManagerWindow()->isBusy());
}

void ContextMenuHelper::setItemFilterModel(ItemFilterModel* model)
{
    d->imageFilterModel = model;
}

void ContextMenuHelper::addStandardActionCut(QObject* recv, const char* slot)
{
    QAction* const cut = DXmlGuiWindow::buildStdAction(StdCutAction, recv, slot, d->parent);
    addAction(cut);
}

void ContextMenuHelper::addStandardActionCopy(QObject* recv, const char* slot)
{
    QAction* const copy = DXmlGuiWindow::buildStdAction(StdCopyAction, recv, slot, d->parent);
    addAction(copy);
}

void ContextMenuHelper::addStandardActionPaste(QObject* recv, const char* slot)
{
    QAction* const paste        = DXmlGuiWindow::buildStdAction(StdPasteAction, recv, slot, d->parent);
    const QMimeData* const data = qApp->clipboard()->mimeData(QClipboard::Clipboard);

    if (!data || !data->hasUrls())
    {
        paste->setEnabled(false);
    }

    addAction(paste, true);
}

void ContextMenuHelper::addStandardActionItemDelete(QObject* recv, const char* slot, int quantity)
{
    QAction* const trashAction = new QAction(QIcon::fromTheme(QLatin1String("user-trash")),
                                             i18ncp("@action:inmenu Pluralized",
                                                    "Move to Trash", "Move %1 Files to Trash",
                                                    quantity), d->parent);
    connect(trashAction, SIGNAL(triggered()),
            recv, slot);

    addAction(trashAction);
}

void ContextMenuHelper::addIQSAction(QObject* recv, const char* slot)
{
    QAction* const IQSAction = new QAction(QIcon::fromTheme(QLatin1String("")),
                                           i18ncp("@action:inmenu Pluralized",
                                                    "Image Quality Sort", "Image Quality Sort",1), d->parent);
    connect(IQSAction, SIGNAL(triggered()),
            recv, slot);

    addAction(IQSAction);
}

QAction* ContextMenuHelper::exec(const QPoint& pos, QAction* at)
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

void ContextMenuHelper::setSelectedIds(const imageIds& ids)
{
    if (d->selectedIds.isEmpty())
    {
        d->selectedIds = ids;
    }
}

void ContextMenuHelper::setSelectedItems(const QList<QUrl>& urls)
{
    if (d->selectedItems.isEmpty())
    {
        d->selectedItems = urls;
    }
}

} // namespace Digikam
