/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : Showfoto folder view bookmarks item
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SHOWFOTO_FOLDER_VIEW_BOOKMARK_ITEM_H
#define SHOWFOTO_FOLDER_VIEW_BOOKMARK_ITEM_H

// Qt includes

#include <QString>
#include <QTreeWidget>
#include <QTreeWidgetItem>

namespace ShowFoto
{

class ShowfotoFolderViewBookmarkItem : public QTreeWidgetItem
{

public:

    explicit ShowfotoFolderViewBookmarkItem(QTreeWidgetItem* const parent);
    ~ShowfotoFolderViewBookmarkItem()    override;

    void setPath(const QString&);
    QString path()      const;

private:

    QString m_path;

    Q_DISABLE_COPY(ShowfotoFolderViewBookmarkItem)
};

} // namespace ShowFoto

#endif // SHOWFOTO_FOLDER_VIEW_BOOKMARK_ITEM_H
