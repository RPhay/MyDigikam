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
#include <QHeaderView>
#include <QDir>
#include <QUndoStack>
#include <QScrollBar>

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

    class UndoCommand : public QUndoCommand
    {
    public:

        UndoCommand(ShowfotoFolderView* const view, const QString& new_path)
            : view_(view)
        {
            old_state_.path       = view_->currentFolder();
            old_state_.scroll_pos = view_->listView()->verticalScrollBar()->value();
            old_state_.index      = view_->listView()->currentIndex();
            new_state_.path       = new_path;
        }

        QString undo_path() const
        {
            return old_state_.path;
        }

        void undo()
        {
            new_state_.scroll_pos = view_->listView()->verticalScrollBar()->value();
            new_state_.index      = view_->listView()->currentIndex();
            view_->setCurrentPathWithoutUndo(old_state_.path);
            view_->listView()->setCurrentIndex(old_state_.index);
            view_->listView()->verticalScrollBar()->setValue(old_state_.scroll_pos);
        }

        void redo()
        {
            view_->setCurrentPathWithoutUndo(new_state_.path);

            if (new_state_.scroll_pos != -1)
            {
                view_->listView()->setCurrentIndex(new_state_.index);
                view_->listView()->verticalScrollBar()->setValue(new_state_.scroll_pos);
            }
        }

    private:

        struct State
        {
            State()
                : scroll_pos(-1)
            {
            }

            QString     path;
            QModelIndex index;
            int         scroll_pos;
        };

        ShowfotoFolderView* view_;
        State               old_state_;
        State               new_state_;
    };

public:

    explicit Private()
      : fsmodel     (nullptr),
        fsview      (nullptr),
        fsbar       (nullptr),
        fsstack     (nullptr)
    {
    }

    static const QString   configLastPathEntry;

    QFileSystemModel*      fsmodel;
    QListView*             fsview;
    ShowfotoFolderViewBar* fsbar;
    QUndoStack*            fsstack;
};

const QString ShowfotoFolderView::Private::configLastPathEntry(QLatin1String("Last Path"));

ShowfotoFolderView::ShowfotoFolderView(QWidget* const parent)
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
    d->fsview->setObjectName(QLatin1String("ShowfotoFolderView"));
    d->fsview->setModel(d->fsmodel);
    d->fsview->setRootIndex(d->fsmodel->index(QDir::rootPath()));
    d->fsview->setAlternatingRowColors(true);

    QVBoxLayout* const layout  = new QVBoxLayout(this);
    layout->addWidget(d->fsbar);
    layout->addWidget(d->fsview);
    layout->setContentsMargins(0, 0, spacing, 0);

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

ShowfotoFolderView::~ShowfotoFolderView()
{
    delete d;
}

QListView* ShowfotoFolderView::listView() const
{
    return d->fsview;
}

void ShowfotoFolderView::slotItemActivated(const QModelIndex& index)
{
/*
    if (d->fsmodel->isDir(index))
    {
        setCurrentPath(d->fsmodel->filePath(index));
    }
*/
}

void ShowfotoFolderView::slotItemDoubleClicked(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }

    QString cpath = currentPath();

    if (d->fsmodel->isDir(index))
    {
        setCurrentPath(d->fsmodel->filePath(index));
    }

    if (QApplication::keyboardModifiers() & (Qt::ShiftModifier | Qt::ControlModifier))
    {
        emit signalCurrentPathChanged(cpath);
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

void ShowfotoFolderView::slotGoUp()
{
    QDir dir(currentFolder());
    dir.cdUp();

    // Is this the same as going back?  If so just go back, so we can keep the view scroll position.

    if (d->fsstack->canUndo())
    {
        const Private::UndoCommand* lastDir = static_cast<const Private::UndoCommand*>(d->fsstack->command(d->fsstack->index() - 1));

        if (lastDir->undo_path() == dir.path())
        {
            d->fsstack->undo();
            return;
        }
    }

    setCurrentPath(dir.absolutePath());
}

QString ShowfotoFolderView::currentFolder() const
{
    return d->fsmodel->rootPath();
}

QString ShowfotoFolderView::currentPath() const
{
    return d->fsmodel->filePath(d->fsview->currentIndex());
}

/*
void ShowfotoFolderView::setCurrentPath(const QString& path, bool withUndo)
{
    QString oldPath = d->fsmodel->rootPath();
    QString newPath = QDir::fromNativeSeparators(path);

    QFileInfo info(newPath);

    if (!info.exists())
    {
        return;
    }

    if (info.isDir())
    {
        QModelIndex index = d->fsmodel->setRootPath(newPath);

        if (index.isValid())
        {
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

    d->fsbar->setCurrentPath(currentFolder());

    if (withUndo && (oldPath != newPath))
    {
        d->fsstack->push(new Private::UndoCommand(this, newPath));
    }
}
*/
void ShowfotoFolderView::setCurrentPath(const QString& new_path_native)
{
    QString new_path = QDir::fromNativeSeparators(new_path_native);
    QString old_path(d->fsmodel->rootPath());

    if (old_path == new_path)
    {
        return;
    }

    QFileInfo info(new_path);

    if (!info.exists())
    {
        return;
    }

    if (info.isDir())
    {
        QModelIndex index = d->fsmodel->setRootPath(new_path);

        if (index.isValid())
        {
            d->fsview->setRootIndex(index);
            d->fsstack->push(new Private::UndoCommand(this, new_path));
qCDebug(DIGIKAM_SHOWFOTO_LOG) << new_path << new_path_native << old_path;
        }
    }
    else
    {
        QModelIndex index = d->fsmodel->index(new_path);

        if (index.isValid())
        {
            d->fsview->setCurrentIndex(index);
        }
    }
}

void ShowfotoFolderView::setCurrentPathWithoutUndo(const QString& new_path)
{
    QModelIndex index = d->fsmodel->setRootPath(new_path);

    if (index.isValid())
    {
        d->fsview->setRootIndex(index);
        d->fsbar->setCurrentPath(currentFolder());
qCDebug(DIGIKAM_SHOWFOTO_LOG) << new_path;
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

    setCurrentPath(group.readEntry(entryName(d->configLastPathEntry), QDir::rootPath()));
    slotItemDoubleClicked(d->fsview->currentIndex());
}

void ShowfotoFolderView::doSaveState()
{
    KConfigGroup group = getConfigGroup();

    group.writeEntry(entryName(d->configLastPathEntry), currentPath());
    group.sync();
}

void ShowfotoFolderView::applySettings()
{
/*
    ShowfotoSettings* const settings = ShowfotoSettings::instance();
    d->albumFolderView->setEnableToolTips(settings->getShowAlbumToolTips());
    d->albumFolderView->setExpandNewCurrentItem(settings->getExpandNewCurrentItem());
*/
}

void ShowfotoFolderView::setActive(bool active)
{
    if (active)
    {
//        AlbumManager::instance()->setCurrentAlbums(QList<Album*>() << d->albumFolderView->currentAlbum());
    }
}

} // namespace ShowFoto
