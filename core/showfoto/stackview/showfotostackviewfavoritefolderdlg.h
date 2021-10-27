/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-09-27
 * Description : Showfoto stack view favorite folder edit dialog
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

#ifndef SHOWFOTO_STACK_VIEW_FAVORITE_FOLDER_DLG_H
#define SHOWFOTO_STACK_VIEW_FAVORITE_FOLDER_DLG_H

// Qt includes

#include <QString>
#include <QDialog>

// Local includes

#include "digikam_config.h"

namespace ShowFoto
{

class ShowfotoStackViewFavoriteList;
class ShowfotoStackViewFavoriteItem;

class ShowfotoStackViewFavoriteFolderDlg : public QDialog
{
    Q_OBJECT

public:

    explicit ShowfotoStackViewFavoriteFolderDlg(ShowfotoStackViewFavoriteList* const list,
                                              bool create = false);
    ~ShowfotoStackViewFavoriteFolderDlg() override;

    void setName(const QString& name);
    QString name()        const;

    void setParentItem(ShowfotoStackViewFavoriteItem* const pitem);

public:

    static bool favoriteFolderDialog(ShowfotoStackViewFavoriteList* const list,
                                     QString& name,
                                     ShowfotoStackViewFavoriteItem* const pitem,
                                     bool create = false);

private Q_SLOTS:

    void slotModified();

private:

    bool canAccept() const;

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOWFOTO_STACK_VIEW_FAVORITE_FOLDER_DLG_H
