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

#include "showfotofolderviewsidebar.h"

// Qt includes

#include <QApplication>
#include <QStyle>
#include <QIcon>
#include <QVBoxLayout>
#include <QFileSystemModel>
#include <QFileInfo>
#include <QHeaderView>
#include <QDir>
#include <QMenu>
#include <QUndoStack>
#include <QScrollBar>
#include <QContextMenuEvent>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "showfotosettings.h"
#include "showfotofolderviewbar.h"
#include "showfotofolderviewundo.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoFolderViewSideBar::Private
{

public:

    explicit Private()
      : fsmodel     (nullptr),
        fsview      (nullptr),
        fsbar       (nullptr),
        fsstack     (nullptr),
        fsmenu      (nullptr)
    {
    }

    static const QString   configLastPathEntry;

    QFileSystemModel*      fsmodel;
    QListView*             fsview;
    ShowfotoFolderViewBar* fsbar;
    QUndoStack*            fsstack;
    QMenu*                 fsmenu;
};

const QString ShowfotoFolderViewSideBar::Private::configLastPathEntry(QLatin1String("Last Path"));

ShowfotoFolderViewSideBar::ShowfotoFolderViewSideBar(QWidget* const parent)
    : QWidget          (parent),
      StateSavingObject(this),
      d                (new Private)
{
    setObjectName(QLatin1String("ShowfotoFolderView Sidebar"));

    // --- Populate the model

    d->fsmodel                 = new QFileSystemModel(this);
    d->fsmodel->setRootPath(QDir::rootPath());

    QString filter;
    QStringList mimeTypes      = supportedImageMimeTypes(QIODevice::ReadOnly, filter);
    QString patterns           = filter.toLower();
    patterns.append(QLatin1Char(' '));
    patterns.append(filter.toUpper());
    d->fsmodel->setNameFilters(patterns.split(QLatin1Char(' ')));

    // if an item fails the filter, hide it

    d->fsmodel->setNameFilterDisables(false);

    d->fsstack                 = new QUndoStack(this);

    // --- Popumate the view

    const int spacing          = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    d->fsbar                   = new ShowfotoFolderViewBar(this);

    d->fsview                  = new QListView(this);
    d->fsview->setObjectName(QLatin1String("ShowfotoFolderViewSideBar"));
    d->fsview->setModel(d->fsmodel);
    d->fsview->setRootIndex(d->fsmodel->index(QDir::rootPath()));
    d->fsview->setAlternatingRowColors(true);
    d->fsview->installEventFilter(this);

    QVBoxLayout* const layout  = new QVBoxLayout(this);
    layout->addWidget(d->fsbar);
    layout->addWidget(d->fsview);
    layout->setContentsMargins(0, 0, spacing, 0);

    // --- Populate context menu

    d->fsmenu                  = new QMenu(d->fsview);
    d->fsmenu->setTitle(i18nc("@title", "Folder-View Options"));
    d->fsmenu->addAction(QIcon::fromTheme(QLatin1String("go-previous")),
                         i18nc("menu", "Go to Previous"),
                         d->fsstack,
                         SLOT(undo()));
    d->fsmenu->addAction(QIcon::fromTheme(QLatin1String("go-next")),
                         i18nc("menu", "Go to Next"),
                         d->fsstack,
                         SLOT(redo()));
    d->fsmenu->addAction(QIcon::fromTheme(QLatin1String("go-home")),
                         i18nc("menu", "Go Home"),
                         this,
                         SLOT(slotGoHome()));
    d->fsmenu->addAction(QIcon::fromTheme(QLatin1String("go-up")),
                         i18nc("menu", "Go Up"),
                         this,
                         SLOT(slotGoUp()));
    d->fsmenu->addSeparator(),
    d->fsmenu->addAction(QIcon::fromTheme(QLatin1String("folder-download")),
                         i18nc("menu", "Load Contents"),
                         this,
                         SLOT(slotLoadContents()));

    // --- Setup connextions

    connect(d->fsview, SIGNAL(activated(QModelIndex)),
            this, SLOT(slotItemActivated(QModelIndex)));

    connect(d->fsview, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(slotItemDoubleClicked(QModelIndex)));

    connect(d->fsbar, SIGNAL(signalGoHome()),
            this, SLOT(slotGoHome()));

    connect(d->fsbar, SIGNAL(signalGoUp()),
            this, SLOT(slotGoUp()));

    connect(d->fsbar, SIGNAL(signalCustomPathChanged(QString)),
            this, SLOT(slotCustomPathChanged(QString)));

    connect(d->fsstack, SIGNAL(canUndoChanged(bool)),
            d->fsbar, SLOT(slotPreviousEnabled(bool)));

    connect(d->fsstack, SIGNAL(canRedoChanged(bool)),
            d->fsbar, SLOT(slotNextEnabled(bool)));

    connect(d->fsbar, SIGNAL(signalGoNext()),
            d->fsstack, SLOT(redo()));

    connect(d->fsbar, SIGNAL(signalGoPrevious()),
            d->fsstack, SLOT(undo()));
}

ShowfotoFolderViewSideBar::~ShowfotoFolderViewSideBar()
{
    delete d;
}

bool ShowfotoFolderViewSideBar::eventFilter(QObject* obj, QEvent* evt)
{
    if (obj == d->fsview)
    {
        if (evt->type() == QEvent::ContextMenu)
        {
            QContextMenuEvent* const e = static_cast<QContextMenuEvent*>(evt);
            d->fsmenu->exec(e->globalPos());
        }
    }

    return QObject::eventFilter(obj, evt);
}

void ShowfotoFolderViewSideBar::slotLoadContents()
{
    QModelIndex index = d->fsmodel->index(currentPath());
    loadContents(index);
    emit signalCurrentPathChanged(currentPath());
}

void ShowfotoFolderViewSideBar::slotItemDoubleClicked(const QModelIndex& index)
{
    loadContents(index);
}

void ShowfotoFolderViewSideBar::loadContents(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }

    if (d->fsmodel->isDir(index))
    {
        setCurrentPath(d->fsmodel->filePath(index));
    }

    if (QApplication::keyboardModifiers() & (Qt::ShiftModifier | Qt::ControlModifier))
    {
        emit signalCurrentPathChanged(currentPath());
    }
}

void ShowfotoFolderViewSideBar::slotCustomPathChanged(const QString& path)
{
    setCurrentPath(path);
}

void ShowfotoFolderViewSideBar::slotGoHome()
{
    setCurrentPath(QDir::homePath());
}

void ShowfotoFolderViewSideBar::slotGoUp()
{
    QDir dir(currentFolder());
    dir.cdUp();

    // Is this the same as going back?  If so just go back, so we can keep the view scroll position.

    if (d->fsstack->canUndo())
    {
        const ShowfotoFolderViewUndo* lastDir = static_cast<const ShowfotoFolderViewUndo*>
                                                (d->fsstack->command(d->fsstack->index() - 1));

        if (lastDir->undoPath() == dir.path())
        {
            d->fsstack->undo();
            return;
        }
    }

    setCurrentPath(dir.absolutePath());
}

QString ShowfotoFolderViewSideBar::currentFolder() const
{
    QString path = d->fsmodel->rootPath();

    if (!path.endsWith(QDir::separator()))
    {
        path.append(QDir::separator());
    }

    return path;
}

QString ShowfotoFolderViewSideBar::currentPath() const
{
    return (d->fsmodel->filePath(d->fsview->currentIndex()));
}

void ShowfotoFolderViewSideBar::setCurrentPath(const QString& newPathNative)
{
    QString newPath = QDir::fromNativeSeparators(newPathNative);

    if (!newPath.endsWith(QDir::separator()))
    {
        newPath.append(QDir::separator());
    }

    QString oldPath(d->fsmodel->rootPath());

    if (!oldPath.endsWith(QDir::separator()))
    {
        oldPath.append(QDir::separator());
    }

    if (oldPath == newPath)
    {
        return;
    }

    QFileInfo info(newPath);

    if (!info.exists())
    {
        return;
    }

    if (info.isDir())
    {
        QModelIndex index = d->fsmodel->index(newPath);

        if (index.isValid())
        {
            d->fsstack->push(new ShowfotoFolderViewUndo(this, newPath));
            d->fsmodel->setRootPath(newPath);
            d->fsview->setRootIndex(index);
        }
    }
    else
    {
        QModelIndex index = d->fsmodel->index(newPath);

        if (index.isValid())
        {
            d->fsview->setCurrentIndex(index);
        }
    }
}

void ShowfotoFolderViewSideBar::setCurrentPathWithoutUndo(const QString& newPath)
{
    QModelIndex index = d->fsmodel->setRootPath(newPath);

    if (index.isValid())
    {
        d->fsview->setRootIndex(index);
        d->fsbar->setCurrentPath(currentFolder());
    }
}

const QIcon ShowfotoFolderViewSideBar::getIcon()
{
    return QIcon::fromTheme(QLatin1String("document-open-folder"));
}

const QString ShowfotoFolderViewSideBar::getCaption()
{
    return i18nc("@title: file system tree", "Folders");
}

void ShowfotoFolderViewSideBar::doLoadState()
{
    KConfigGroup group = getConfigGroup();

    setCurrentPathWithoutUndo(group.readEntry(entryName(d->configLastPathEntry), QDir::rootPath()));
    slotItemDoubleClicked(d->fsview->currentIndex());
}

void ShowfotoFolderViewSideBar::doSaveState()
{
    KConfigGroup group = getConfigGroup();

    group.writeEntry(entryName(d->configLastPathEntry), currentPath());
    group.sync();
}

void ShowfotoFolderViewSideBar::applySettings()
{
/*
    ShowfotoSettings* const settings = ShowfotoSettings::instance();
    d->albumFolderView->setEnableToolTips(settings->getShowAlbumToolTips());
    d->albumFolderView->setExpandNewCurrentItem(settings->getExpandNewCurrentItem());
*/
}

void ShowfotoFolderViewSideBar::setActive(bool active)
{
    if (active)
    {
//        AlbumManager::instance()->setCurrentAlbums(QList<Album*>() << d->albumFolderView->currentAlbum());
    }
}

} // namespace ShowFoto
