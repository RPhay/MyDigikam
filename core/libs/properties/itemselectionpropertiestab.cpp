/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-04-11
 * Description : A tab to display information about the current selection.
 *
 * SPDX-FileCopyrightText:      2020 by Kartik Ramesh <kartikx2000 at gmail dot com>
 * SPDX-FileCopyrightText: 2020-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <QTimer>
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

    ItemsGroupedView* treeSelectionGroups  = nullptr;
    ItemsGroupedView* treeTotalGroups      = nullptr;

    QWidget*          select               = nullptr;
    QWidget*          total                = nullptr;
};

ItemSelectionPropertiesTab::ItemSelectionPropertiesTab(QWidget* const parent)
    : DExpanderBox(parent),
      d           (new Private)
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setLineWidth(style()->pixelMetric(QStyle::PM_DefaultFrameWidth));

    // --------------------------------------------------

    const int spacing                     = layoutSpacing();

    d->select                             = new QWidget(this);
    QGridLayout* const grid1              = new QGridLayout(d->select);

    DTextLabelName* const selectionCount  = new DTextLabelName(i18n("Count: "),  d->select);
    DTextLabelName* const selectionSize   = new DTextLabelName(i18n("Size: "),   d->select);
    DTextLabelName* const selectionGroups = new DTextLabelName(i18n("Groups: "), d->select);
    d->labelSelectionCount                = new DTextLabelValue(QString(), d->select);
    d->labelSelectionSize                 = new DTextLabelValue(QString(), d->select);
    d->labelSelectionGroups               = new DTextLabelValue(QString(), d->select);

    d->treeSelectionGroups                = new ItemsGroupedView(d->select);

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

    insertItem(ItemSelectionPropertiesTab::Private::SelectionItemProperties, d->select,
               QIcon::fromTheme(QLatin1String("dialog-information")),
               i18n("Selected Item Properties"), QLatin1String("Selection Properties"), true);

    d->total                              = new QWidget(this);
    QGridLayout* const grid2              = new QGridLayout(d->total);

    DTextLabelName* const totalCount      = new DTextLabelName(i18n("Count: "),  d->total);
    DTextLabelName* const totalSize       = new DTextLabelName(i18n("Size: "),   d->total);
    DTextLabelName* const totalGroups     = new DTextLabelName(i18n("Groups: "), d->total);

    d->labelTotalCount                    = new DTextLabelValue(QString(), d->total);
    d->labelTotalSize                     = new DTextLabelValue(QString(), d->total);
    d->labelTotalGroups                   = new DTextLabelValue(QString(), d->total);

    d->treeTotalGroups                    = new ItemsGroupedView(d->total);

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

    insertItem(ItemSelectionPropertiesTab::Private::AlbumItemProperties, d->total,
               QIcon::fromTheme(QLatin1String("folder")),
               i18n("All Item Properties"), QLatin1String("All Item Properties"), true);

    addStretch();
}

ItemSelectionPropertiesTab::~ItemSelectionPropertiesTab()
{
    delete d;
}

void ItemSelectionPropertiesTab::setCurrentUrl(const QUrl& url)
{
}

void ItemSelectionPropertiesTab::setSelectionCount(const QString& str)
{
    d->labelSelectionCount->setAdjustedText(str);
}

void ItemSelectionPropertiesTab::setSelectionSize(const QString& str)
{
    d->labelSelectionSize->setAdjustedText(str);
}

void ItemSelectionPropertiesTab::setTotalCount(const QString& str)
{
    d->labelTotalCount->setAdjustedText(str);
}

void ItemSelectionPropertiesTab::setTotalSize(const QString& str)
{
    d->labelTotalSize->setAdjustedText(str);
}

void ItemSelectionPropertiesTab::setGroups(const ItemInfoList& selected, const ItemInfoList& total)
{
    d->labelSelectionGroups->setAdjustedText(QString::number(selected.count()));

    d->treeSelectionGroups->setGroups(selected);

    d->labelTotalGroups->setAdjustedText(QString::number(total.count()));
    d->treeTotalGroups->setGroups(total);
}

void ItemSelectionPropertiesTab::setItemFilterModel(ItemFilterModel* const model)
{
    d->treeSelectionGroups->setItemFilterModel(model);
    d->treeTotalGroups->setItemFilterModel(model);
}

} // namespace Digikam

#include "moc_itemselectionpropertiestab.cpp"
