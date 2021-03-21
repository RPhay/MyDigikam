/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-06
 * Description : Albums folder view.
 *
 * Copyright (C) 2005-2006 by Joern Ahrens <joern dot ahrens at kdemail dot net>
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2012 by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright (C) 2009-2011 by Johannes Wienke <languitar at semipol dot de>
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

#include "albumselectiontreeview.h"

// Qt includes

#include <QMessageBox>
#include <QAction>
#include <QEvent>
#include <QIcon>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "tagscache.h"
#include "albummanager.h"
#include "facescansettings.h"
#include "applicationsettings.h"
#include "contextmenuhelper.h"
#include "itemviewtooltip.h"
#include "tooltipfiller.h"
#include "thumbsgenerator.h"
#include "newitemsfinder.h"
#include "facesdetector.h"
#include "coredbaccess.h"
#include "coredb.h"

namespace Digikam
{

class Q_DECL_HIDDEN AlbumViewToolTip: public ItemViewToolTip
{
    Q_OBJECT

public:

    explicit AlbumViewToolTip(AlbumSelectionTreeView* const view)
        : ItemViewToolTip(view)
    {
    }

    AlbumSelectionTreeView* view() const
    {
        return static_cast<AlbumSelectionTreeView*>(ItemViewToolTip::view());
    }

protected:

    QString tipContents() override
    {
        PAlbum* const album = view()->albumForIndex(currentIndex());

        return (ToolTipFiller::albumTipContents(album, view()->albumModel()->albumCount(album)));
    }
};

// ----------------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN AlbumSelectionTreeView::Private
{

public:

    explicit Private()
      : enableToolTips          (false),
        albumModificationHelper (nullptr),
        toolTip                 (nullptr),
        renameAction            (nullptr),
        resetIconAction         (nullptr),
        findDuplAction          (nullptr),
        scanFacesAction         (nullptr),
        repairHiddenAction      (nullptr),
        rebuildThumbsAction     (nullptr),
        contextMenuElement      (nullptr)
    {
    }

    bool                                      enableToolTips;

    AlbumModificationHelper*                  albumModificationHelper;
    AlbumViewToolTip*                         toolTip;

    QAction*                                  renameAction;
    QAction*                                  resetIconAction;
    QAction*                                  findDuplAction;
    QAction*                                  scanFacesAction;
    QAction*                                  repairHiddenAction;
    QAction*                                  rebuildThumbsAction;

    class AlbumSelectionTreeViewContextMenuElement;
    AlbumSelectionTreeViewContextMenuElement* contextMenuElement;
};

// ----------------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN AlbumSelectionTreeView::Private::AlbumSelectionTreeViewContextMenuElement
    : public AbstractAlbumTreeView::ContextMenuElement
{

public:

    explicit AlbumSelectionTreeViewContextMenuElement(AlbumSelectionTreeView::Private* const dd)
        : d(dd)
    {
    }

    void addActions(AbstractAlbumTreeView*, ContextMenuHelper& cmh, Album* a) override
    {
        if (!a || a->isRoot())
        {
            return;
        }

        PAlbum* const album = dynamic_cast<PAlbum*>(a);

        if (!album)
        {
            return;
        }

        if (album->isAlbumRoot())
        {
            cmh.addActionNewAlbum(d->albumModificationHelper, album);
            cmh.addAction(QLatin1String("album_openinfilemanager"));
            cmh.addSeparator();

        // --------------------------------------------------------

            cmh.addAction(d->rebuildThumbsAction);
            d->albumModificationHelper->bindAlbum(d->rebuildThumbsAction, album);
            return;
        }

        // --------------------------------------------------------

        cmh.addActionNewAlbum(d->albumModificationHelper, album);
        cmh.addAction(QLatin1String("album_openinfilemanager"));
        cmh.addSeparator();

        // --------------------------------------------------------

        cmh.addActionRenameAlbum(d->albumModificationHelper, album);
        cmh.addActionResetAlbumIcon(d->albumModificationHelper, album);
        cmh.addSeparator();

        // --------------------------------------------------------

        cmh.addAction(d->findDuplAction);
        d->albumModificationHelper->bindAlbum(d->findDuplAction, album);
        cmh.addAction(d->scanFacesAction);
        d->albumModificationHelper->bindAlbum(d->scanFacesAction, album);
        cmh.addAction(d->rebuildThumbsAction);
        d->albumModificationHelper->bindAlbum(d->rebuildThumbsAction, album);
        cmh.addImportMenu();
        cmh.addExportMenu();
        cmh.addAlbumActions();
        cmh.addSeparator();

        // --------------------------------------------------------

        cmh.addAction(d->repairHiddenAction);
        d->albumModificationHelper->bindAlbum(d->repairHiddenAction, album);
        cmh.addSeparator();

        // --------------------------------------------------------

        cmh.addActionDeleteAlbum(d->albumModificationHelper, album);
        cmh.addSeparator();

        // --------------------------------------------------------

        cmh.addActionEditAlbum(d->albumModificationHelper, album);
    }

public:

    AlbumSelectionTreeView::Private* const d;
};

// ----------------------------------------------------------------------------------------------------

AlbumSelectionTreeView::AlbumSelectionTreeView(QWidget* const parent,
                                               AlbumModel* const model,
                                               AlbumModificationHelper* const albumModificationHelper)
    : AlbumTreeView(parent),
      d(new Private)
{
    setAlbumModel(model);
    d->albumModificationHelper = albumModificationHelper;
    d->toolTip                 = new AlbumViewToolTip(this);
    d->findDuplAction          = new QAction(QIcon::fromTheme(QLatin1String("tools-wizard")),  i18n("Find Duplicates..."),  this);
    d->scanFacesAction         = new QAction(QIcon::fromTheme(QLatin1String("list-add-user")), i18n("Scan for Faces"),      this);
    d->repairHiddenAction      = new QAction(QIcon::fromTheme(QLatin1String("edit-group")),    i18n("Repair hidden Items"), this);
    d->rebuildThumbsAction     = new QAction(QIcon::fromTheme(QLatin1String("view-refresh")),  i18n("Refresh"),             this);

    connect(d->findDuplAction, SIGNAL(triggered()),
            this, SLOT(slotFindDuplicates()));

    connect(d->scanFacesAction, SIGNAL(triggered()),
            this, SLOT(slotScanForFaces()));

    connect(d->repairHiddenAction, SIGNAL(triggered()),
            this, SLOT(slotRepairHiddenItems()));

    connect(d->rebuildThumbsAction, SIGNAL(triggered()),
            this, SLOT(slotRebuildThumbs()));

    setSortingEnabled(true);
    setSelectAlbumOnClick(true);
    setEnableContextMenu(true);
    setContextMenuTitle(i18n("Albums"));

    d->contextMenuElement = new Private::AlbumSelectionTreeViewContextMenuElement(d);
    addContextMenuElement(d->contextMenuElement);
}

AlbumSelectionTreeView::~AlbumSelectionTreeView()
{
    delete d->contextMenuElement;
    delete d;
}

void AlbumSelectionTreeView::setEnableToolTips(bool enable)
{
    d->enableToolTips = enable;
}

void AlbumSelectionTreeView::slotFindDuplicates()
{
    emit signalFindDuplicates(QList<PAlbum*> { d->albumModificationHelper->boundAlbum(sender()) });
}

void AlbumSelectionTreeView::slotScanForFaces()
{
    PAlbum* const album = d->albumModificationHelper->boundAlbum(sender());

    if (!album)
    {
        return;
    }

    AlbumList albums;
    albums << album;

    if (album->childCount())
    {
        if (QMessageBox::question(this, i18n("Scan for Faces"),
                                  i18n("Should sub-albums be scanned too?"))
            == QMessageBox::Yes)
        {
            albums << album->childAlbums(true);
        }
    }

    FaceScanSettings settings;

    settings.accuracy               = ApplicationSettings::instance()->getFaceDetectionAccuracy();
    settings.useYoloV3              = ApplicationSettings::instance()->getFaceDetectionYoloV3();
    settings.task                   = FaceScanSettings::DetectAndRecognize;
    settings.alreadyScannedHandling = FaceScanSettings::Rescan;
    settings.albums                 = albums;

    FacesDetector* const tool = new FacesDetector(settings);
    tool->start();
}

void AlbumSelectionTreeView::slotRepairHiddenItems()
{
    PAlbum* const album = d->albumModificationHelper->boundAlbum(sender());

    if (!album)
    {
        return;
    }

    int needTaggingTag     = TagsCache::instance()->getOrCreateInternalTag(InternalTagName::needTaggingHistoryGraph());
    int originalVersionTag = TagsCache::instance()->getOrCreateInternalTag(InternalTagName::originalVersion());

    foreach (const qlonglong& id, CoreDbAccess().db()->getItemIDsInAlbum(album->id()))
    {
        ItemInfo info(id);

        if (!info.isNull() && info.isGrouped())
        {
            if (info.groupImage().albumId() != album->id())
            {
                info.removeFromGroup();
            }
        }

        if (!info.hasDerivedImages() && info.tagIds().contains(originalVersionTag))
        {
            info.removeTag(originalVersionTag);
            info.setTag(needTaggingTag);
        }
    }
}

void AlbumSelectionTreeView::slotRebuildThumbs()
{
    PAlbum* const album = d->albumModificationHelper->boundAlbum(sender());

    if (!album)
    {
        return;
    }

    ThumbsGenerator* const tool = new ThumbsGenerator(true, album->id());
    tool->start();

    // if physical album, schedule a collection scan of current album's path

    if (album && album->type() == Album::PHYSICAL)
    {
        NewItemsFinder* const tool2 = new NewItemsFinder(NewItemsFinder::ScheduleCollectionScan,
                                                         QStringList() << static_cast<PAlbum*>(album)->folderPath());
        tool2->start();
    }
}

bool AlbumSelectionTreeView::viewportEvent(QEvent* event)
{
    // let the base class handle the event if it is not a tool tip request

    if (event->type() != QEvent::ToolTip)
    {
        return AlbumTreeView::viewportEvent(event);
    }

    // only show tool tips if requested

    if (!d->enableToolTips)
    {
        return false;
    }

    // check that we got a correct event

    QHelpEvent* const helpEvent = dynamic_cast<QHelpEvent*> (event);

    if (!helpEvent)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Unable to determine the correct type of the event. "
                                     << "This should not happen.";
        return false;
    }

    // find the item this tool tip belongs to

    QModelIndex index = indexAt(helpEvent->pos());

    if (!index.isValid())
    {
        return true;
    }

    PAlbum* const album = albumForIndex(index);

    if (!album || album->isRoot() || album->isAlbumRoot())
    {
        // there was no album so we really don't want to show a tooltip.

        return true;
    }

    QRect itemRect = visualRect(index);

    if (!itemRect.contains(helpEvent->pos()))
    {
        return true;
    }

    QStyleOptionViewItem option = viewOptions();
    option.rect                 = itemRect;

    // visualRect can be larger than viewport, intersect with viewport rect

    option.rect                 &= viewport()->rect();
    option.state                |= (index == currentIndex() ? QStyle::State_HasFocus : QStyle::State_None);
    d->toolTip->show(option, index);

    return true;
}

} // namespace Digikam

#include "albumselectiontreeview.moc"
