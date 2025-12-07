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
#include <QPainter>
#include <QFileInfo>

// Local includes

#include "digikam_globals.h"
#include "applicationsettings.h"
#include "itemsgroupedviewitem.h"

namespace Digikam
{

class Q_DECL_HIDDEN ItemsGroupedView::Private
{
public:

    Private() = default;

public:

    QTreeWidget*         treeSelectionGroups  = nullptr;
    QTreeWidget*         treeTotalGroups      = nullptr;
    ThumbnailLoadThread* thumbLoadThread      = nullptr;
    int                  iconSize             = 0;
    bool                 showCount            = false;
};

ItemsGroupedView::ItemsGroupedView(QWidget* const parent)
    : QTreeWidget(parent),
      d          (new Private)
{
    setSortingEnabled(true);
    setRootIsDecorated(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAllColumnsShowFocus(true);
    header()->hide();

    d->thumbLoadThread = new ThumbnailLoadThread();
    d->thumbLoadThread->setThumbnailSize(d->iconSize);
    d->thumbLoadThread->setSendSurrogatePixmap(false);

    connect(d->thumbLoadThread, SIGNAL(signalThumbnailLoaded(LoadingDescription,QPixmap)),
            this, SLOT(slotGotThumbnail(LoadingDescription,QPixmap)),
            Qt::QueuedConnection);

    connect(ApplicationSettings::instance(), SIGNAL(setupChanged()),
            this, SLOT(slotSettingsChanged()));

    slotSettingsChanged();
}

ItemsGroupedView::~ItemsGroupedView()
{
    delete d->thumbLoadThread;
    delete d;
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
            static_cast<ItemsGroupedViewItem*>(*it)->setThumb(thumb);
            break;
        }

        ++it;
    }
}

void ItemsGroupedView::slotSettingsChanged()
{
    if (
        d->iconSize  != ApplicationSettings::instance()->getTreeViewIconSize() &&
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
            static_cast<ItemsGroupedViewItem*>(*it)->updateTitle();
            ++it;
        }
    }
}

} // namespace Digikam

#include "moc_itemsgroupedview.cpp"
