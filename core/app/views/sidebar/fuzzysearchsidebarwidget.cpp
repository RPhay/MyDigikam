/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-05
 * Description : Side Bar Widget for the fuzzy search.
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

#include "fuzzysearchsidebarwidget.h"

// Qt includes

#include <QApplication>
#include <QStyle>
#include <QIcon>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "albummanager.h"
#include "applicationsettings.h"
#include "fuzzysearchview.h"

namespace Digikam
{

class Q_DECL_HIDDEN FuzzySearchSideBarWidget::Private
{
public:

    explicit Private()
      : fuzzySearchView         (nullptr),
        searchModificationHelper(nullptr)
    {
    }

    FuzzySearchView*          fuzzySearchView;
    SearchModificationHelper* searchModificationHelper;
};

FuzzySearchSideBarWidget::FuzzySearchSideBarWidget(QWidget* const parent,
                                                   SearchModel* const searchModel,
                                                   SearchModificationHelper* const searchModificationHelper)
    : SidebarWidget(parent),
      d            (new Private)
{
    setObjectName(QLatin1String("Fuzzy Search Sidebar"));
    setProperty("Shortcut", Qt::CTRL + Qt::SHIFT + Qt::Key_F7);

    const int spacing         = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    d->fuzzySearchView        = new FuzzySearchView(searchModel, searchModificationHelper, this);
    d->fuzzySearchView->setConfigGroup(getConfigGroup());

    connect(d->fuzzySearchView, SIGNAL(signalNotificationError(QString,int)),
            this, SIGNAL(signalNotificationError(QString,int)));

    QVBoxLayout* const layout = new QVBoxLayout(this);

    layout->addWidget(d->fuzzySearchView);
    layout->setContentsMargins(0, 0, spacing, 0);
}

FuzzySearchSideBarWidget::~FuzzySearchSideBarWidget()
{
    delete d;
}

void FuzzySearchSideBarWidget::setActive(bool active)
{
    d->fuzzySearchView->setActive(active);

    if (active)
    {
        AlbumManager::instance()->setCurrentAlbums(QList<Album*>() << d->fuzzySearchView->currentAlbum());
    }

    emit signalActive(active);
}

void FuzzySearchSideBarWidget::doLoadState()
{
    d->fuzzySearchView->loadState();
}

void FuzzySearchSideBarWidget::doSaveState()
{
    d->fuzzySearchView->saveState();
}

void FuzzySearchSideBarWidget::applySettings()
{
}

void FuzzySearchSideBarWidget::changeAlbumFromHistory(const QList<Album*>& album)
{
    SAlbum* const salbum = dynamic_cast<SAlbum*>(album.first());
    d->fuzzySearchView->setCurrentAlbum(salbum);
}

const QIcon FuzzySearchSideBarWidget::getIcon()
{
    return QIcon::fromTheme(QLatin1String("tools-wizard"));
}

const QString FuzzySearchSideBarWidget::getCaption()
{
    return i18nc("Fuzzy Search images, as duplicates, sketch, searches by similarities", "Similarity");
}

void FuzzySearchSideBarWidget::newDuplicatesSearch(const QList<PAlbum*>& albums)
{
    d->fuzzySearchView->newDuplicatesSearch(albums);
}

void FuzzySearchSideBarWidget::newDuplicatesSearch(const QList<TAlbum*>& albums)
{
    d->fuzzySearchView->newDuplicatesSearch(albums);
}

void FuzzySearchSideBarWidget::newSimilarSearch(const ItemInfo& imageInfo)
{
    if (imageInfo.isNull())
    {
        return;
    }

    d->fuzzySearchView->setItemInfo(imageInfo);
}

} // namespace Digikam
