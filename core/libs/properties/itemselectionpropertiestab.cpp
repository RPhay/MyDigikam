/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-04-11
 * Description : A tab to display information about the current selection.
 *
 * SPDX-FileCopyrightText: 2020 by Kartik Ramesh <kartikx2000 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "itemselectionpropertiestab.h"

// Qt includes

#include <QApplication>
#include <QGridLayout>
#include <QStyle>
#include <QTreeWidget>
#include <QHeaderView>
#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include <QFileInfo>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dexpanderbox.h"
#include "itempropertiestab.h"
#include "itempropertiestxtlabel.h"
#include "digikam_globals.h"

namespace Digikam
{

class Q_DECL_HIDDEN ItemSelectionPropertiesTab::Private
{
public:

    enum Section
    {
        SelectionItemProperties = 0,
        AlbumItemProperties
    };

public:

    Private() = default;

public:

    DTextLabelValue*     labelSelectionCount  = nullptr;
    DTextLabelValue*     labelSelectionSize   = nullptr;
    DTextLabelValue*     labelSelectionGroups = nullptr;
    DTextLabelValue*     labelTotalCount      = nullptr;
    DTextLabelValue*     labelTotalSize       = nullptr;
    DTextLabelValue*     labelTotalGroups     = nullptr;

    QTreeWidget*         treeSelectionGroups  = nullptr;
    QTreeWidget*         treeTotalGroups      = nullptr;

    ThumbnailLoadThread* thumbLoadThread      = ThumbnailLoadThread::defaultThread();

    const int iconSize                        = 24;
};

ItemSelectionPropertiesTab::ItemSelectionPropertiesTab(QWidget* const parent)
    : DExpanderBox(parent),
      d           (new Private)
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setLineWidth(style()->pixelMetric(QStyle::PM_DefaultFrameWidth));

    // --------------------------------------------------

    const int spacing                     = layoutSpacing();

    QWidget* const select                 = new QWidget(this);
    QGridLayout* const grid1              = new QGridLayout(select);

    DTextLabelName* const selectionCount  = new DTextLabelName(i18n("Count: "),  select);
    DTextLabelName* const selectionSize   = new DTextLabelName(i18n("Size: "),   select);
    DTextLabelName* const selectionGroups = new DTextLabelName(i18n("Groups: "), select);
    d->labelSelectionCount                = new DTextLabelValue(QString(), select);
    d->labelSelectionSize                 = new DTextLabelValue(QString(), select);
    d->labelSelectionGroups               = new DTextLabelValue(QString(), select);

    d->treeSelectionGroups                = new QTreeWidget(select);
    d->treeSelectionGroups->setSortingEnabled(true);
    d->treeSelectionGroups->setRootIsDecorated(true);
    d->treeSelectionGroups->setSelectionMode(QAbstractItemView::SingleSelection);
    d->treeSelectionGroups->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->treeSelectionGroups->setAllColumnsShowFocus(true);
    d->treeSelectionGroups->header()->hide();

    grid1->addWidget(selectionCount,                  0, 0, 1, 1);
    grid1->addWidget(d->labelSelectionCount,          0, 1, 1, 1);
    grid1->addWidget(selectionSize,                   1, 0, 1, 1);
    grid1->addWidget(d->labelSelectionSize,           1, 1, 1, 1);
    grid1->addWidget(selectionGroups,                 2, 0, 1, 1);
    grid1->addWidget(d->labelSelectionGroups,         2, 1, 1, 1);
    grid1->addWidget(d->treeSelectionGroups,          3, 0, 1, 2);
    grid1->setContentsMargins(spacing, spacing, spacing, spacing);
    grid1->setColumnStretch(0, 10);
    grid1->setColumnStretch(1, 25);
    grid1->setSpacing(0);

    insertItem(ItemSelectionPropertiesTab::Private::SelectionItemProperties, select,
               QIcon::fromTheme(QLatin1String("dialog-information")),
               i18n("Selected Item Properties"), QLatin1String("Selection Properties"), true);

    QWidget* const total                  = new QWidget(this);
    QGridLayout* const grid2              = new QGridLayout(total);

    DTextLabelName* const totalCount      = new DTextLabelName(i18n("Count: "),  total);
    DTextLabelName* const totalSize       = new DTextLabelName(i18n("Size: "),   total);
    DTextLabelName* const totalGroups     = new DTextLabelName(i18n("Groups: "), total);

    d->labelTotalCount                    = new DTextLabelValue(QString(), total);
    d->labelTotalSize                     = new DTextLabelValue(QString(), total);
    d->labelTotalGroups                   = new DTextLabelValue(QString(), total);

    d->treeTotalGroups                    = new QTreeWidget(total);
    d->treeTotalGroups->setSortingEnabled(true);
    d->treeTotalGroups->setRootIsDecorated(true);
    d->treeTotalGroups->setSelectionMode(QAbstractItemView::SingleSelection);
    d->treeTotalGroups->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->treeTotalGroups->setAllColumnsShowFocus(true);
    d->treeTotalGroups->header()->hide();

    grid2->addWidget(totalCount,                      5, 0, 1, 1);
    grid2->addWidget(d->labelTotalCount,              5, 1, 1, 1);
    grid2->addWidget(totalSize,                       6, 0, 1, 1);
    grid2->addWidget(d->labelTotalSize,               6, 1, 1, 1);
    grid2->addWidget(totalGroups,                     7, 0, 1, 1);
    grid2->addWidget(d->labelTotalGroups,             7, 1, 1, 1);
    grid2->addWidget(d->treeTotalGroups,              8, 0, 1, 2);
    grid2->setContentsMargins(spacing, spacing, spacing, spacing);
    grid2->setColumnStretch(0, 10);
    grid2->setColumnStretch(1, 25);
    grid2->setSpacing(0);

    insertItem(ItemSelectionPropertiesTab::Private::AlbumItemProperties, total,
               QIcon::fromTheme(QLatin1String("dialog-information")),
               i18n("Album Item Properties"), QLatin1String("Album Item Properties"), true);

    // --------------------------------------------------

    addStretch();

    connect(d->thumbLoadThread, SIGNAL(signalThumbnailLoaded(LoadingDescription,QPixmap)),
            this, SLOT(slotThumbnail(LoadingDescription,QPixmap)));
}

ItemSelectionPropertiesTab::~ItemSelectionPropertiesTab()
{
    delete d;
}

void ItemSelectionPropertiesTab::setCurrentURL(const QUrl& url)
{
    if (url.isEmpty())
    {
         d->labelSelectionCount->setAdjustedText(QString());
         d->labelSelectionSize->setAdjustedText(QString());
         d->labelTotalCount->setAdjustedText(QString());
         d->labelTotalSize->setAdjustedText(QString());
         setEnabled(false);
         return;
    }

    setEnabled(true);
}

void ItemSelectionPropertiesTab::setSelectionCount(const QString& str)
{
    d->labelSelectionCount->setAdjustedText(str);
}

void ItemSelectionPropertiesTab::setSelectionSize(const QString& str)
{
    d->labelSelectionSize->setAdjustedText(str);
}

void ItemSelectionPropertiesTab::setSelectionGroups(const ItemInfoList& infos)
{
    d->labelSelectionGroups->setAdjustedText(QString::number(infos.count()));
    d->treeSelectionGroups->clear();
    QList<ThumbnailIdentifier> thumbs;

    for (const ItemInfo& pinf : std::as_const(infos))
    {
        QTreeWidgetItem* const p = new QTreeWidgetItem(d->treeSelectionGroups, QStringList() << pinf.name());
        d->treeSelectionGroups->addTopLevelItem(p);
        thumbs.append(ThumbnailIdentifier(pinf.fileUrl().toLocalFile()));
        const auto list          = pinf.groupedImages();

        for (const ItemInfo& cinf : list)
        {
            new QTreeWidgetItem(p, QStringList() << cinf.name());
            thumbs.append(ThumbnailIdentifier(cinf.fileUrl().toLocalFile()));
        }
    }
qDebug() << "Items to get Thumbnails from selection:" << thumbs.size();

    d->thumbLoadThread->findGroup(thumbs, d->iconSize);
}

void ItemSelectionPropertiesTab::setTotalCount(const QString& str)
{
    d->labelTotalCount->setAdjustedText(str);
}

void ItemSelectionPropertiesTab::setTotalSize(const QString& str)
{
    d->labelTotalSize->setAdjustedText(str);
}

void ItemSelectionPropertiesTab::setTotalGroups(const ItemInfoList& infos)
{
    d->labelTotalGroups->setAdjustedText(QString::number(infos.count()));
    d->treeTotalGroups->clear();
    QList<ThumbnailIdentifier> thumbs;

    for (const ItemInfo& pinf : std::as_const(infos))
    {
        QTreeWidgetItem* const p = new QTreeWidgetItem(d->treeTotalGroups, QStringList() << pinf.name());
        d->treeTotalGroups->addTopLevelItem(p);
        thumbs.append(ThumbnailIdentifier(pinf.fileUrl().toLocalFile()));
        const auto list          = pinf.groupedImages();

        for (const ItemInfo& cinf : list)
        {
            new QTreeWidgetItem(p, QStringList() << cinf.name());
            thumbs.append(ThumbnailIdentifier(cinf.fileUrl().toLocalFile()));
        }
    }
qDebug() << "Items to get Thumbnails from album:" << thumbs.size();

    d->thumbLoadThread->findGroup(thumbs, d->iconSize);
}

void ItemSelectionPropertiesTab::slotThumbnail(const LoadingDescription& desc, const QPixmap& pix)
{
    QString file = QFileInfo(desc.filePath).fileName();
qDebug() << "File to find:" << file;


    auto items = d->treeSelectionGroups->findItems(file, Qt::MatchExactly);

    if (!items.isEmpty())
    {
qDebug() << "Found in Selection:" << items.first();
        setThumbnail(items.first(), pix);
    }

    items = d->treeTotalGroups->findItems(file, Qt::MatchExactly);

    if (!items.isEmpty())
    {
qDebug() << "Found in album:" << items.first();
        setThumbnail(items.first(), pix);
    }
}

void ItemSelectionPropertiesTab::setThumbnail(QTreeWidgetItem* const item, const QPixmap& pix)
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

} // namespace Digikam

#include "moc_itemselectionpropertiestab.cpp"
