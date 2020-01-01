/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-05
 * Description : Side Bar Widget for People
 *
 * Copyright (C) 2009-2010 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2010-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include "searchtextbar.h"
#include "tagfolderview.h"
#include "timelinewidget.h"
#include "facescanwidget.h"
#include "facesdetector.h"
#include "dnotificationwidget.h"

namespace Digikam
{

class Q_DECL_HIDDEN PeopleSideBarWidget::Private
{
public:

    explicit Private()
      : personIcon(nullptr),
        textLabel(nullptr),
        rescanButton(nullptr),
        searchModificationHelper(nullptr),
        settingsWdg(nullptr),
        tagFolderView(nullptr),
        tagSearchBar(nullptr)
    {
    }

    QLabel*                   personIcon;
    QLabel*                   textLabel;
    QPushButton*              rescanButton;
    SearchModificationHelper* searchModificationHelper;
    FaceScanWidget*           settingsWdg;
    TagFolderView*            tagFolderView;
    SearchTextBar*            tagSearchBar;
};

PeopleSideBarWidget::PeopleSideBarWidget(QWidget* const parent,
                                         TagModel* const model,
                                         SearchModificationHelper* const searchModificationHelper)
    : SidebarWidget(parent),
      d(new Private)
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

    QVBoxLayout* const vlay     = new QVBoxLayout;
    QHBoxLayout* const hlay     = new QHBoxLayout;
    d->tagFolderView            = new TagFolderView(this, model);
    d->tagFolderView->setConfigGroup(getConfigGroup());
    d->tagFolderView->setExpandNewCurrentItem(true);
    d->tagFolderView->setAlbumManagerCurrentAlbum(true);
    d->tagFolderView->setShowDeleteFaceTagsAction(true);

    d->tagFolderView->filteredModel()->listOnlyTagsWithProperty(TagPropertyName::person());
    d->tagFolderView->filteredModel()->setFilterBehavior(AlbumFilterModel::StrictFiltering);

    d->tagSearchBar   = new SearchTextBar(this, QLatin1String("ItemIconViewPeopleSearchBar"));
    d->tagSearchBar->setHighlightOnResult(true);
    d->tagSearchBar->setModel(d->tagFolderView->filteredModel(),
                              AbstractAlbumModel::AlbumIdRole, AbstractAlbumModel::AlbumTitleRole);
    d->tagSearchBar->setFilterModel(d->tagFolderView->albumFilterModel());

    d->personIcon     = new QLabel;
    d->personIcon->setPixmap(QIcon::fromTheme(QLatin1String("edit-image-face-show")).pixmap(48));

    d->textLabel      = new QLabel;
    d->textLabel->setWordWrap(true);
    d->textLabel->setText(i18nc("@info",
                                "digiKam can search for faces in your photos. "
                                "When you have identified your friends on a number of photos, "
                                "it can also recognize the people shown on your photos."));

    d->settingsWdg    = new FaceScanWidget(this);

    d->rescanButton   = new QPushButton;
    d->rescanButton->setText(i18n("Scan collection for faces"));

    hlay->addWidget(d->personIcon);
    hlay->addWidget(d->textLabel);

    vlay->addLayout(hlay);
    vlay->addWidget(d->tagFolderView, 10);
    vlay->addWidget(d->tagSearchBar);
    vlay->addWidget(d->settingsWdg);
    vlay->addWidget(d->rescanButton);
    vlay->setContentsMargins(0, 0, spacing, 0);

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
        emit signalNofificationError(i18n("Face recognition is aborted, because "
                                          "there are no identities to recognize. "
                                          "Please add new identities."),
                                     DNotificationWidget::Information);
    }
}

void PeopleSideBarWidget::slotScanComplete()
{
    d->settingsWdg->setEnabled(true);
    d->rescanButton->setEnabled(true);
    d->settingsWdg->resetRetrainAllButton();
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
