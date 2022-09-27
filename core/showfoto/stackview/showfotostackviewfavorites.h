/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-09-27
 * Description : Showfoto stack view favorites
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef SHOWFOTO_STACK_VIEW_FAVORITES_H
#define SHOWFOTO_STACK_VIEW_FAVORITES_H

// Qt includes

#include <QList>
#include <QUrl>
#include <QAction>
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QTreeWidgetItem>

// Local includes

#include "searchtextbar.h"

using namespace Digikam;

namespace ShowFoto
{

class ShowfotoStackViewSideBar;

class ShowfotoStackViewFavorites : public QWidget
{
    Q_OBJECT

public:

    explicit ShowfotoStackViewFavorites(ShowfotoStackViewSideBar* const sidebar);
    ~ShowfotoStackViewFavorites()                     override;

    bool saveSettings();
    bool readSettings();

    QTreeWidgetItem* topFavoritesItem()         const;

    QAction* toolBarAction(const QString& name) const;
    QList<QAction*> pluginActions()             const;

    void loadContents();

public Q_SLOTS:

    void slotItemListChanged(int nbitems);

Q_SIGNALS:

    void signalLoadContents();
    void signalLoadContentsFromFiles(const QStringList& files, const QString& current);

private Q_SLOTS:

    void slotFavoriteDoubleClicked(QTreeWidgetItem*);
    void slotItemSelectionChanged();
    void slotAddFavorite(const QList<QUrl>& newUrls, const QUrl& current);
    void slotAddFavorite();
    void slotAddSubFolder();
    void slotDelItem();
    void slotEditItem();
    void slotSearchTextChanged(const SearchTextSettings& settings);
    void slotSearchResult(int found);

private:

    ShowfotoStackViewFavorites(QWidget*);

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOWFOTO_STACK_VIEW_FAVORITES_H
