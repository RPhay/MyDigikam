/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-05
 * Description : Side Bar Widget for the search.
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

#include "searchsidebarwidget.h"

// Qt includes

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
#include "searchtextbardb.h"
#include "searchtreeview.h"

namespace Digikam
{

class Q_DECL_HIDDEN SearchSideBarWidget::Private
{
public:

    explicit Private()
      : searchSearchBar(nullptr),
        searchTreeView (nullptr),
        searchTabHeader(nullptr)
    {
    }

    SearchTextBarDb*      searchSearchBar;
    NormalSearchTreeView* searchTreeView;
    SearchTabHeader*      searchTabHeader;
};

SearchSideBarWidget::SearchSideBarWidget(QWidget* const parent,
                                         SearchModel* const searchModel,
                                         SearchModificationHelper* const searchModificationHelper)
    : SidebarWidget(parent),
      d            (new Private)
{
    setObjectName(QLatin1String("Search Sidebar"));
    setProperty("Shortcut", Qt::CTRL + Qt::SHIFT + Qt::Key_F6);

    const int spacing         = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    QVBoxLayout* const layout = new QVBoxLayout(this);

    d->searchTabHeader  = new SearchTabHeader(this);
    d->searchTreeView   = new NormalSearchTreeView(this, searchModel, searchModificationHelper);
    d->searchTreeView->setConfigGroup(getConfigGroup());
    d->searchTreeView->filteredModel()->listNormalSearches();
    d->searchTreeView->filteredModel()->setListTemporarySearches(true);
    d->searchTreeView->setAlbumManagerCurrentAlbum(true);
    d->searchSearchBar  = new SearchTextBarDb(this, QLatin1String("ItemIconViewSearchSearchBar"));
    d->searchSearchBar->setModel(d->searchTreeView->filteredModel(),
                                 AbstractAlbumModel::AlbumIdRole, AbstractAlbumModel::AlbumTitleRole);
    d->searchSearchBar->setFilterModel(d->searchTreeView->albumFilterModel());

    layout->addWidget(d->searchTabHeader);
    layout->addWidget(d->searchTreeView);
    layout->setStretchFactor(d->searchTreeView, 1);
    layout->addWidget(d->searchSearchBar);
    layout->setContentsMargins(0, 0, spacing, 0);

    connect(d->searchTreeView, SIGNAL(newSearch()),
            d->searchTabHeader, SLOT(newAdvancedSearch()));

    connect(d->searchTreeView, SIGNAL(editSearch(SAlbum*)),
            d->searchTabHeader, SLOT(editSearch(SAlbum*)));

    connect(d->searchTreeView, SIGNAL(currentAlbumChanged(Album*)),
            d->searchTabHeader, SLOT(selectedSearchChanged(Album*)));

    connect(d->searchTabHeader, SIGNAL(searchShallBeSelected(QList<Album*>)),
            d->searchTreeView, SLOT(setCurrentAlbums(QList<Album*>)));
}

SearchSideBarWidget::~SearchSideBarWidget()
{
    delete d;
}

void SearchSideBarWidget::setActive(bool active)
{
    if (active)
    {
        AlbumManager::instance()->setCurrentAlbums(QList<Album*>() << d->searchTreeView->currentAlbum());
        d->searchTabHeader->selectedSearchChanged(d->searchTreeView->currentAlbum());
    }
}

void SearchSideBarWidget::doLoadState()
{
    d->searchTreeView->loadState();
}

void SearchSideBarWidget::doSaveState()
{
    d->searchTreeView->saveState();
}

void SearchSideBarWidget::applySettings()
{
}

void SearchSideBarWidget::changeAlbumFromHistory(const QList<Album*>& album)
{
    d->searchTreeView->setCurrentAlbums(album);
}

const QIcon SearchSideBarWidget::getIcon()
{
    return QIcon::fromTheme(QLatin1String("edit-find"));
}

const QString SearchSideBarWidget::getCaption()
{
    return i18nc("Advanced search images, access stored searches", "Search");
}

void SearchSideBarWidget::newKeywordSearch()
{
    d->searchTabHeader->newKeywordSearch();
}

void SearchSideBarWidget::newAdvancedSearch()
{
    d->searchTabHeader->newAdvancedSearch();
}

} // namespace Digikam
