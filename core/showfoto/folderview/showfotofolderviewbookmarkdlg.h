/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : Showfoto folder view bookmark edit dialog
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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

class ShowfotoFolderViewBookmarkList;

class ShowfotoFolderViewBookmarkDlg : public QDialog
{
    Q_OBJECT

public:

    explicit ShowfotoFolderViewBookmarkDlg(ShowfotoFolderViewBookmarkList* const parent,
                                           bool create = false);
    ~ShowfotoFolderViewBookmarkDlg() override;

    void setTitle(const QString& title);
    QString title()    const;

    void setIcon(const QString& icon);
    QString icon()     const;

    void setPath(const QString& path);
    QString path()     const;

public:

    static bool bookmarkDialog(ShowfotoFolderViewBookmarkList* const parent,
                               QString& title,
                               QString& icon,
                               QString& path,
                               bool create = false);

private Q_SLOTS:

    void slotIconChanged();
    void slotIconResetClicked();
    void slotAccept();
    void slotModified();
    void slotHelp();

private:

    bool canAccept() const;

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOWFOTO_FOLDER_VIEW_BOOKMARK_DLG_H
