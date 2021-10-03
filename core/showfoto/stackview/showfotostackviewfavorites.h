/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-09-27
 * Description : Showfoto stack view favorites
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

#ifndef SHOWFOTO_STACK_VIEW_FAVORITES_H
#define SHOWFOTO_STACK_VIEW_FAVORITES_H

// Qt includes

#include <QList>
#include <QAction>
#include <QWidget>
#include <QString>
#include <QTreeWidgetItem>

class KConfigGroup;

namespace ShowFoto
{

class ShowfotoStackViewSideBar;

class ShowfotoStackViewFavorites : public QWidget
{
    Q_OBJECT

public:

    explicit ShowfotoStackViewFavorites(ShowfotoStackViewSideBar* const sidebar);
    ~ShowfotoStackViewFavorites()                     override;

    void saveSettings(KConfigGroup&);
    void readSettings(const KConfigGroup&);

    QTreeWidgetItem* topFavoritesItem()         const;

    QAction* toolBarAction(const QString& name) const;
    QList<QAction*> pluginActions()             const;

Q_SIGNALS:

    void signalLoadContents();

private Q_SLOTS:

    void slotLoadContents(const QString& path);
    void slotFavoriteDoubleClicked(QTreeWidgetItem*);
    void slotFavoriteSelectionChanged();
    void slotAddFavorite();
    void slotDelFavorite();
    void slotEdtFavorite();

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOWFOTO_STACK_VIEW_FAVORITES_H
