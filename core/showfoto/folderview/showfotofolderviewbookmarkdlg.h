/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : Showfoto folder view bookmark edit dialog
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

#ifndef SHOWFOTO_FOLDER_VIEW_BOOKMARK_DLG_H
#define SHOWFOTO_FOLDER_VIEW_BOOKMARK_DLG_H

// Qt includes

#include <QString>
#include <QDialog>

// Local includes

#include "digikam_config.h"

namespace ShowFoto
{

class ShowfotoFolderViewBookmarks;

class ShowfotoFolderViewBookmarkDlg : public QDialog
{
    Q_OBJECT

public:

    explicit ShowfotoFolderViewBookmarkDlg(ShowfotoFolderViewBookmarks* const parent,
                                           bool create = false);
    ~ShowfotoFolderViewBookmarkDlg() override;

    void setTitle(const QString& title);
    QString title()    const;

    void setIcon(const QString& icon);
    QString icon()     const;

    void setPath(const QString& path);
    QString path()     const;

public:

    static bool bookmarkEdit(ShowfotoFolderViewBookmarks* const parent, QString& title, QString& icon, QString& path);
    static bool bookmarkCreate(ShowfotoFolderViewBookmarks* const parent, QString& title, QString& icon, QString& path);

private Q_SLOTS:

    void slotIconChanged();
    void slotIconResetClicked();

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOWFOTO_FOLDER_VIEW_BOOKMARK_DLG_H
