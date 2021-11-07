/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-09-27
 * Description : Showfoto stack view favorite item edit dialog
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

#ifndef SHOWFOTO_STACK_VIEW_FAVORITE_ITEM_DLG_H
#define SHOWFOTO_STACK_VIEW_FAVORITE_ITEM_DLG_H

// Qt includes

#include <QString>
#include <QDate>
#include <QDialog>
#include <QList>
#include <QUrl>

// Local includes

#include "digikam_config.h"
#include "ditemslist.h"

using namespace Digikam;

namespace ShowFoto
{

class ShowfotoStackViewFavoriteList;
class ShowfotoStackViewFavoriteItem;

class ShowfotoStackViewFavoriteItemDlg : public QDialog
{
    Q_OBJECT

public:

    explicit ShowfotoStackViewFavoriteItemDlg(ShowfotoStackViewFavoriteList* const list,
                                              bool create = false);
    ~ShowfotoStackViewFavoriteItemDlg() override;

    void setName(const QString& name);
    QString name()        const;

    void setDescription(const QString& desc);
    QString description() const;

    void setDate(const QDate& name);
    QDate date()          const;

    void setIcon(const QString& icon);
    QString icon()        const;

    void setUrls(const QList<QUrl>& urls);
    QList<QUrl> urls()    const;

    void setCurrentUrl(const QUrl& url);
    QUrl currentUrl()     const;

    void setIconSize(int size);
    void setSortOrder(int order);
    void setSortRole(int role);
    void setParentItem(ShowfotoStackViewFavoriteItem* const pitem);

public:

    static bool favoriteItemDialog(ShowfotoStackViewFavoriteList* const list,
                                   QString& name,
                                   QString& desc,
                                   QDate& date,
                                   QString& icon,
                                   QList<QUrl>& urls,
                                   QUrl& current,
                                   int iconSize,
                                   int sortOrder,
                                   int sortRole,
                                   ShowfotoStackViewFavoriteItem* const pitem,
                                   bool create = false);

private:

    static bool s_itemIsLessThanHandler(const QTreeWidgetItem* current, const QTreeWidgetItem& other);

private Q_SLOTS:

    void slotIconChanged();
    void slotIconResetClicked();
    void slotModified();
    void slotUpdateMetadata();

private:

    bool canAccept()              const;

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOWFOTO_STACK_VIEW_FAVORITE_ITEM_DLG_H
