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
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QApplication>
#include <QStyle>
#include <QUrl>
#include <QListWidget>
#include <QIcon>
#include <QMessageBox>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "thumbnailsize.h"
#include "showfotofolderviewsidebar.h"
#include "showfotofolderviewbookmarkdlg.h"

namespace ShowFoto
{

ShowfotoFolderViewBookmarkItem::ShowfotoFolderViewBookmarkItem(QListWidget* const parent)
    : QListWidgetItem(parent)
{
}

ShowfotoFolderViewBookmarkItem::~ShowfotoFolderViewBookmarkItem()
{
}

void ShowfotoFolderViewBookmarkItem::setPath(const QString& path)
{
    m_path = path;
}

QString ShowfotoFolderViewBookmarkItem::path() const
{
    return m_path;
}

// ---------------------------------------------------------------------

class Q_DECL_HIDDEN ShowfotoFolderViewBookmarks::Private
{
public:

    explicit Private()
      : addBtn       (nullptr),
        delBtn       (nullptr),
        edtBtn       (nullptr),
        bookmarksList(nullptr),
        sidebar      (nullptr)
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

        for (int i = 0 ; i < bookmarksList->count() ; ++i)
        {
            item = dynamic_cast<ShowfotoFolderViewBookmarkItem*>(bookmarksList->item(i));

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

    static const QString       configBookmarkItemsEntry;
    static const QString       configBookmarkPathPrefixEntry;
    static const QString       configBookmarkTitlePrefixEntry;
    static const QString       configBookmarkIconPrefixEntry;

    QPushButton*               addBtn;
    QPushButton*               delBtn;
    QPushButton*               edtBtn;
    QListWidget*               bookmarksList;
    ShowfotoFolderViewSideBar* sidebar;
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
    const int spacing       = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);
    QGridLayout* const grid = new QGridLayout(this);

    QLabel* const title     = new QLabel(this);
    title->setText(i18n("Places"));

    // --------------------------------------------------------

    d->bookmarksList        = new QListWidget(this);
    d->bookmarksList->setWhatsThis(i18n("You can add or remove Album "
                                        "category types here to improve how "
                                        "your Albums are sorted in digiKam."));

    d->bookmarksList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    d->addBtn               = new QPushButton(this);
    d->delBtn               = new QPushButton(this);
    d->edtBtn               = new QPushButton(this);

    d->addBtn->setIcon(QIcon::fromTheme(QLatin1String("list-add")));
    d->delBtn->setIcon(QIcon::fromTheme(QLatin1String("list-remove")));
    d->edtBtn->setIcon(QIcon::fromTheme(QLatin1String("document-edit")));
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
    grid->setSpacing(spacing);

    // --------------------------------------------------------

    connect(d->bookmarksList, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotBookmarkSelectionChanged()));

    connect(d->bookmarksList, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(slotBookmarkDoubleClicked(QListWidgetItem*)));

    connect(d->addBtn, SIGNAL(clicked()),
            this, SLOT(slotAddBookmark()));

    connect(d->delBtn, SIGNAL(clicked()),
            this, SLOT(slotDelBookmark()));

    connect(d->edtBtn, SIGNAL(clicked()),
            this, SLOT(slotEdtBookmark()));
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
                item = new ShowfotoFolderViewBookmarkItem(d->bookmarksList);
                item->setText(title);
                item->setIcon(QIcon::fromTheme(icon));
                item->setPath(path);
                d->bookmarksList->insertItem(d->bookmarksList->count(), item);
                return;
            }
        }
    }

    QMessageBox::information(this,
                             i18n("Add New Bookmark"),
                             i18n("This bookmark referencing\n%1\nalready exists in the list with name \"%2\".",
                             item->path(), item->text()));
}

void ShowfotoFolderViewBookmarks::slotDelBookmark()
{
    QListWidgetItem* const item = d->bookmarksList->currentItem();

    if (!item)
    {
        return;
    }

    d->bookmarksList->takeItem(d->bookmarksList->row(item));
    delete item;
}

void ShowfotoFolderViewBookmarks::slotEdtBookmark()
{
    if (!d->bookmarksList->selectedItems().isEmpty())
    {
        ShowfotoFolderViewBookmarkItem* const item = dynamic_cast<ShowfotoFolderViewBookmarkItem*>(d->bookmarksList->selectedItems().at(0));

        QString title = item->text();
        QString icon  = item->icon().name();
        QString path  = item->path();

        bool ok = ShowfotoFolderViewBookmarkDlg::bookmarkEdit(this, title, icon, path);

        if (ok && !path.isEmpty() && !title.isEmpty())
        {
            ShowfotoFolderViewBookmarkItem* const nitem = d->isBookmarkExist(path);

            if (!nitem)
            {
                item->setText(title);
                item->setIcon(QIcon::fromTheme(icon));
                item->setPath(path);
                return;
            }

            QMessageBox::information(this,
                                     i18n("Edit Bookmark"),
                                     i18n("This bookmark referencing\n%1\nalready exists in the list with name \"%2\".",
                                     nitem->path(), nitem->text()));
        }
    }
}

void ShowfotoFolderViewBookmarks::slotBookmarkSelectionChanged()
{
    bool b = !d->bookmarksList->selectedItems().isEmpty();

    d->delBtn->setEnabled(b);
    d->edtBtn->setEnabled(b);
}

void ShowfotoFolderViewBookmarks::slotBookmarkDoubleClicked(QListWidgetItem* item)
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
    int nbItems                          = d->bookmarksList->count();

    group.writeEntry(d->configBookmarkItemsEntry, nbItems);

    for (int i = 0 ; i < nbItems ; ++i)
    {
        item = dynamic_cast<ShowfotoFolderViewBookmarkItem*>(d->bookmarksList->item(i));

        if (item)
        {
            confEntry = QString::fromLatin1("%1_%2").arg(d->configBookmarkPathPrefixEntry).arg(i);
            group.writeEntry(confEntry, item->path());
            confEntry = QString::fromLatin1("%1_%2").arg(d->configBookmarkTitlePrefixEntry).arg(i);
            group.writeEntry(confEntry, item->text());
            confEntry = QString::fromLatin1("%1_%2").arg(d->configBookmarkIconPrefixEntry).arg(i);
            group.writeEntry(confEntry, item->icon().name());
        }
    }
}

void ShowfotoFolderViewBookmarks::readSettings(const KConfigGroup& group)
{
    d->bookmarksList->clear();

    QString confEntry;
    int nbItems = group.readEntry(d->configBookmarkItemsEntry, 0);

    for (int i = 0 ; i < nbItems ; ++i)
    {
        ShowfotoFolderViewBookmarkItem* const item = new ShowfotoFolderViewBookmarkItem(d->bookmarksList);

        confEntry       = QString::fromLatin1("%1_%2").arg(d->configBookmarkPathPrefixEntry).arg(i);
        item->setPath(group.readEntry(confEntry, QString()));

        if (!item->path().isEmpty())
        {
            confEntry       = QString::fromLatin1("%1_%2").arg(d->configBookmarkTitlePrefixEntry).arg(i);
            item->setText(group.readEntry(confEntry, d->bookmarkBaseName(item->path())));

            confEntry       = QString::fromLatin1("%1_%2").arg(d->configBookmarkIconPrefixEntry).arg(i);
            QString icoName = group.readEntry(confEntry, QString::fromLatin1("folder"));
            item->setIcon(QIcon::fromTheme(icoName));

            d->bookmarksList->addItem(item);
        }
        else
        {
            delete item;
        }
    }
}

} // namespace ShowFoto
