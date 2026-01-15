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
    d->labelSelectionGroups               = new DTextLabelValue(QString(), d->select);

    d->treeSelectionGroups                = new ItemsGroupedView(d->select);

    grid1->addWidget(selectionCount,                  0, 0, 1, 1);
    grid1->addWidget(d->labelSelectionCount,          0, 1, 1, 1);
    grid1->addWidget(selectionSize,                   1, 0, 1, 1);
    grid1->addWidget(d->labelSelectionSize,           1, 1, 1, 1);
    grid1->addWidget(d->selectionGroups,              2, 0, 1, 1);
    grid1->addWidget(d->labelSelectionGroups,         2, 1, 1, 1);
    grid1->addWidget(d->treeSelectionGroups,          3, 0, 1, 2);
    grid1->setContentsMargins(spacing, spacing, spacing, spacing);
    grid1->setColumnStretch(0, 10);
    grid1->setColumnStretch(1, 25);
    grid1->setRowStretch(3, 10);
    grid1->setSpacing(0);

    // ---

    d->total                              = new QGroupBox(i18n("All Item Properties"), this);
    QGridLayout* const grid2              = new QGridLayout(d->total);

    DTextLabelName* const totalCount      = new DTextLabelName(i18n("Count: "),  d->total);
    DTextLabelName* const totalSize       = new DTextLabelName(i18n("Size: "),   d->total);
    DTextLabelName* const totalGroups     = new DTextLabelName(i18n("Groups: "), d->total);

    d->labelTotalCount                    = new DTextLabelValue(QString(), d->total);
    d->labelTotalSize                     = new DTextLabelValue(QString(), d->total);
    d->labelTotalGroups                   = new DTextLabelValue(QString(), d->total);

    d->treeTotalGroups                    = new ItemsGroupedView(d->total);

    grid2->addWidget(totalCount,                      0, 0, 1, 1);
    grid2->addWidget(d->labelTotalCount,              0, 1, 1, 1);
    grid2->addWidget(totalSize,                       1, 0, 1, 1);
    grid2->addWidget(d->labelTotalSize,               1, 1, 1, 1);
    grid2->addWidget(totalGroups,                     2, 0, 1, 1);
    grid2->addWidget(d->labelTotalGroups,             2, 1, 1, 1);
    grid2->addWidget(d->treeTotalGroups,              3, 0, 1, 2);
    grid2->setContentsMargins(spacing, spacing, spacing, spacing);
    grid2->setColumnStretch(0, 10);
    grid2->setColumnStretch(1, 25);
    grid2->setRowStretch(3, 10);
    grid2->setSpacing(0);
}

ItemSelectionPropertiesTab::~ItemSelectionPropertiesTab()
{
    delete d;
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

void ItemSelectionPropertiesTab::setTotalCount(int count)
{
    d->labelTotalCount->setAdjustedText(QLocale().toString(count));
}

void ItemSelectionPropertiesTab::setTotalSize(const QString& str)
{
    d->labelTotalSize->setAdjustedText(str);
}

void ItemSelectionPropertiesTab::setGroups(const ItemInfoList& totalGroup, const ItemInfoList& selectedGroup)
{
    d->labelSelectionGroups->setAdjustedText(QString::number(selectedGroup.count()));
    d->treeSelectionGroups->setGroups(selectedGroup);

    d->labelTotalGroups->setAdjustedText(QString::number(totalGroup.count()));
    d->treeTotalGroups->setGroups(totalGroup);
}

void ItemSelectionPropertiesTab::setItemFilterModel(ItemFilterModel* const model)
{
    d->treeSelectionGroups->setItemFilterModel(model);
    d->treeTotalGroups->setItemFilterModel(model);
}

} // namespace Digikam

#include "moc_itemselectionpropertiestab.cpp"
