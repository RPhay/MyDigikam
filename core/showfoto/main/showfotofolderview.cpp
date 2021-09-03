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
#include <QTreeView>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "showfotosettings.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoFolderView::Private
{
public:

    explicit Private()
      : fsmodel        (nullptr),
        fstree         (nullptr)
    {
    }

    QFileSystemModel* fsmodel;
    QTreeView*        fstree;
};

ShowfotoFolderView::ShowfotoFolderView(QWidget* const parent)
    : QWidget          (parent),
      StateSavingObject(this),
      d                (new Private)
{
    setObjectName(QLatin1String("ShowfotoFolderView Sidebar"));

    const int spacing          = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);
    QVBoxLayout* const layout  = new QVBoxLayout(this);

    d->fsmodel                 = new QFileSystemModel;
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

    layout->addWidget(d->fstree);
    layout->setContentsMargins(0, 0, spacing, 0);

    connect(d->fstree, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(slotItemDoubleClicked(QModelIndex)));
}

ShowfotoFolderView::~ShowfotoFolderView()
{
    delete d;
}

void ShowfotoFolderView::slotItemDoubleClicked(const QModelIndex&)
{
    emit signalItemDoubleClicked(currentPath());
}

QString ShowfotoFolderView::currentPath() const
{
    return d->fsmodel->filePath(d->fstree->currentIndex());
}

void ShowfotoFolderView::setActive(bool active)
{
    if (active)
    {
//        AlbumManager::instance()->setCurrentAlbums(QList<Album*>() << d->albumFolderView->currentAlbum());
    }
}

void ShowfotoFolderView::doLoadState()
{
//    d->albumFolderView->loadState();
}

void ShowfotoFolderView::doSaveState()
{
//    d->albumFolderView->saveState();
}

void ShowfotoFolderView::applySettings()
{
    ShowfotoSettings* const settings = ShowfotoSettings::instance();
//    d->albumFolderView->setEnableToolTips(settings->getShowAlbumToolTips());
//    d->albumFolderView->setExpandNewCurrentItem(settings->getExpandNewCurrentItem());
}


void ShowfotoFolderView::setCurrentPath(const QString& path)
{
    d->fstree->setCurrentIndex(d->fsmodel->index(path));
}

const QIcon ShowfotoFolderView::getIcon()
{
    return QIcon::fromTheme(QLatin1String("document-open-folder"));
}

const QString ShowfotoFolderView::getCaption()
{
    return i18nc("@title: file system tree", "Folders");
}

} // namespace ShowFoto
