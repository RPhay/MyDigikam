/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-05
 * Description : Side Bar Widget for People
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

#include "peoplesidebarwidget.h"

// Qt includes

#include <QLabel>
#include <QScrollArea>
#include <QApplication>
#include <QStyle>
#include <QPushButton>
#include <QIcon>
#include <QHBoxLayout>
#include <QVBoxLayout>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "searchtextbardb.h"
#include "tagfolderview.h"
#include "timelinewidget.h"
#include "facescanwidget.h"
#include "facesdetector.h"
#include "dnotificationwidget.h"
#include "applicationsettings.h"

namespace Digikam
{

class Q_DECL_HIDDEN PeopleSideBarWidget::Private
{
public:

    explicit Private()
      : rescanButton            (nullptr),
        searchModificationHelper(nullptr),
        settingsWdg             (nullptr),
        tagFolderView           (nullptr),
        tagSearchBar            (nullptr)
    {
    }

    QPushButton*              rescanButton;
    SearchModificationHelper* searchModificationHelper;
    FaceScanWidget*           settingsWdg;
    TagFolderView*            tagFolderView;
    SearchTextBarDb*          tagSearchBar;
};

PeopleSideBarWidget::PeopleSideBarWidget(QWidget* const parent,
                                         TagModel* const model,
                                         SearchModificationHelper* const searchModificationHelper)
    : SidebarWidget(parent),
      d            (new Private)
{
    setObjectName(QLatin1String("People Sidebar"));
    setProperty("Shortcut", Qt::CTRL + Qt::SHIFT + Qt::Key_F9);
    d->searchModificationHelper   = searchModificationHelper;

    const int spacing             = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    QWidget* const     mainView   = new QWidget(this);
    QScrollArea* const scrollArea = new QScrollArea(this);
    QVBoxLayout* const mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(scrollArea);
    mainLayout->setContentsMargins(0, 0, spacing, 0);
    scrollArea->setWidget(mainView);
    scrollArea->setWidgetResizable(true);

    model->setColumnHeader(getCaption());

    QVBoxLayout* const vlay     = new QVBoxLayout;
    d->tagFolderView            = new TagFolderView(this, model);
    d->tagFolderView->setConfigGroup(getConfigGroup());
    d->tagFolderView->setAlbumManagerCurrentAlbum(true);
    d->tagFolderView->setShowDeleteFaceTagsAction(true);

    d->tagFolderView->filteredModel()->listOnlyTagsWithProperty(TagPropertyName::person());
    d->tagFolderView->filteredModel()->setFilterBehavior(AlbumFilterModel::StrictFiltering);

    d->tagSearchBar   = new SearchTextBarDb(this, QLatin1String("ItemIconViewPeopleSearchBar"));
    d->tagSearchBar->setHighlightOnResult(true);
    d->tagSearchBar->setModel(d->tagFolderView->filteredModel(),
                              AbstractAlbumModel::AlbumIdRole, AbstractAlbumModel::AlbumTitleRole);
    d->tagSearchBar->setFilterModel(d->tagFolderView->albumFilterModel());

    d->settingsWdg    = new FaceScanWidget(this);

    d->rescanButton   = new QPushButton;
    d->rescanButton->setText(i18n("Scan collection for faces"));

    vlay->addWidget(d->tagFolderView, 10);
    vlay->addWidget(d->tagSearchBar);
    vlay->addWidget(d->settingsWdg);
    vlay->addWidget(d->rescanButton);
    vlay->setContentsMargins(spacing, spacing, spacing, spacing);

    mainView->setLayout(vlay);

    connect(d->tagFolderView, SIGNAL(signalFindDuplicates(QList<TAlbum*>)),
            this, SIGNAL(signalFindDuplicates(QList<TAlbum*>)));

    connect(d->rescanButton, SIGNAL(pressed()),
            this, SLOT(slotScanForFaces()) );
}

PeopleSideBarWidget::~PeopleSideBarWidget()
{
    delete d;
}

void PeopleSideBarWidget::slotInit()
{
    loadState();
}

void PeopleSideBarWidget::setActive(bool active)
{
    emit requestFaceMode(active);

    if (active)
    {
        d->tagFolderView->setCurrentAlbums(QList<Album*>() << d->tagFolderView->currentAlbum());

        if (!ApplicationSettings::instance()->getHelpBoxNotificationSeen())
        {
            QString msg = i18n("Welcome to Face Management in digiKam. "
                               "If this is your first time using this feature, please consider "
                               "using the Help Box in the Bottom Left Side Panel.");
            emit signalNotificationError(msg, DNotificationWidget::Information);
            ApplicationSettings::instance()->setHelpBoxNotificationSeen(true);
        }
    }
}

void PeopleSideBarWidget::doLoadState()
{
    d->tagFolderView->loadState();
    d->settingsWdg->loadState();
}

void PeopleSideBarWidget::doSaveState()
{
    d->tagFolderView->saveState();
    d->settingsWdg->saveState();
}

void PeopleSideBarWidget::applySettings()
{
    ApplicationSettings* const settings = ApplicationSettings::instance();
    d->tagFolderView->setExpandNewCurrentItem(settings->getExpandNewCurrentItem());
}

void PeopleSideBarWidget::changeAlbumFromHistory(const QList<Album*>& album)
{
    d->tagFolderView->setCurrentAlbums(album);
}

void PeopleSideBarWidget::slotScanForFaces()
{
    FaceScanSettings faceScanSettings = d->settingsWdg->settings();

    if (!d->settingsWdg->settingsConflicted())
    {
        FacesDetector* const tool = new FacesDetector(faceScanSettings);
        tool->start();

        d->settingsWdg->setEnabled(false);
        d->rescanButton->setEnabled(false);

        connect(tool, SIGNAL(signalComplete()),
                this, SLOT(slotScanComplete()));

        connect(tool, SIGNAL(signalCanceled()),
                this, SLOT(slotScanComplete()));
    }
    else
    {
        emit signalNotificationError(i18n("Face recognition is aborted, because "
                                          "there are no identities to recognize. "
                                          "Please add new identities."),
                                     DNotificationWidget::Information);
    }
}

void PeopleSideBarWidget::slotScanComplete()
{
    d->settingsWdg->setEnabled(true);
    d->rescanButton->setEnabled(true);
}

const QIcon PeopleSideBarWidget::getIcon()
{
    return QIcon::fromTheme(QLatin1String("edit-image-face-show"));
}

const QString PeopleSideBarWidget::getCaption()
{
    return i18nc("Browse images sorted by depicted people", "People");
}

} // namespace Digikam
