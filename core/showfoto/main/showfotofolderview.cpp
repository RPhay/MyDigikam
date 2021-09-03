/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : Side Bar Widget for the Showfoto folder view.
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "showfotofolderview.h"

// Qt includes

#include <QApplication>
#include <QStyle>
#include <QIcon>
#include <QVBoxLayout>
#include <QFileSystemModel>
#include <QFileInfo>
#include <QTreeView>
#include <QHeaderView>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "showfotosettings.h"
#include "showfotofolderviewbar.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoFolderView::Private
{
public:

    explicit Private()
      : fsmodel         (nullptr),
        fstree          (nullptr),
        fsbar           (nullptr)
    {
    }

    static const QString   configFolderViewModeEntry;
    static const QString   configLastPathEntry;

    QFileSystemModel*      fsmodel;
    QTreeView*             fstree;
    ShowfotoFolderViewBar* fsbar;
};

const QString ShowfotoFolderView::Private::configFolderViewModeEntry(QLatin1String("Folder View Mode"));
const QString ShowfotoFolderView::Private::configLastPathEntry(QLatin1String("Last Path"));

ShowfotoFolderView::ShowfotoFolderView(QWidget* const parent)
    : QWidget          (parent),
      StateSavingObject(this),
      d                (new Private)
{
    setObjectName(QLatin1String("ShowfotoFolderView Sidebar"));

    const int spacing          = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    d->fsbar                   = new ShowfotoFolderViewBar(this);

    d->fsmodel                 = new QFileSystemModel(this);
    d->fsmodel->setRootPath(QDir::rootPath());

    QString filter;
    QStringList mimeTypes      = supportedImageMimeTypes(QIODevice::ReadOnly, filter);
    QString patterns           = filter.toLower();
    patterns.append(QLatin1Char(' '));
    patterns.append(filter.toUpper());
    d->fsmodel->setNameFilters(patterns.split(QLatin1Char(' ')));

    d->fstree                  = new QTreeView(this);
    d->fstree->setObjectName(QLatin1String("ShowfotoFolderView"));
    d->fstree->setModel(d->fsmodel);
    d->fstree->setRootIndex(d->fsmodel->index(QDir::rootPath()));
    d->fstree->resizeColumnToContents(1);
    d->fstree->setAlternatingRowColors(true);
    d->fstree->setExpandsOnDoubleClick(false);

    QVBoxLayout* const layout  = new QVBoxLayout(this);
    layout->addWidget(d->fsbar);
    layout->addWidget(d->fstree);
    layout->setContentsMargins(0, 0, spacing, 0);

    connect(d->fstree, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(slotItemDoubleClicked(QModelIndex)));

    connect(d->fsbar, SIGNAL(signalFolderViewModeChanged(int)),
            this, SLOT(slotFolderViewModeChanged(int)));

    connect(d->fsbar, SIGNAL(signalGoHome()),
            this, SLOT(slotGoHome()));

    connect(d->fsbar, SIGNAL(signalCustomPathChanged(QString)),
            this, SLOT(slotCustomPathChanged(QString)));
}

ShowfotoFolderView::~ShowfotoFolderView()
{
    delete d;
}

void ShowfotoFolderView::slotFolderViewModeChanged(int mode)
{
    bool hidden                = (mode != ShowfotoFolderViewBar::FolderViewDetailled);

    QHeaderView* const header  = d->fstree->header();
    header->setSectionHidden(1, hidden);
    header->setSectionHidden(2, hidden);
    header->setSectionHidden(3, hidden);
    header->setSectionHidden(4, hidden);
}

void ShowfotoFolderView::slotItemDoubleClicked(const QModelIndex& index)
{
    if (index.isValid())
    {
        emit signalCurrentPathChanged(currentPath());
        d->fsbar->setCurrentPath(QFileInfo(currentPath()).absolutePath());
    }
}

void ShowfotoFolderView::slotCustomPathChanged(const QString& path)
{
    setCurrentPath(path);
}

void ShowfotoFolderView::slotGoHome()
{
    setCurrentPath(QDir::homePath());
}

QString ShowfotoFolderView::currentPath() const
{
    return d->fsmodel->filePath(d->fstree->currentIndex());
}

void ShowfotoFolderView::setCurrentPath(const QString& path)
{
    QModelIndex index = d->fsmodel->index(path);

    if (index.isValid())
    {
        d->fstree->setCurrentIndex(index);
    }
    else
    {
        d->fsbar->setCurrentPath(currentPath());
    }
}

const QIcon ShowfotoFolderView::getIcon()
{
    return QIcon::fromTheme(QLatin1String("document-open-folder"));
}

const QString ShowfotoFolderView::getCaption()
{
    return i18nc("@title: file system tree", "Folders");
}

void ShowfotoFolderView::doLoadState()
{
    KConfigGroup group = getConfigGroup();

    d->fsbar->setFolderViewMode(group.readEntry(entryName(d->configFolderViewModeEntry), (int)ShowfotoFolderViewBar::FolderViewSimplified));
    setCurrentPath(group.readEntry(entryName(d->configLastPathEntry), QDir::rootPath()));
    slotItemDoubleClicked(d->fstree->currentIndex());
}

void ShowfotoFolderView::doSaveState()
{
    KConfigGroup group = getConfigGroup();

    group.writeEntry(entryName(d->configFolderViewModeEntry), d->fsbar->folderViewMode());
    group.writeEntry(entryName(d->configLastPathEntry),       currentPath());
    group.sync();
}

void ShowfotoFolderView::applySettings()
{
    ShowfotoSettings* const settings = ShowfotoSettings::instance();
//    d->albumFolderView->setEnableToolTips(settings->getShowAlbumToolTips());
//    d->albumFolderView->setExpandNewCurrentItem(settings->getExpandNewCurrentItem());
}

void ShowfotoFolderView::setActive(bool active)
{
    if (active)
    {
//        AlbumManager::instance()->setCurrentAlbums(QList<Album*>() << d->albumFolderView->currentAlbum());
    }
}

} // namespace ShowFoto
