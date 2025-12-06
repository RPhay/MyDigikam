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

namespace Digikam
{

class Q_DECL_HIDDEN ItemsGroupedView::Private
{
public:

    Private() = default;

public:

    QTreeWidget*               treeSelectionGroups  = nullptr;
    QTreeWidget*               treeTotalGroups      = nullptr;
    ThumbnailLoadThread*       thumbLoadThread      = ThumbnailLoadThread::defaultThread();
    int                        iconSize             = ApplicationSettings::instance()->getTreeViewIconSize();
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

    connect(d->thumbLoadThread, SIGNAL(signalThumbnailLoaded(LoadingDescription,QPixmap)),
            this, SLOT(slotGotThumbnail(LoadingDescription,QPixmap)),
            Qt::QueuedConnection);

    connect(ApplicationSettings::instance(), SIGNAL(setupChanged()),
            this, SLOT(slotSettingsChanged()));
}

ItemsGroupedView::~ItemsGroupedView()
{
    delete d;
}

void ItemsGroupedView::setGroups(const ItemInfoList& items)
{
    clear();

    QList<ThumbnailIdentifier> thumbs;
    bool count = ApplicationSettings::instance()->getShowFolderTreeViewItemsCount();

    for (const ItemInfo& pinf : std::as_const(items))
    {
        QTreeWidgetItem* const p = new QTreeWidgetItem(this, QStringList() << (count ? QString::fromLatin1("%1 (%2)")
                                                                                       .arg(pinf.name())
                                                                                       .arg(pinf.numberOfGroupedImages() + 1)
                                                                                     : pinf.name()));
        addTopLevelItem(p);
        thumbs.append(ThumbnailIdentifier(pinf.fileUrl().toLocalFile()));
        const auto list          = pinf.groupedImages();

        for (const ItemInfo& cinf : std::as_const(list))
        {
            new QTreeWidgetItem(p, QStringList() << cinf.name());
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

    if (thumb.isNull())
    {
        thumb = QIcon::fromTheme(QLatin1String("view-preview")).pixmap(d->iconSize, d->iconSize, QIcon::Disabled);
    }

    QString file = QFileInfo(desc.filePath).fileName();
    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        if ((*it)->text(0) == file)
        {
            setThumbnail(*it, thumb);
            break;
        }

        ++it;
    }
}

void ItemsGroupedView::setThumbnail(QTreeWidgetItem* const item, const QPixmap& pix)
{
    QPixmap pixmap(d->iconSize + 2, d->iconSize + 2);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    p.drawPixmap((pixmap.width()  / 2) - (pix.width()  / 2),
                 (pixmap.height() / 2) - (pix.height() / 2), pix);
    QIcon icon = QIcon(pixmap);

    // We make sure the preview icon stays the same regardless of the role.

    icon.addPixmap(pix, QIcon::Selected, QIcon::On);
    icon.addPixmap(pix, QIcon::Selected, QIcon::Off);
    icon.addPixmap(pix, QIcon::Active,   QIcon::On);
    icon.addPixmap(pix, QIcon::Active,   QIcon::Off);
    icon.addPixmap(pix, QIcon::Normal,   QIcon::On);
    icon.addPixmap(pix, QIcon::Normal,   QIcon::Off);
    item->setIcon(0, icon);
}

void ItemsGroupedView::slotSettingsChanged()
{
    if (d->iconSize != ApplicationSettings::instance()->getTreeViewIconSize())
    {
        setIconSize(ApplicationSettings::instance()->getTreeViewIconSize());
    }
}

void ItemsGroupedView::setIconSize(int size)
{
    d->iconSize = size;
    d->thumbLoadThread->setThumbnailSize(d->iconSize);

    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        (*it)->setSizeHint(0, QSize(d->iconSize, d->iconSize));
        ++it;
    }
}

} // namespace Digikam

#include "moc_itemsgroupedview.cpp"
