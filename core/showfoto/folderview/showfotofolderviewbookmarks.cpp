/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : Showfoto folder view bookmark places
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

#include "showfotofolderviewbookmarks.h"

// Qt includes

#include <QDir>
#include <QStandardPaths>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QApplication>
#include <QUrl>
#include <QIcon>
#include <QMessageBox>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "thumbnailsize.h"
#include "showfotofolderviewsidebar.h"
#include "showfotofolderviewbookmarkdlg.h"
#include "showfotofolderviewbookmarklist.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoFolderViewBookmarks::Private
{
public:

    explicit Private()
      : addBtn        (nullptr),
        delBtn        (nullptr),
        edtBtn        (nullptr),
        bookmarksList (nullptr),
        topBookmarks  (nullptr),
        topUsualPlaces(nullptr),
        sidebar       (nullptr)
    {
    }

    QString bookmarkBaseName(const QString& path) const
    {
        if (path.endsWith(QDir::separator()))
        {
            return (path.section(QDir::separator(), -2).remove(QDir::separator()));
        }
        else
        {
            return (QFileInfo(path).baseName());
        }
    }

    ShowfotoFolderViewBookmarkItem* isBookmarkExist(const QString& path) const
    {
        bool found                           = false;
        ShowfotoFolderViewBookmarkItem* item = nullptr;

        for (int i = 0 ; i < topBookmarks->childCount() ; ++i)
        {
            item = dynamic_cast<ShowfotoFolderViewBookmarkItem*>(topBookmarks->child(i));

            if (path == item->path())
            {
                found = true;
                break;
            }
        }

        if (found)
        {
            return item;
        }

        return nullptr;
    }

public:

    static const QString            configBookmarkItemsEntry;
    static const QString            configBookmarkPathPrefixEntry;
    static const QString            configBookmarkTitlePrefixEntry;
    static const QString            configBookmarkIconPrefixEntry;

    QPushButton*                    addBtn;
    QPushButton*                    delBtn;
    QPushButton*                    edtBtn;
    ShowfotoFolderViewBookmarkList* bookmarksList;
    QTreeWidgetItem*                topBookmarks;
    QTreeWidgetItem*                topUsualPlaces;
    ShowfotoFolderViewSideBar*      sidebar;
};

const QString ShowfotoFolderViewBookmarks::Private::configBookmarkItemsEntry(QLatin1String("BookmarkItems"));
const QString ShowfotoFolderViewBookmarks::Private::configBookmarkPathPrefixEntry(QLatin1String("BookmarkPath"));
const QString ShowfotoFolderViewBookmarks::Private::configBookmarkTitlePrefixEntry(QLatin1String("BookmarkTitle"));
const QString ShowfotoFolderViewBookmarks::Private::configBookmarkIconPrefixEntry(QLatin1String("BookmarkIcon"));

ShowfotoFolderViewBookmarks::ShowfotoFolderViewBookmarks(ShowfotoFolderViewSideBar* const sidebar)
    : QWidget(sidebar),
      d      (new Private)
{
    d->sidebar              = sidebar;
    QGridLayout* const grid = new QGridLayout(this);

    QLabel* const title     = new QLabel(this);
    title->setText(i18nc("@title", "Places"));

    // --------------------------------------------------------

    d->bookmarksList        = new ShowfotoFolderViewBookmarkList(this);
    d->addBtn               = new QPushButton(this);
    d->delBtn               = new QPushButton(this);
    d->edtBtn               = new QPushButton(this);

    d->addBtn->setIcon(QIcon::fromTheme(QLatin1String("list-add")));
    d->addBtn->setToolTip(i18nc("@info", "Add new bookmark to the list"));
    d->delBtn->setIcon(QIcon::fromTheme(QLatin1String("list-remove")));
    d->delBtn->setToolTip(i18nc("@info", "Remove selected bookmark from the list"));
    d->edtBtn->setIcon(QIcon::fromTheme(QLatin1String("document-edit")));
    d->edtBtn->setToolTip(i18nc("@info", "Edit current bookmark from the list"));
    d->delBtn->setEnabled(false);
    d->edtBtn->setEnabled(false);

    grid->setAlignment(Qt::AlignTop);
    grid->addWidget(title,             0, 0, 1, 1);
    grid->addWidget(d->addBtn,         0, 2, 1, 1);
    grid->addWidget(d->delBtn,         0, 3, 1, 1);
    grid->addWidget(d->edtBtn,         0, 4, 1, 1);
    grid->addWidget(d->bookmarksList,  1, 0, 1, 5);
    grid->setRowStretch(1, 10);
    grid->setColumnStretch(1, 10);
    grid->setContentsMargins(0, 0, 0, 0);

    // --------------------------------------------------------

    connect(d->bookmarksList, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotBookmarkSelectionChanged()));

    connect(d->bookmarksList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(slotBookmarkDoubleClicked(QTreeWidgetItem*)));

    connect(d->addBtn, SIGNAL(clicked()),
            this, SLOT(slotAddBookmark()));

    connect(d->delBtn, SIGNAL(clicked()),
            this, SLOT(slotDelBookmark()));

    connect(d->edtBtn, SIGNAL(clicked()),
            this, SLOT(slotEdtBookmark()));

    connect(d->sidebar, SIGNAL(signalAddBookmark()),
            this, SLOT(slotAddBookmark()));
}

ShowfotoFolderViewBookmarks::~ShowfotoFolderViewBookmarks()
{
    delete d;
}

void ShowfotoFolderViewBookmarks::slotAddBookmark()
{
    QString newBookmark = d->sidebar->currentFolder();

    if (newBookmark.isEmpty())
    {
        return;
    }

    ShowfotoFolderViewBookmarkItem* item = d->isBookmarkExist(newBookmark);

    if (!item)
    {
        QString title = d->bookmarkBaseName(newBookmark);
        QString icon  = QLatin1String("folder");
        QString path  = newBookmark;

        bool ok = ShowfotoFolderViewBookmarkDlg::bookmarkCreate(this, title, icon, path);

        if (ok && !path.isEmpty() && !title.isEmpty())
        {
            item = d->isBookmarkExist(path);

            if (!item)
            {
                item = new ShowfotoFolderViewBookmarkItem(d->topBookmarks);
                item->setText(0, title);
                item->setIcon(0, QIcon::fromTheme(icon));
                item->setPath(path);
                return;
            }
        }
    }

    QMessageBox::information(this,
                             i18nc("@title: window", "Add New Bookmark"),
                             i18nc("@info", "This bookmark referencing\n%1\nalready exists in the list with name \"%2\".",
                             item->path(), item->text(0)));
}

void ShowfotoFolderViewBookmarks::slotDelBookmark()
{
    QTreeWidgetItem* const item = d->bookmarksList->currentItem();

    if (!item || (item->parent() != d->topBookmarks))
    {
        return;
    }

    if (QMessageBox::question(this, i18nc("@title: window", "Confirm Bookmark Deletion"),
                              i18nc("@info", "Are you sure you want to remove the bookmark \"%1\"?", item->text(0)))
            != QMessageBox::Yes)
    {
        return;
    }

    d->bookmarksList->removeItemWidget(item, 0);
    delete item;
}

void ShowfotoFolderViewBookmarks::slotEdtBookmark()
{
    if (!d->bookmarksList->selectedItems().isEmpty())
    {
        ShowfotoFolderViewBookmarkItem* const item = dynamic_cast<ShowfotoFolderViewBookmarkItem*>(d->bookmarksList->selectedItems().at(0));

        if (!item || (item->parent() != d->topBookmarks))
        {
            return;
        }

        QString title = item->text(0);
        QString icon  = item->icon(0).name();
        QString path  = item->path();

        bool ok = ShowfotoFolderViewBookmarkDlg::bookmarkEdit(this, title, icon, path);

        if (ok && !path.isEmpty() && !title.isEmpty())
        {
            ShowfotoFolderViewBookmarkItem* const nitem = d->isBookmarkExist(path);

            if (!nitem)
            {
                item->setText(0, title);
                item->setIcon(0, QIcon::fromTheme(icon));
                item->setPath(path);
                return;
            }

            QMessageBox::information(this,
                                     i18nc("@title: window", "Edit Bookmark"),
                                     i18nc("@info", "This bookmark referencing\n%1\nalready exists in the list with name \"%2\".",
                                     nitem->path(), nitem->text(0)));
        }
    }
}

void ShowfotoFolderViewBookmarks::slotBookmarkSelectionChanged()
{
    bool b                                     = true;
    ShowfotoFolderViewBookmarkItem* const item = dynamic_cast<ShowfotoFolderViewBookmarkItem*>(d->bookmarksList->selectedItems().at(0));

    if (!item || (item->parent() != d->topBookmarks))
    {
        b = false;
    }

    d->delBtn->setEnabled(b);
    d->edtBtn->setEnabled(b);
}

void ShowfotoFolderViewBookmarks::slotBookmarkDoubleClicked(QTreeWidgetItem* item)
{
    ShowfotoFolderViewBookmarkItem* const bitem = dynamic_cast<ShowfotoFolderViewBookmarkItem*>(item);

    if (bitem)
    {
        d->sidebar->setCurrentPath(bitem->path());
    }
}

void ShowfotoFolderViewBookmarks::saveSettings(KConfigGroup& group)
{
    QString confEntry;
    ShowfotoFolderViewBookmarkItem* item = nullptr;
    int nbItems                          = d->topBookmarks->childCount();

    group.writeEntry(d->configBookmarkItemsEntry, nbItems);

    for (int i = 0 ; i < nbItems ; ++i)
    {
        item = dynamic_cast<ShowfotoFolderViewBookmarkItem*>(d->topBookmarks->child(i));

        if (item)
        {
            confEntry = QString::fromLatin1("%1_%2").arg(d->configBookmarkPathPrefixEntry).arg(i);
            group.writeEntry(confEntry, item->path());
            confEntry = QString::fromLatin1("%1_%2").arg(d->configBookmarkTitlePrefixEntry).arg(i);
            group.writeEntry(confEntry, item->text(0));
            confEntry = QString::fromLatin1("%1_%2").arg(d->configBookmarkIconPrefixEntry).arg(i);
            group.writeEntry(confEntry, item->icon(0).name());
        }
    }
}

void ShowfotoFolderViewBookmarks::readSettings(const KConfigGroup& group)
{
    d->bookmarksList->clear();

    d->topBookmarks          = new QTreeWidgetItem(d->bookmarksList);
    d->topBookmarks->setFlags(Qt::ItemIsEnabled);
    d->topBookmarks->setExpanded(true);
    d->topBookmarks->setDisabled(false);
    d->topBookmarks->setText(0, i18nc("@title", "Bookmarks"));

    QString confEntry;
    int nbItems = group.readEntry(d->configBookmarkItemsEntry, 0);

    for (int i = 0 ; i < nbItems ; ++i)
    {
        ShowfotoFolderViewBookmarkItem* const item = new ShowfotoFolderViewBookmarkItem(d->topBookmarks);

        confEntry       = QString::fromLatin1("%1_%2").arg(d->configBookmarkPathPrefixEntry).arg(i);
        item->setPath(group.readEntry(confEntry, QString()));

        if (!item->path().isEmpty())
        {
            confEntry       = QString::fromLatin1("%1_%2").arg(d->configBookmarkTitlePrefixEntry).arg(i);
            item->setText(0, group.readEntry(confEntry, d->bookmarkBaseName(item->path())));

            confEntry       = QString::fromLatin1("%1_%2").arg(d->configBookmarkIconPrefixEntry).arg(i);
            QString icoName = group.readEntry(confEntry, QString::fromLatin1("folder"));
            item->setIcon(0, QIcon::fromTheme(icoName));
        }
        else
        {
            delete item;
        }
    }

    d->topUsualPlaces                      = new QTreeWidgetItem(d->bookmarksList);
    d->topUsualPlaces->setFlags(Qt::ItemIsEnabled);
    d->topUsualPlaces->setExpanded(true);
    d->topUsualPlaces->setDisabled(false);
    d->topUsualPlaces->setText(0, i18nc("@title", "Usual"));

    ShowfotoFolderViewBookmarkItem* uplace = nullptr;
    uplace                                 = new ShowfotoFolderViewBookmarkItem(d->topUsualPlaces);
    uplace->setText(0, i18nc("@item", "Home"));
    uplace->setIcon(0, QIcon::fromTheme(QLatin1String("user-home")));
    uplace->setPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));

    uplace                                 = new ShowfotoFolderViewBookmarkItem(d->topUsualPlaces);
    uplace->setText(0, i18nc("@item", "Pictures"));
    uplace->setIcon(0, QIcon::fromTheme(QLatin1String("folder-image")));
    uplace->setPath(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));

    uplace                                 = new ShowfotoFolderViewBookmarkItem(d->topUsualPlaces);
    uplace->setText(0, i18nc("@item", "Documents"));
    uplace->setIcon(0, QIcon::fromTheme(QLatin1String("folder-document")));
    uplace->setPath(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));

    uplace                                 = new ShowfotoFolderViewBookmarkItem(d->topUsualPlaces);
    uplace->setText(0, i18nc("@item", "Downloads"));
    uplace->setIcon(0, QIcon::fromTheme(QLatin1String("folder-downloads")));
    uplace->setPath(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));

    uplace                                 = new ShowfotoFolderViewBookmarkItem(d->topUsualPlaces);
    uplace->setText(0, i18nc("@item", "Desktop"));
    uplace->setIcon(0, QIcon::fromTheme(QLatin1String("user-desktop")));
    uplace->setPath(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));

    uplace                                 = new ShowfotoFolderViewBookmarkItem(d->topUsualPlaces);
    uplace->setText(0, i18nc("@item", "Music"));
    uplace->setIcon(0, QIcon::fromTheme(QLatin1String("folder-music")));
    uplace->setPath(QStandardPaths::writableLocation(QStandardPaths::MusicLocation));

    uplace                                 = new ShowfotoFolderViewBookmarkItem(d->topUsualPlaces);
    uplace->setText(0, i18nc("@item", "Videos"));
    uplace->setIcon(0, QIcon::fromTheme(QLatin1String("folder-videos")));
    uplace->setPath(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation));
}

} // namespace ShowFoto
