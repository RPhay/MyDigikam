/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-09-27
 * Description : Showfoto stack view favorites item
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

#ifndef SHOWFOTO_STACK_VIEW_FAVORITE_ITEM_H
#define SHOWFOTO_STACK_VIEW_FAVORITE_ITEM_H

// Qt includes

#include <QString>
#include <QStringList>
#include <QUrl>
#include <QDate>
#include <QList>
#include <QTreeWidget>
#include <QTreeWidgetItem>

namespace ShowFoto
{

class ShowfotoStackViewFavoriteItem : public QTreeWidgetItem
{

public:

    enum FavoriteType
    {
        FavoriteRoot = -1,      ///< Favorite is root item from hierarchy.
        FavoriteFolder,         ///< Favorite is a simple folder in hierarchy.
        FavoriteItem            ///< Favorite is a hierarchy item including all properties.
    };

public:

    explicit ShowfotoStackViewFavoriteItem(QTreeWidget* const parent);
    ShowfotoStackViewFavoriteItem(QTreeWidgetItem* const parent, int favType);
    ~ShowfotoStackViewFavoriteItem() override;

    void setName(const QString& name);
    QString name()            const;

    void setHierarchy(const QString& desc);
    QString hierarchy()       const;

    void setFavoriteType(int favoriteType);
    int favoriteType()        const;

    void setDescription(const QString& desc);
    QString description()     const;

    void setDate(const QDate& date);
    QDate date()              const;

    void setUrls(const QList<QUrl>&);
    QList<QUrl> urls()        const;

    void setCurrentUrl(const QUrl& url);
    QUrl currentUrl()         const;

    /**
     * Helper method to get a list local paths from image urls included in favorite item.
     */
    QStringList urlsToPaths() const;

public:

    /**
     * Helper static method to get hierarchy path from item. 'name' is the title and 'pitem" the parent instance.
     */
    static QString hierarchyFromParent(const QString& name, ShowfotoStackViewFavoriteItem* const pitem);

private:

    void updateToolTip();
    Q_DISABLE_COPY(ShowfotoStackViewFavoriteItem)

private:

    int         m_favoriteType;
    QString     m_hierarchy;
    QList<QUrl> m_urls;
    QString     m_desc;
    QDate       m_date;
    QUrl        m_current;
};

} // namespace ShowFoto

#endif // SHOWFOTO_STACK_VIEW_FAVORITE_ITEM_H
