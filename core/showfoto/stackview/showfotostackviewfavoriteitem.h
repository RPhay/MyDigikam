/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-09-27
 * Description : Showfoto stack view favorites item
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

class ShowfotoStackViewFavoriteBase : public QTreeWidgetItem
{

public:

    enum FavoriteType
    {
        FavoriteRoot = QTreeWidgetItem::UserType,
        FavoriteFolder,
        FavoriteItem
    };

public:

    ShowfotoStackViewFavoriteBase(QTreeWidget* const parent, int type);
    ShowfotoStackViewFavoriteBase(QTreeWidgetItem* const parent, int type);
    ~ShowfotoStackViewFavoriteBase() override;

    void setName(const QString& name);
    QString name()            const;

    void setHierarchy(const QString& desc);
    QString hierarchy()       const;

    static QString hierarchyFromParent(const QString& name, ShowfotoStackViewFavoriteBase* const pitem);

protected:

    virtual void updateToolTip() {};

private:

    QString     m_hierarchy;

    Q_DISABLE_COPY(ShowfotoStackViewFavoriteBase)
};

// ------------------------------------------------------------------------------------

class ShowfotoStackViewFavoriteRoot : public ShowfotoStackViewFavoriteBase
{

public:

    explicit ShowfotoStackViewFavoriteRoot(QTreeWidget* const parent);
    ~ShowfotoStackViewFavoriteRoot() override;

private:

    Q_DISABLE_COPY(ShowfotoStackViewFavoriteRoot)
};

// ------------------------------------------------------------------------------------

class ShowfotoStackViewFavoriteFolder : public ShowfotoStackViewFavoriteBase
{

public:

    explicit ShowfotoStackViewFavoriteFolder(QTreeWidgetItem* const parent, int type = FavoriteFolder);
    ~ShowfotoStackViewFavoriteFolder() override;

private:

    Q_DISABLE_COPY(ShowfotoStackViewFavoriteFolder)
};

// ------------------------------------------------------------------------------------

class ShowfotoStackViewFavoriteItem : public ShowfotoStackViewFavoriteFolder
{

public:

    explicit ShowfotoStackViewFavoriteItem(QTreeWidgetItem* const parent);
    ~ShowfotoStackViewFavoriteItem() override;

    void setDescription(const QString& desc);
    QString description()     const;

    void setDate(const QDate& date);
    QDate date()              const;

    void setUrls(const QList<QUrl>&);
    QList<QUrl> urls()        const;

    void setCurrentUrl(const QUrl& url);
    QUrl currentUrl()         const;

    QStringList urlsToPaths() const;

private:

    void updateToolTip()            override;

private:

    QList<QUrl> m_urls;
    QString     m_desc;
    QDate       m_date;
    QUrl        m_current;

    Q_DISABLE_COPY(ShowfotoStackViewFavoriteItem)
};

} // namespace ShowFoto

#endif // SHOWFOTO_STACK_VIEW_FAVORITE_ITEM_H
