/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-04-11
 * Description : A tab to display information about the current selection.
 *
 * Copyright (C) 2020 by Kartik Ramesh <kartikx2000 at gmail dot com>
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

#include "itemselectionpropertiestab.h"

// Qt includes

#include <QApplication>
#include <QGridLayout>
#include <QStyle>
#include <QIcon>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "itempropertiestab.h"
#include "itempropertiestxtlabel.h"

namespace Digikam
{

class Q_DECL_HIDDEN ItemSelectionPropertiesTab::Private
{
public:

    enum Section
    {
        SelectionProperties = 0
    };

public:

    explicit Private()
      : labelSelectionCount(nullptr),
        labelSelectionSize (nullptr),
        labelTotalCount    (nullptr),
        labelTotalSize     (nullptr)
    {
    }

    DTextLabelValue* labelSelectionCount;
    DTextLabelValue* labelSelectionSize;
    DTextLabelValue* labelTotalCount;
    DTextLabelValue* labelTotalSize;
};

ItemSelectionPropertiesTab::ItemSelectionPropertiesTab(QWidget* const parent)
    : DExpanderBox(parent),
      d           (new Private)
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setLineWidth(style()->pixelMetric(QStyle::PM_DefaultFrameWidth));

    // --------------------------------------------------

    const int spacing = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    QWidget* const w1                    = new QWidget(this);
    QGridLayout* const glay1             = new QGridLayout(w1);

    DTextLabelName* const selectionCount = new DTextLabelName(i18n("Count: "),       w1);
    DTextLabelName* const selectionSize  = new DTextLabelName(i18n("Size: "),        w1);
    DTextLabelName* const totalCount     = new DTextLabelName(i18n("Total Count: "), w1);
    DTextLabelName* const totalSize      = new DTextLabelName(i18n("Total Size: "),  w1);


    d->labelSelectionCount               = new DTextLabelValue(QString(), w1);
    d->labelSelectionSize                = new DTextLabelValue(QString(), w1);
    d->labelTotalCount                   = new DTextLabelValue(QString(), w1);
    d->labelTotalSize                    = new DTextLabelValue(QString(), w1);

    glay1->addWidget(selectionCount,         0, 0, 1, 1);
    glay1->addWidget(d->labelSelectionCount, 0, 1, 1, 1);
    glay1->addWidget(selectionSize,          1, 0, 1, 1);
    glay1->addWidget(d->labelSelectionSize,  1, 1, 1, 1);
    glay1->addWidget(totalCount,             2, 0, 1, 1);
    glay1->addWidget(d->labelTotalCount,     2, 1, 1, 1);
    glay1->addWidget(totalSize,              3, 0, 1, 1);
    glay1->addWidget(d->labelTotalSize,      3, 1, 1, 1);


    glay1->setContentsMargins(spacing, spacing, spacing, spacing);
    glay1->setColumnStretch(0, 10);
    glay1->setColumnStretch(1, 25);
    glay1->setSpacing(0);

    insertItem(ItemSelectionPropertiesTab::Private::SelectionProperties, w1,
               QIcon::fromTheme(QLatin1String("dialog-information")),
               i18n("Selection Properties"), QLatin1String("Selection Properties"), true);

    // --------------------------------------------------

    addStretch();
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

void ItemSelectionPropertiesTab::setTotalCount(const QString& str)
{
    d->labelTotalCount->setAdjustedText(str);
}

void ItemSelectionPropertiesTab::setTotalSize(const QString& str)
{
    d->labelTotalSize->setAdjustedText(str);
}

} // namespace Digikam
