/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-09-27
 * Description : Side Bar Widget for the Showfoto stack view.
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

#include "showfotostackviewsidebar.h"

// Qt includes

#include <QApplication>
#include <QStyle>
#include <QIcon>
#include <QUndoStack>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include <QSplitter>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "showfoto.h"
#include "showfotostackviewlist.h"
#include "showfotostackviewmodel.h"
#include "showfotothumbnailbar.h"
//#include "showfotostackviewbar.h"
//#include "showfotostackviewbookmarks.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoStackViewSideBar::Private
{

public:

    explicit Private()
      : fsmodel     (nullptr),
        fsview      (nullptr),
/*      fsbar       (nullptr),
        fsmarks     (nullptr),
        splitter    (nullptr),
*/
        fsSortOrder (Qt::AscendingOrder),
        fsRole      (ShowfotoStackViewList::FileName)
    {
    }

    static const QString                 configIconSizeEntry;
    static const QString                 configLastFolderEntry;
    static const QString                 configFolderViewModeEntry;
    static const QString                 configFolderViewTypeMimeEntry;
    static const QString                 configBookmarksVisibleEntry;
    static const QString                 configSplitterStateEntry;

    ShowfotoStackViewModel*              fsmodel;
    ShowfotoStackViewList*               fsview;
/*
    ShowfotoStackViewBar*                fsbar;
    ShowfotoStackViewBookmarks*          fsmarks;
    QSplitter*                           splitter;
*/
    QList<DPluginAction*>                pluginActions;
    Qt::SortOrder                        fsSortOrder;
    ShowfotoStackViewList::StackViewRole fsRole;
};

const QString ShowfotoStackViewSideBar::Private::configIconSizeEntry(QLatin1String("Icon Size"));
const QString ShowfotoStackViewSideBar::Private::configLastFolderEntry(QLatin1String("Last Folder"));
const QString ShowfotoStackViewSideBar::Private::configFolderViewModeEntry(QLatin1String("Folder View Mode"));
const QString ShowfotoStackViewSideBar::Private::configFolderViewTypeMimeEntry(QLatin1String("Folder View Type Mime"));
const QString ShowfotoStackViewSideBar::Private::configBookmarksVisibleEntry(QLatin1String("Bookmarks Visible"));
const QString ShowfotoStackViewSideBar::Private::configSplitterStateEntry(QLatin1String("Splitter State"));

ShowfotoStackViewSideBar::ShowfotoStackViewSideBar(Showfoto* const parent)
    : QWidget          (parent),
      StateSavingObject(this),
      d                (new Private)
{
    setObjectName(QLatin1String("ShowfotoStackView Sidebar"));

    // --- Populate the view

//    d->fsbar                   = new ShowfotoStackViewBar(this);
    d->fsview                  = new ShowfotoStackViewList(this);
    d->fsview->setEnableToolTips(true);
/*
    d->fsmarks                 = new ShowfotoStackViewBookmarks(this);

    d->splitter                = new QSplitter(Qt::Vertical, this);
    d->splitter->addWidget(d->fsview);
    d->splitter->addWidget(d->fsmarks);
    d->splitter->setStretchFactor(0, 10);
    d->splitter->setStretchFactor(1, 3);
*/
    QVBoxLayout* const layout  = new QVBoxLayout(this);
//    layout->addWidget(d->fsbar);
//    layout->addWidget(d->splitter);
    layout->addWidget(d->fsview);
    layout->setContentsMargins(0, 0, 0, 0);

    // --- Setup connections
/*
    connect(d->fsbar, SIGNAL(signalSetup()),
            this, SIGNAL(signalSetup()));

    connect(d->fsbar, SIGNAL(signalShowBookmarks(bool)),
            this, SLOT(slotShowBookmarks(bool)));

    connect(d->fsbar, SIGNAL(signalViewModeChanged(int)),
            this, SLOT(slotViewModeChanged(int)));

    connect(d->fsbar, SIGNAL(signalIconSizeChanged(int)),
            d->fsview, SLOT(slotIconSizeChanged(int)));

    connect(d->fsbar, SIGNAL(signalGoHome()),
            this, SLOT(slotGoHome()));

    connect(d->fsbar, SIGNAL(signalGoUp()),
            this, SLOT(slotGoUp()));

    connect(d->fsbar, SIGNAL(signalLoadContents()),
            this, SLOT(slotLoadContents()));

    connect(d->fsmarks, SIGNAL(signalLoadContents()),
            this, SLOT(slotLoadContents()));

    connect(d->fsbar, SIGNAL(signalCustomPathChanged(QString)),
            this, SLOT(slotCustomPathChanged(QString)));

    connect(d->fsbar, SIGNAL(signalTypeMimesChanged(QString)),
            this, SLOT(slotTypeMimesChanged(QString)));

    connect(d->fsbar, SIGNAL(signalGoNext()),
            this, SLOT(slotRedo()));

    connect(d->fsbar, SIGNAL(signalGoPrevious()),
            this, SLOT(slotUndo()));

    connect(d->fsstack, SIGNAL(canUndoChanged(bool)),
            d->fsbar, SLOT(slotPreviousEnabled(bool)));

    connect(d->fsstack, SIGNAL(canRedoChanged(bool)),
            d->fsbar, SLOT(slotNextEnabled(bool)));

    connect(d->fsview, SIGNAL(signalAddBookmark()),
            this, SIGNAL(signalAddBookmark()));
*/

    connect(d->fsview, SIGNAL(signalShowfotoItemInfoActivated(ShowfotoItemInfo)),
            this, SIGNAL(signalShowfotoItemInfoActivated(ShowfotoItemInfo)));
}

ShowfotoStackViewSideBar::~ShowfotoStackViewSideBar()
{
    delete d;
}

void ShowfotoStackViewSideBar::setThumbbar(ShowfotoThumbnailBar* const thumbbar)
{
    d->fsview->setThumbbar(thumbbar);
}

/*
void ShowfotoStackViewSideBar::slotTypeMimesChanged(const QString& patterns)
{
    d->fsmodel->setNameFilters(patterns.split(QLatin1Char(' ')));
}

void ShowfotoStackViewSideBar::slotLoadContents()
{
    QModelIndex index = d->fsmodel->index(currentPath());
    loadContents(index);
}

void ShowfotoStackViewSideBar::loadContents(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }

    QStringList lst;

    if      (d->fsmodel->isDir(index))
    {
        setCurrentPath(d->fsmodel->filePath(index));

        lst = d->fsmodel->currentFilesPath();
    }
    else if (d->fsmodel->fileInfo(index).isFile())
    {
        lst = d->fsmodel->currentFilesPath();
    }

    qCDebug(DIGIKAM_SHOWFOTO_LOG) << "Load Contents from:" << currentPath() << "Files:" << lst;

    if (!lst.isEmpty())
    {
        emit signalLoadContentsFromFiles(lst);
    }
}

void ShowfotoStackViewSideBar::slotCustomPathChanged(const QString& path)
{
    setCurrentPath(path);
}

void ShowfotoStackViewSideBar::slotUndo()
{
    d->fsstack->undo();
}

void ShowfotoStackViewSideBar::slotRedo()
{
    d->fsstack->redo();
}

void ShowfotoStackViewSideBar::slotGoHome()
{
    setCurrentPath(QDir::homePath());
}

void ShowfotoStackViewSideBar::slotGoUp()
{
    QDir dir(currentFolder());
    dir.cdUp();

    // Is this the same as going back?  If so just go back, so we can keep the view scroll position.

    if (d->fsstack->canUndo())
    {
        const ShowfotoStackViewUndo* lastDir = static_cast<const ShowfotoStackViewUndo*>
                                                (d->fsstack->command(d->fsstack->index() - 1));

        if (lastDir->undoPath() == dir.path())
        {
            d->fsstack->undo();
            return;
        }
    }

    setCurrentPath(dir.absolutePath());
}

QString ShowfotoStackViewSideBar::currentFolder() const
{
    QString path = d->fsmodel->rootPath();

    if (!path.endsWith(QDir::separator()))
    {
        path.append(QDir::separator());
    }

    return path;
}

QString ShowfotoStackViewSideBar::currentPath() const
{
    QModelIndex index = d->fsview->currentIndex();

    if (index.isValid())
    {
        return (d->fsmodel->filePath(index));
    }

    return currentFolder();
}

void ShowfotoStackViewSideBar::setCurrentPath(const QString& newPathNative)
{
    QFileInfo infoNative(newPathNative);

    if (!infoNative.exists())
    {
        return;
    }

    QString newPath = QDir::fromNativeSeparators(newPathNative);

    if (infoNative.isDir() && !newPath.endsWith(QDir::separator()))
    {
        newPath.append(QDir::separator());
    }

    QString oldDir(d->fsmodel->rootPath());

    if (!oldDir.endsWith(QDir::separator()))
    {
        oldDir.append(QDir::separator());
    }

    if (oldDir == newPath)
    {
        return;
    }

    QFileInfo info(newPath);

    if (info.isDir())
    {
        QModelIndex index = d->fsmodel->index(newPath);

        if (index.isValid())
        {
            d->fsstack->push(new ShowfotoStackViewUndo(this, newPath));
            d->fsmodel->setRootPath(newPath);
            d->fsview->setRootIndex(index);
        }
    }
    else
    {
        QModelIndex index = d->fsmodel->index(newPath);

        if (index.isValid())
        {
            QString newDir = info.absolutePath();

            if (!newDir.endsWith(QDir::separator()))
            {
                newDir.append(QDir::separator());
            }

            if (newDir != oldDir)
            {
                d->fsstack->push(new ShowfotoStackViewUndo(this, newDir));
                d->fsmodel->setRootPath(newDir);
            }

            d->fsview->setCurrentIndex(index);
            d->fsview->scrollTo(index);
        }
    }
}

void ShowfotoStackViewSideBar::setCurrentPathWithoutUndo(const QString& newPath)
{
    QModelIndex index = d->fsmodel->setRootPath(newPath);

    if (index.isValid())
    {
        d->fsview->setRootIndex(index);
        d->fsbar->setCurrentPath(currentFolder());
    }
}

void ShowfotoStackViewSideBar::slotViewModeChanged(int mode)
{
    switch (mode)
    {
        case ShowfotoStackViewList::ShortView:
        {
            d->fsview->setColumnHidden(ShowfotoStackViewList::FileSize, true);
            d->fsview->setColumnHidden(ShowfotoStackViewList::FileType, true);
            d->fsview->setColumnHidden(ShowfotoStackViewList::FileDate, true);
            d->fsview->setHeaderHidden(true);
            break;
        }

        default:    // ShowfotoStackViewList::DetailledView
        {
            d->fsview->setColumnHidden(ShowfotoStackViewList::FileSize, false);
            d->fsview->setColumnHidden(ShowfotoStackViewList::FileType, false);
            d->fsview->setColumnHidden(ShowfotoStackViewList::FileDate, false);
            d->fsview->setHeaderHidden(false);
            break;
        }
    }
}

void ShowfotoStackViewSideBar::setSortOrder(int order)
{
    d->fsSortOrder = (order == ShowfotoItemSortSettings::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder; // Inverted compared to Thumbbar.
    d->fsmodel->sort(d->fsRole, d->fsSortOrder);
}

void ShowfotoStackViewSideBar::setSortRole(int role)
{
    d->fsRole = (ShowfotoStackViewList::FolderViewRole)role;
    d->fsmodel->sort(d->fsRole, d->fsSortOrder);
}

void ShowfotoStackViewSideBar::slotShowBookmarks(bool visible)
{
    d->fsmarks->setVisible(visible);
}

void ShowfotoStackViewSideBar::registerPluginActions(const QList<DPluginAction*>& actions)
{
    d->pluginActions = actions;

    d->fsbar->registerPluginActions(d->pluginActions);

    connect(d->fsbar, SIGNAL(signalPluginActionTriggered(QAction*)),
            this, SLOT(slotPluginActionTriggered(QAction*)));
}

void ShowfotoStackViewSideBar::slotPluginActionTriggered(QAction* act)
{
    foreach (QAction* const dpact, d->pluginActions)
    {
        if (act->objectName() == dpact->objectName())
        {
            slotLoadContents();
            QTimer::singleShot(1000, dpact, SLOT(trigger()));
            return;
        }
    }
}

QList<QAction*> ShowfotoStackViewSideBar::pluginActions() const
{
    return d->fsbar->pluginActions();
}
*/
const QIcon ShowfotoStackViewSideBar::getIcon()
{
    return QIcon::fromTheme(QLatin1String("photos"));
}

const QString ShowfotoStackViewSideBar::getCaption()
{
    return i18nc("@title: file system tree", "Stack");
}

void ShowfotoStackViewSideBar::doLoadState()
{
    KConfigGroup group = getConfigGroup();
/*
    d->fsmarks->readSettings(group);
    d->fsbar->setFolderViewMode(group.readEntry(entryName(d->configFolderViewModeEntry),         (int)ShowfotoStackViewList::ShortView));
    d->fsbar->setFolderViewTypeMime(group.readEntry(entryName(d->configFolderViewTypeMimeEntry), (int)ShowfotoStackViewBar::TYPE_MIME_ALL));
    d->fsbar->setBookmarksVisible(group.readEntry(entryName(d->configBookmarksVisibleEntry),     false));
    slotViewModeChanged(d->fsbar->folderViewMode());
    d->fsbar->setIconSize(group.readEntry(entryName(d->configIconSizeEntry),                     32));

    QByteArray state = group.readEntry(entryName(d->configSplitterStateEntry),                   QByteArray());

    if (!state.isEmpty())
    {
        d->splitter->restoreState(QByteArray::fromBase64(state));
    }

    setCurrentPathWithoutUndo(group.readEntry(entryName(d->configLastFolderEntry),               QDir::rootPath()));
    loadContents(d->fsview->currentIndex());
*/    
}

void ShowfotoStackViewSideBar::doSaveState()
{
    KConfigGroup group = getConfigGroup();
/*
    d->fsmarks->saveSettings(group);
    group.writeEntry(entryName(d->configFolderViewModeEntry),       d->fsbar->folderViewMode());
    group.writeEntry(entryName(d->configFolderViewTypeMimeEntry),   d->fsbar->folderViewTypeMime());
    group.writeEntry(entryName(d->configBookmarksVisibleEntry),     d->fsbar->bookmarksVisible());
    group.writeEntry(entryName(d->configIconSizeEntry),             d->fsbar->iconSize());
    group.writeEntry(entryName(d->configLastFolderEntry),           currentFolder());
    group.writeEntry(entryName(d->configSplitterStateEntry),        d->splitter->saveState().toBase64());
    group.sync();
*/    
}

} // namespace ShowFoto
