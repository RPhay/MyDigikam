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
#include <QUndoStack>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QDir>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "showfotosettings.h"
#include "showfotofolderviewbar.h"
#include "showfotofolderviewundo.h"
#include "showfotofolderviewmodel.h"
#include "showfotofolderviewlist.h"

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
        fsSortOrder (Qt::AscendingOrder),
        fsRole      (ShowfotoFolderViewList::FileName)
    {
    }

    static const QString                   configIconSizeEntry;
    static const QString                   configLastFolderEntry;
    static const QString                   configFolderViewModeEntry;

    ShowfotoFolderViewModel*               fsmodel;
    ShowfotoFolderViewList*                fsview;
    ShowfotoFolderViewBar*                 fsbar;
    QUndoStack*                            fsstack;

    Qt::SortOrder                          fsSortOrder;
    ShowfotoFolderViewList::FolderViewRole fsRole;
};

const QString ShowfotoFolderViewSideBar::Private::configIconSizeEntry(QLatin1String("Icon Size"));
const QString ShowfotoFolderViewSideBar::Private::configLastFolderEntry(QLatin1String("Last Folder"));
const QString ShowfotoFolderViewSideBar::Private::configFolderViewModeEntry(QLatin1String("Folder View Mode"));

ShowfotoFolderViewSideBar::ShowfotoFolderViewSideBar(QWidget* const parent)
    : QWidget          (parent),
      StateSavingObject(this),
      d                (new Private)
{
    setObjectName(QLatin1String("ShowfotoFolderView Sidebar"));

    d->fsstack                 = new QUndoStack(this);

    // --- Populate the view

    const int spacing          = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);
    d->fsbar                   = new ShowfotoFolderViewBar(this);
    d->fsview                  = new ShowfotoFolderViewList(this, d->fsbar);
    d->fsview->setEnableToolTips(true);
    d->fsmodel                 = new ShowfotoFolderViewModel(d->fsview);
    d->fsview->setModel(d->fsmodel);
    d->fsview->setRootIndex(d->fsmodel->index(QDir::rootPath()));

    QVBoxLayout* const layout  = new QVBoxLayout(this);
    layout->addWidget(d->fsbar);
    layout->addWidget(d->fsview);
    layout->setContentsMargins(0, 0, spacing, 0);

    // --- Setup connections

    connect(d->fsbar, SIGNAL(signalSetup()),
            this, SIGNAL(signalSetup()));

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

    connect(d->fsbar, SIGNAL(signalCustomPathChanged(QString)),
            this, SLOT(slotCustomPathChanged(QString)));

    connect(d->fsbar, SIGNAL(signalGoNext()),
            this, SLOT(slotRedo()));

    connect(d->fsbar, SIGNAL(signalGoPrevious()),
            this, SLOT(slotUndo()));

    connect(d->fsstack, SIGNAL(canUndoChanged(bool)),
            d->fsbar, SLOT(slotPreviousEnabled(bool)));

    connect(d->fsstack, SIGNAL(canRedoChanged(bool)),
            d->fsbar, SLOT(slotNextEnabled(bool)));
}

ShowfotoFolderViewSideBar::~ShowfotoFolderViewSideBar()
{
    delete d;
}

void ShowfotoFolderViewSideBar::slotLoadContents()
{
    QModelIndex index = d->fsmodel->index(currentPath());
    loadContents(index);

    if (d->fsmodel->isDir(index))
    {
        // From tool bar, load contents will always open directory in editor (without CTRL|SHIFT keys modifiers)

        emit signalCurrentPathChanged(currentPath());
    }
}

void ShowfotoFolderViewSideBar::loadContents(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }

    if      (d->fsmodel->isDir(index))
    {
        setCurrentPath(d->fsmodel->filePath(index));

        if (QApplication::keyboardModifiers() & (Qt::ShiftModifier | Qt::ControlModifier))
        {
            emit signalCurrentPathChanged(currentPath());
        }
    }
    else if (d->fsmodel->fileInfo(index).isFile())
    {
        emit signalCurrentPathChanged(currentPath());
    }
}

void ShowfotoFolderViewSideBar::slotCustomPathChanged(const QString& path)
{
    setCurrentPath(path);
}

void ShowfotoFolderViewSideBar::slotUndo()
{
    d->fsstack->undo();
}

void ShowfotoFolderViewSideBar::slotRedo()
{
    d->fsstack->redo();
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
    QModelIndex index = d->fsview->currentIndex();

    if (index.isValid())
    {
        return (d->fsmodel->filePath(index));
    }

    return currentFolder();
}

void ShowfotoFolderViewSideBar::setCurrentPath(const QString& newPathNative)
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
            QString newDir = info.absolutePath();

            if (!newDir.endsWith(QDir::separator()))
            {
                newDir.append(QDir::separator());
            }

            if (newDir != oldDir)
            {
                d->fsstack->push(new ShowfotoFolderViewUndo(this, newDir));
                d->fsmodel->setRootPath(newDir);
            }

            d->fsview->setCurrentIndex(index);
            d->fsview->scrollTo(index);
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

    d->fsbar->setFolderViewMode(group.readEntry(entryName(d->configFolderViewModeEntry), (int)ShowfotoFolderViewList::ShortView));
    slotViewModeChanged(d->fsbar->folderViewMode());
    d->fsbar->setIconSize(group.readEntry(entryName(d->configIconSizeEntry), 32));
    setCurrentPathWithoutUndo(group.readEntry(entryName(d->configLastFolderEntry), QDir::rootPath()));
    loadContents(d->fsview->currentIndex());
}

void ShowfotoFolderViewSideBar::doSaveState()
{
    KConfigGroup group = getConfigGroup();

    group.writeEntry(entryName(d->configFolderViewModeEntry), d->fsbar->folderViewMode());
    group.writeEntry(entryName(d->configIconSizeEntry), d->fsbar->iconSize());
    group.writeEntry(entryName(d->configLastFolderEntry), currentFolder());
    group.sync();
}

void ShowfotoFolderViewSideBar::applySettings()
{
    ShowfotoSettings* const settings = ShowfotoSettings::instance();
    Q_UNUSED(settings);
}

void ShowfotoFolderViewSideBar::setActive(bool active)
{
    Q_UNUSED(active);
}

void ShowfotoFolderViewSideBar::slotViewModeChanged(int mode)
{
    switch (mode)
    {
        case ShowfotoFolderViewList::ShortView:
        {
            d->fsview->setColumnHidden(ShowfotoFolderViewList::FileSize, true);
            d->fsview->setColumnHidden(ShowfotoFolderViewList::FileType, true);
            d->fsview->setColumnHidden(ShowfotoFolderViewList::FileDate, true);
            d->fsview->setHeaderHidden(true);
            break;
        }

        default:    // ShowfotoFolderViewList::DetailledView
        {
            d->fsview->setColumnHidden(ShowfotoFolderViewList::FileSize, false);
            d->fsview->setColumnHidden(ShowfotoFolderViewList::FileType, false);
            d->fsview->setColumnHidden(ShowfotoFolderViewList::FileDate, false);
            d->fsview->setHeaderHidden(false);
            break;
        }
    }
}

void ShowfotoFolderViewSideBar::setSortOrder(int order)
{
    d->fsSortOrder = (order == ShowfotoItemSortSettings::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder; // Inverted compared to Thumbbar.
    d->fsmodel->sort(d->fsRole, d->fsSortOrder);
}

void ShowfotoFolderViewSideBar::setSortRole(int role)
{
    d->fsRole = (ShowfotoFolderViewList::FolderViewRole)role;
    d->fsmodel->sort(d->fsRole, d->fsSortOrder);
}

} // namespace ShowFoto
