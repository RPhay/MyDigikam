/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2000-12-05
 * Description : filters view for the right sidebar
 *
 * SPDX-FileCopyrightText: 2009-2010 by Johannes Wienke <languitar at semipol dot de>
 * SPDX-FileCopyrightText: 2010-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2011-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText:      2011 by Michael G. Hansen <mike at mghansen dot de>
 * SPDX-FileCopyrightText:      2014 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "filtersidebarwidget_p.h"

namespace Digikam
{

void FilterSideBarWidget::populatePropertiesView()
{
    d->expbox     = new DExpanderBox(this);
    d->expbox->setObjectName(QLatin1String("FilterSideBarWidget Expander"));

    // --------------------------------------------------------------------------------------------------------

    d->textFilter = new TextFilter(d->expbox);
    d->expbox->insertItem(Private::TEXT, d->textFilter, QIcon::fromTheme(QLatin1String("text-field")),
                          i18n("Text Filter"), QLatin1String("TextFilter"), true);

    // --------------------------------------------------------------------------------------------------------

    d->mimeFilter = new MimeFilter(d->expbox);
    d->expbox->insertItem(Private::MIME, d->mimeFilter, QIcon::fromTheme(QLatin1String("folder-open")),
                          i18n("MIME Type Filter"), QLatin1String("TypeMimeFilter"), true);

    // --------------------------------------------------------------------------------------------------------

    d->geolocationFilter = new GeolocationFilter(d->expbox);
    d->expbox->insertItem(Private::GEOLOCATION, d->geolocationFilter, QIcon::fromTheme(QLatin1String("globe")),
                          i18n("Geolocation Filter"), QLatin1String("TypeGeolocationFilter"), true);

    // --------------------------------------------------------------------------------------------------------

    QWidget* const box4 = new QWidget(d->expbox);
    d->colorLabelFilter = new ColorLabelFilter(box4);
    d->pickLabelFilter  = new PickLabelFilter(box4);
    d->ratingFilter     = new RatingFilter(box4);

    QGridLayout* const lay4 = new QGridLayout(box4);
    lay4->addWidget(d->colorLabelFilter, 0, 0, 1, 3);
    lay4->addWidget(d->pickLabelFilter,  1, 0, 1, 1);
    lay4->addWidget(d->ratingFilter,     1, 2, 1, 1);
    lay4->setColumnStretch(2, 1);
    lay4->setColumnStretch(3, 10);
    lay4->setContentsMargins(QMargins());
    lay4->setSpacing(0);

    d->expbox->insertItem(Private::LABELS, box4, QIcon::fromTheme(QLatin1String("folder-favorites")),
                          i18n("Labels Filter"), QLatin1String("LabelsFilter"), true);

    d->expanderVlay = dynamic_cast<QVBoxLayout*>(dynamic_cast<QScrollArea*>(d->expbox)->widget()->layout());
    d->space        = new QWidget();
    d->expanderVlay->addWidget(d->space);
    d->expanderVlay->setStretchFactor(d->space, 100);

    d->tabWidget->insertTab(Private::PropertiesTab, d->expbox, i18nc("@title", "Properties"));

    // ---

    connect(d->mimeFilter, SIGNAL(activated(int)),
            this, SIGNAL(signalMimeTypeFilterChanged(int)));

    connect(d->geolocationFilter, SIGNAL(signalFilterChanged(ItemFilterSettings::GeolocationCondition)),
            this, SIGNAL(signalGeolocationFilterChanged(ItemFilterSettings::GeolocationCondition)));

    connect(d->textFilter, SIGNAL(signalSearchTextFilterSettings(SearchTextFilterSettings)),
            this, SIGNAL(signalSearchTextFilterChanged(SearchTextFilterSettings)));

    connect(d->colorLabelFilter, SIGNAL(signalColorLabelSelectionChanged(QList<ColorLabel>)),
            this, SLOT(slotColorLabelFilterChanged(QList<ColorLabel>)));

    connect(d->pickLabelFilter, SIGNAL(signalPickLabelSelectionChanged(QList<PickLabel>)),
            this, SLOT(slotPickLabelFilterChanged(QList<PickLabel>)));

    connect(d->ratingFilter, SIGNAL(signalRatingFilterChanged(int,ItemFilterSettings::RatingCondition,bool)),
            this, SIGNAL(signalRatingFilterChanged(int,ItemFilterSettings::RatingCondition,bool)));
}

void FilterSideBarWidget::setFocusToTextFilter()
{
    d->textFilter->searchTextBar()->setFocus();
}

void FilterSideBarWidget::slotFilterMatchesForText(bool match)
{
    d->textFilter->searchTextBar()->slotSearchResult(match);
}

void FilterSideBarWidget::slotColorLabelFilterChanged(const QList<ColorLabel>& list)
{
    Q_UNUSED(list);

    checkFilterChanges();
}

void FilterSideBarWidget::slotPickLabelFilterChanged(const QList<PickLabel>& list)
{
    Q_UNUSED(list);

    checkFilterChanges();
}

} // namespace Digikam
