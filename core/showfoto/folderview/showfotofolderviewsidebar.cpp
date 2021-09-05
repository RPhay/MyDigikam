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
#include <QFileInfo>
#include <QDir>
#include <QUndoStack>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "showfotosettings.h"
#include "showfotofolderviewbar.h"
#include "showfotofolderviewundo.h"
#include "showfotofolderviewlist.h"
#include "showfotofolderviewmodel.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoFolderViewSideBar::Private
{

public:

    explicit Private()
      : fsmodel     (nullptr),
        fsview      (nullptr),
        fsbar       (nullptr),
        fsstack     (nullptr)
    {
    }

    static const QString     configLastPathEntry;

    ShowfotoFolderViewModel* fsmodel;
    QListView*               fsview;
    ShowfotoFolderViewBar*   fsbar;
    QUndoStack*              fsstack;
};

const QString ShowfotoFolderViewSideBar::Private::configLastPathEntry(QLatin1String("Last Path"));

ShowfotoFolderViewSideBar::ShowfotoFolderViewSideBar(QWidget* const parent)
    : QWidget          (parent),
      StateSavingObject(this),
      d                (new Private)
{
    setObjectName(QLatin1String("ShowfotoFolderView Sidebar"));

    // --- Populate the model

    d->fsmodel                 = new ShowfotoFolderViewModel(this);
    d->fsstack                 = new QUndoStack(this);

    // --- Populate the view

    const int spacing          = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);
    d->fsbar                   = new ShowfotoFolderViewBar(this);
    d->fsview                  = new ShowfotoFolderViewList(this);
    d->fsview->setModel(d->fsmodel);
    d->fsview->setRootIndex(d->fsmodel->index(QDir::rootPath()));

    QVBoxLayout* const layout  = new QVBoxLayout(this);
    layout->addWidget(d->fsbar);
    layout->addWidget(d->fsview);
    layout->setContentsMargins(0, 0, spacing, 0);

    // --- Setup connections

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

    emit signalCurrentPathChanged(currentPath());
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
    loadContents(d->fsview->currentIndex());
}

void ShowfotoFolderViewSideBar::doSaveState()
{
    KConfigGroup group = getConfigGroup();

    group.writeEntry(entryName(d->configLastPathEntry), currentPath());
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

} // namespace ShowFoto
