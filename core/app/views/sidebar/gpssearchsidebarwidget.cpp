/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-05
 * Description : Side Bar Widget for the gps search.
 *
 * Copyright (C) 2009-2010 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright (C) 2014      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * Copyright (C) 2010      by Aditya Bhatt <adityabhatt1991 at gmail dot com>
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

#include "gpssearchsidebarwidget.h"

// Qt includes

#include <QScrollArea>
#include <QApplication>
#include <QStyle>
#include <QIcon>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "albumpointer.h"
#include "applicationsettings.h"
#include "editablesearchtreeview.h"
#include "searchfolderview.h"
#include "searchtabheader.h"
#include "searchtextbar.h"
#include "searchtreeview.h"

namespace Digikam
{

class Q_DECL_HIDDEN GPSSearchSideBarWidget::Private
{
public:

    explicit Private()
      : gpsSearchView(nullptr)
    {
    }

    GPSSearchView* gpsSearchView;
};

GPSSearchSideBarWidget::GPSSearchSideBarWidget(QWidget* const parent,
                                               SearchModel* const searchModel,
                                               SearchModificationHelper* const searchModificationHelper,
                                               ItemFilterModel* const imageFilterModel,
                                               QItemSelectionModel* const itemSelectionModel)
    : SidebarWidget(parent),
      d            (new Private)
{
    setObjectName(QLatin1String("GPS Search Sidebar"));
    setProperty("Shortcut", Qt::CTRL + Qt::SHIFT + Qt::Key_F8);

    d->gpsSearchView = new GPSSearchView(this, searchModel, searchModificationHelper, imageFilterModel, itemSelectionModel);
    d->gpsSearchView->setConfigGroup(getConfigGroup());

    const int spacing             = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    QScrollArea* const scrollArea = new QScrollArea(this);
    QVBoxLayout* const layout     = new QVBoxLayout(this);

    layout->addWidget(scrollArea);
    layout->setContentsMargins(0, 0, spacing, 0);
    scrollArea->setWidget(d->gpsSearchView);
    scrollArea->setWidgetResizable(true);

    connect(d->gpsSearchView, SIGNAL(signalMapSoloItems(QList<qlonglong>,QString)),
            this, SIGNAL(signalMapSoloItems(QList<qlonglong>,QString)));
}

GPSSearchSideBarWidget::~GPSSearchSideBarWidget()
{
    delete d;
}

void GPSSearchSideBarWidget::setActive(bool active)
{
    d->gpsSearchView->setActive(active);
}

void GPSSearchSideBarWidget::doLoadState()
{
    d->gpsSearchView->loadState();
}

void GPSSearchSideBarWidget::doSaveState()
{
    d->gpsSearchView->saveState();
}

void GPSSearchSideBarWidget::applySettings()
{
}

void GPSSearchSideBarWidget::changeAlbumFromHistory(const QList<Album*>& album)
{
    d->gpsSearchView->changeAlbumFromHistory(dynamic_cast<SAlbum*>(album.first()));
}

const QIcon GPSSearchSideBarWidget::getIcon()
{
    return QIcon::fromTheme(QLatin1String("globe"));
}

const QString GPSSearchSideBarWidget::getCaption()
{
    return i18nc("Search images on a map", "Map");
}

} // namespace Digikam
