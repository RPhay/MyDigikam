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

#ifndef SHOWFOTO_FOLDER_VIEW_BOOKMARKS_H
#define SHOWFOTO_FOLDER_VIEW_BOOKMARKS_H

// Qt includes

#include <QWidget>
#include <QIcon>
#include <QString>
#include <QListWidgetItem>

class KConfigGroup;

namespace ShowFoto
{

class ShowfotoFolderViewSideBar;

class ShowfotoFolderViewBookmarkItem : public QListWidgetItem
{

public:

    explicit ShowfotoFolderViewBookmarkItem(QListWidget* const parent = nullptr);
    ~ShowfotoFolderViewBookmarkItem()    override;

    void setPath(const QString&);
    QString path()      const;

private:

    QString m_path;

    Q_DISABLE_COPY(ShowfotoFolderViewBookmarkItem)
};

// -------------------------------------------------------------------


class ShowfotoFolderViewBookmarks : public QWidget
{
    Q_OBJECT

public:

    explicit ShowfotoFolderViewBookmarks(ShowfotoFolderViewSideBar* const sidebar);
    ~ShowfotoFolderViewBookmarks() override;

    void saveSettings(KConfigGroup&);
    void readSettings(const KConfigGroup&);

private Q_SLOTS:

    void slotBookmarkDoubleClicked(QListWidgetItem*);
    void slotBookmarkSelectionChanged();
    void slotAddBookmark();
    void slotDelBookmark();
    void slotEdtBookmark();

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOWFOTO_FOLDER_VIEW_BOOKMARKS_H
