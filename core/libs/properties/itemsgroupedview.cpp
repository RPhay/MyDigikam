/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-04-11
 * Description : A list view to show grouped items
 *
 * SPDX-FileCopyrightText: 2020-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "itemsgroupedview.h"

// Qt includes

#include <QApplication>
#include <QStyle>
#include <QHeaderView>
#include <QIcon>
#include <QTimer>
#include <QPixmap>
#include <QModelIndex>
#include <QFileInfo>

// Local includes

#include "digikam_globals.h"
#include "coredb.h"
#include "coredbchangesets.h"
#include "coredbwatch.h"
#include "applicationsettings.h"
#include "itemsgroupedviewitem.h"
#include "itemsgroupedtooltip.h"
#include "itemfiltermodel.h"

namespace Digikam
{

class Q_DECL_HIDDEN ItemsGroupedView::Private
{
public:

    Private() = default;

public:

    QTreeWidget*          treeSelectionGroups = nullptr;
    QTreeWidget*          treeTotalGroups     = nullptr;
    ThumbnailLoadThread*  thumbLoadThread     = nullptr;
    int                   iconSize            = 0;
    bool                  showCount           = false;
    bool                  showTips            = false;
    QTimer*               toolTipTimer        = nullptr;
    ItemsGroupedToolTip*  toolTip             = nullptr;
    ItemsGroupedViewItem* toolTipItem         = nullptr;
    ItemFilterModel*      itemModel           = nullptr;
};

ItemsGroupedView::ItemsGroupedView(QWidget* const parent)
    : QTreeWidget(parent),
      d          (new Private)
{
    setSortingEnabled(false);
    setRootIsDecorated(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAllColumnsShowFocus(true);
    viewport()->setMouseTracking(true);
    header()->hide();

    d->thumbLoadThread = new ThumbnailLoadThread();
    d->thumbLoadThread->setThumbnailSize(d->iconSize);
    d->thumbLoadThread->setSendSurrogatePixmap(false);

    d->toolTip         = new ItemsGroupedToolTip(this);
    d->toolTipTimer    = new QTimer(this);

    // ---

    connect(d->thumbLoadThread, SIGNAL(signalThumbnailLoaded(LoadingDescription,QPixmap)),
            this, SLOT(slotGotThumbnail(LoadingDescription,QPixmap)),
            Qt::QueuedConnection);

    connect(ApplicationSettings::instance(), SIGNAL(setupChanged()),
            this, SLOT(slotSettingsChanged()));

    connect(d->toolTipTimer, SIGNAL(timeout()),
            this, SLOT(slotToolTip()));

    connect(CoreDbAccess::databaseWatch(), SIGNAL(collectionImageChange(CollectionImageChangeset)),
            this, SLOT(slotCollectionImageChange(CollectionImageChangeset)),
            Qt::QueuedConnection);

    slotSettingsChanged();
}

ItemsGroupedView::~ItemsGroupedView()
{
    delete d->toolTip;
    delete d->thumbLoadThread;
    delete d;
}

void ItemsGroupedView::setEnableToolTips(bool val)
{
    d->showTips = val;

    if (!val)
    {
        hideToolTip();
    }
}

void ItemsGroupedView::setItemFilterModel(ItemFilterModel* const model)
{
    d->itemModel = model;

    connect(d->itemModel, &ItemFilterModel::signalGroupIsOpen,
            [this](qlonglong group, bool open)
        {
            QTreeWidgetItemIterator it(this);

            while (*it)
            {
                ItemsGroupedViewItem* const item = dynamic_cast<ItemsGroupedViewItem*>(*it);

                if (item && (item->info().id() == group))
                {
                    item->setExpanded(open);
                }

                break;
            }

            ++it;
        }
    );
}

void ItemsGroupedView::hideToolTip()
{
    d->toolTipItem = nullptr;
    d->toolTipTimer->stop();
    slotToolTip();
}

bool ItemsGroupedView::acceptToolTip(const QPoint& pos) const
{
    if (columnAt(pos.x()) == 0)
    {
        return true;
    }

    return false;
}

void ItemsGroupedView::slotToolTip()
{
    d->toolTip->setItem(d->toolTipItem);
}

void ItemsGroupedView::mouseMoveEvent(QMouseEvent* e)
{
    if (e->buttons() == Qt::NoButton)
    {
        ItemsGroupedViewItem* const item = dynamic_cast<ItemsGroupedViewItem*>(itemAt(e->pos()));

        if (d->showTips)
        {
            if (!isActiveWindow())
            {
                hideToolTip();
                return;
            }

            if (item != d->toolTipItem)
            {
                hideToolTip();

                if (acceptToolTip(e->pos()))
                {
                    d->toolTipItem = item;
                    d->toolTipTimer->setSingleShot(true);
                    d->toolTipTimer->start(500);
                }
            }

            if ((item == d->toolTipItem) && !acceptToolTip(e->pos()))
            {
                hideToolTip();
            }
        }

        return;
    }

    hideToolTip();
    QTreeWidget::mouseMoveEvent(e);
}

void ItemsGroupedView::wheelEvent(QWheelEvent* e)
{
    hideToolTip();
    QTreeWidget::wheelEvent(e);
}

void ItemsGroupedView::keyPressEvent(QKeyEvent* e)
{
    hideToolTip();
    QTreeWidget::keyPressEvent(e);
}

void ItemsGroupedView::focusOutEvent(QFocusEvent* e)
{
    hideToolTip();
    QTreeWidget::focusOutEvent(e);
}

void ItemsGroupedView::leaveEvent(QEvent* e)
{
    hideToolTip();
    QTreeWidget::leaveEvent(e);
}

void ItemsGroupedView::setGroups(const ItemInfoList& items)
{
    clear();

    QList<ThumbnailIdentifier> thumbs;

    for (const ItemInfo& pinf : std::as_const(items))
    {
        ItemsGroupedViewItem* const p = new ItemsGroupedViewItem(this, pinf);
        addTopLevelItem(p);
        thumbs.append(ThumbnailIdentifier(pinf.fileUrl().toLocalFile()));

        if (d->itemModel)
        {
            QModelIndex index = d->itemModel->indexForItemInfo(pinf);

            if (index.isValid())
            {
                p->setExpanded(index.data(ItemFilterModel::GroupIsOpenRole).toBool());
            }
        }

        const auto list               = pinf.groupedImages();

        for (const ItemInfo& cinf : std::as_const(list))
        {
            new ItemsGroupedViewItem(p, cinf);
            thumbs.append(ThumbnailIdentifier(cinf.fileUrl().toLocalFile()));
        }
    }

    for (const ThumbnailIdentifier& th : std::as_const(thumbs))
    {
        d->thumbLoadThread->find(th);
    }
}

void ItemsGroupedView::slotGotThumbnail(const LoadingDescription& desc, const QPixmap& pix)
{
    QPixmap thumb = pix;

    if (pix.isNull())
    {
        thumb = QIcon::fromTheme(QLatin1String("view-preview")).pixmap(d->iconSize, QIcon::Disabled);
    }

    QString file = QFileInfo(desc.filePath).fileName();
    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        if ((*it)->text(0).startsWith(file))
        {
            ItemsGroupedViewItem* const item = dynamic_cast<ItemsGroupedViewItem*>(*it);

            if (item)
            {
                item->setThumb(thumb);
            }

            break;
        }

        ++it;
    }
}

void ItemsGroupedView::slotSettingsChanged()
{
    if (
        d->iconSize  != ApplicationSettings::instance()->getTreeViewIconSize() ||
        d->showCount != ApplicationSettings::instance()->getShowFolderTreeViewItemsCount()
       )
    {
        d->iconSize  = ApplicationSettings::instance()->getTreeViewIconSize();
        d->showCount = ApplicationSettings::instance()->getShowFolderTreeViewItemsCount();
        setIconSize(QSize(d->iconSize, d->iconSize));
        d->thumbLoadThread->setThumbnailSize(d->iconSize);

        QTreeWidgetItemIterator it(this);

        while (*it)
        {
            (*it)->setSizeHint(0, QSize(d->iconSize, d->iconSize));

            ItemsGroupedViewItem* const item = dynamic_cast<ItemsGroupedViewItem*>(*it);

            if (item)
            {
                item->updateTitle();
            }

            ++it;
        }
    }

    if (d->showTips != ApplicationSettings::instance()->getShowToolTips())
    {
        setEnableToolTips(ApplicationSettings::instance()->getShowToolTips());
    }
}

void ItemsGroupedView::slotCollectionImageChange(const CollectionImageChangeset& changeset)
{
    switch (changeset.operation())
    {
        case CollectionImageChangeset::Deleted:
        case CollectionImageChangeset::Removed:
        case CollectionImageChangeset::RemovedAll:
        {
            const auto ids = changeset.ids();

            for (const qlonglong& id : ids)
            {
                removeItemById(id);
            }

            break;
        }

        default:
        {
            break;
        }
    }
}

void ItemsGroupedView::removeItemById(qlonglong id)
{
    hideToolTip();

    bool find;

    do
    {
        find = false;
        QTreeWidgetItemIterator it(this);

        while (*it)
        {
            ItemsGroupedViewItem* const item = dynamic_cast<ItemsGroupedViewItem*>(*it);

            if (item && (item->info().id() == id))
            {
                delete item;
                find = true;
                break;
            }

            ++it;
        }
    }
    while (find);
}

} // namespace Digikam

#include "moc_itemsgroupedview.cpp"
