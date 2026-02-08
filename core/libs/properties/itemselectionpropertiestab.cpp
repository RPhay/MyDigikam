/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-04-11
 * Description : A tab to display information about the current selection.
 *
 * SPDX-FileCopyrightText:      2020 by Kartik Ramesh <kartikx2000 at gmail dot com>
 * SPDX-FileCopyrightText: 2020-2026 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "itemselectionpropertiestab.h"

// Qt includes

#include <QApplication>
#include <QGridLayout>
#include <QGroupBox>
#include <QStyle>
#include <QTreeWidget>
#include <QHeaderView>
#include <QIcon>
#include <QTimer>
#include <QPixmap>
#include <QPainter>
#include <QFileInfo>
#include <QLocale>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dexpanderbox.h"
#include "itempropertiestab.h"
#include "itempropertiestxtlabel.h"
#include "digikam_globals.h"
#include "applicationsettings.h"
#include "itemsgroupedview.h"

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

    DTextLabelValue*  labelSelectionCount  = nullptr;
    DTextLabelValue*  labelSelectionSize   = nullptr;
    DTextLabelValue*  labelSelectionGroups = nullptr;
    DTextLabelValue*  labelTotalCount      = nullptr;
    DTextLabelValue*  labelTotalSize       = nullptr;
    DTextLabelValue*  labelTotalGroups     = nullptr;
    DTextLabelName*   selectionGroups      = nullptr;

    ItemsGroupedView* treeSelectionGroups  = nullptr;
    ItemsGroupedView* treeTotalGroups      = nullptr;

    QTreeWidget*      selectionMimes       = nullptr;
    QTreeWidget*      totalMimes           = nullptr;

    QGroupBox*        select               = nullptr;
    QGroupBox*        total                = nullptr;
};

ItemSelectionPropertiesTab::ItemSelectionPropertiesTab(QWidget* const parent)
    : DVBox(parent),
      d    (new Private)
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setLineWidth(style()->pixelMetric(QStyle::PM_DefaultFrameWidth));
    const int spacing                     = layoutSpacing();

    // --------------------------------------------------

    d->select                             = new QGroupBox(i18n("Selected Item Properties"), this);
    QGridLayout* const grid1              = new QGridLayout(d->select);

    DTextLabelName* const selectionCount  = new DTextLabelName(i18n("Count: "),  d->select);
    DTextLabelName* const selectionSize   = new DTextLabelName(i18n("Size: "),   d->select);
    d->selectionGroups                    = new DTextLabelName(i18n("Groups: "), d->select);
    d->labelSelectionCount                = new DTextLabelValue(QString(), d->select);
    d->labelSelectionSize                 = new DTextLabelValue(QString(), d->select);

    d->selectionMimes                     = new QTreeWidget(d->select);
    d->selectionMimes->setSortingEnabled(false);
    d->selectionMimes->setRootIsDecorated(false);
    d->selectionMimes->setSelectionMode(QAbstractItemView::NoSelection);
    d->selectionMimes->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->selectionMimes->setColumnCount(2);
    d->selectionMimes->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    d->selectionMimes->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    d->selectionMimes->header()->hide();
    d->selectionMimes->headerItem()->setTextAlignment(1, Qt::AlignRight);

    d->labelSelectionGroups               = new DTextLabelValue(QString(), d->select);

    d->treeSelectionGroups                = new ItemsGroupedView(d->select);

    grid1->addWidget(selectionCount,                  0, 0, 1, 1);
    grid1->addWidget(d->labelSelectionCount,          0, 1, 1, 1);
    grid1->addWidget(d->selectionMimes,               1, 0, 1, 2);
    grid1->addWidget(selectionSize,                   2, 0, 1, 1);
    grid1->addWidget(d->labelSelectionSize,           2, 1, 1, 1);
    grid1->addWidget(d->selectionGroups,              3, 0, 1, 1);
    grid1->addWidget(d->labelSelectionGroups,         3, 1, 1, 1);
    grid1->addWidget(d->treeSelectionGroups,          4, 0, 1, 2);
    grid1->setContentsMargins(spacing, spacing, spacing, spacing);
    grid1->setColumnStretch(0, 10);
    grid1->setColumnStretch(1, 25);
    grid1->setRowStretch(4, 10);
    grid1->setSpacing(0);

    // ---

    d->total                              = new QGroupBox(i18n("All Item Properties"), this);
    QGridLayout* const grid2              = new QGridLayout(d->total);

    DTextLabelName* const totalCount      = new DTextLabelName(i18n("Count: "),  d->total);
    DTextLabelName* const totalSize       = new DTextLabelName(i18n("Size: "),   d->total);
    DTextLabelName* const totalGroups     = new DTextLabelName(i18n("Groups: "), d->total);

    d->labelTotalCount                    = new DTextLabelValue(QString(), d->total);
    d->labelTotalSize                     = new DTextLabelValue(QString(), d->total);

    d->totalMimes                         = new QTreeWidget(d->total);
    d->totalMimes->setSortingEnabled(false);
    d->totalMimes->setRootIsDecorated(false);
    d->totalMimes->setSelectionMode(QAbstractItemView::NoSelection);
    d->totalMimes->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->totalMimes->setColumnCount(2);
    d->totalMimes->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    d->totalMimes->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    d->totalMimes->header()->hide();

    d->labelTotalGroups                   = new DTextLabelValue(QString(), d->total);

    d->treeTotalGroups                    = new ItemsGroupedView(d->total);

    grid2->addWidget(totalCount,                      0, 0, 1, 1);
    grid2->addWidget(d->labelTotalCount,              0, 1, 1, 1);
    grid2->addWidget(d->totalMimes,                   1, 0, 1, 2);
    grid2->addWidget(totalSize,                       2, 0, 1, 1);
    grid2->addWidget(d->labelTotalSize,               2, 1, 1, 1);
    grid2->addWidget(totalGroups,                     3, 0, 1, 1);
    grid2->addWidget(d->labelTotalGroups,             3, 1, 1, 1);
    grid2->addWidget(d->treeTotalGroups,              4, 0, 1, 2);
    grid2->setContentsMargins(spacing, spacing, spacing, spacing);
    grid2->setColumnStretch(0, 10);
    grid2->setColumnStretch(1, 25);
    grid2->setRowStretch(4, 10);
    grid2->setSpacing(0);
}

ItemSelectionPropertiesTab::~ItemSelectionPropertiesTab()
{
    delete d;
}

void ItemSelectionPropertiesTab::clear()
{
    d->treeSelectionGroups->clear();
    d->treeTotalGroups->clear();
    d->selectionMimes->clear();
    d->totalMimes->clear();
    setSelectionCount(0);
    setSelectionSize(QString());
    setTotalCount(0);
    setTotalSize(QString());
}

void ItemSelectionPropertiesTab::setSelectionCount(int count)
{
    d->labelSelectionCount->setAdjustedText(QLocale().toString(count));

    d->select->setVisible(count != 0);
    d->total->setVisible(count == 0);
}

void ItemSelectionPropertiesTab::setSelectionSize(const QString& str)
{
    d->labelSelectionSize->setAdjustedText(str);
}

void ItemSelectionPropertiesTab::setSelectionMimes(const QHash<QString, int>& mimes)
{
    d->selectionMimes->clear();

    QHash<QString, int>::const_iterator it;

    for (it = mimes.constBegin() ; it != mimes.constEnd() ; ++it)
    {
        QTreeWidgetItem* const item = new QTreeWidgetItem(d->selectionMimes, QStringList() << it.key() << QString::number(it.value()));
        item->setTextAlignment(1, Qt::AlignRight);
    }
}

void ItemSelectionPropertiesTab::setSelectionGroups(const ItemInfoList& groups)
{
    d->labelSelectionGroups->setAdjustedText(QString::number(groups.count()));
    d->treeSelectionGroups->setGroups(groups);
}

void ItemSelectionPropertiesTab::setTotalCount(int count)
{
    d->labelTotalCount->setAdjustedText(QLocale().toString(count));
}

void ItemSelectionPropertiesTab::setTotalSize(const QString& str)
{
    d->labelTotalSize->setAdjustedText(str);
}

void ItemSelectionPropertiesTab::setTotalMimes(const QHash<QString, int>& mimes)
{
    d->totalMimes->clear();

    QHash<QString, int>::const_iterator it;

    for (it = mimes.constBegin() ; it != mimes.constEnd() ; ++it)
    {
        QTreeWidgetItem* const item = new QTreeWidgetItem(d->totalMimes, QStringList() << it.key() << QString::number(it.value()));
        item->setTextAlignment(1, Qt::AlignRight);
    }
}

void ItemSelectionPropertiesTab::setTotalGroups(const ItemInfoList& groups)
{
    d->labelTotalGroups->setAdjustedText(QString::number(groups.count()));
    d->treeTotalGroups->setGroups(groups);
}

void ItemSelectionPropertiesTab::setItemFilterModel(ItemFilterModel* const model)
{
    d->treeSelectionGroups->setItemFilterModel(model);
    d->treeTotalGroups->setItemFilterModel(model);
}

} // namespace Digikam

#include "moc_itemselectionpropertiestab.cpp"
