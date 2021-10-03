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

#include "showfotostackviewfavorites.h"

// Qt includes

#include <QDir>
#include <QStandardPaths>
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>
#include <QApplication>
#include <QUrl>
#include <QIcon>
#include <QMessageBox>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "thumbnailsize.h"
#include "showfotostackviewsidebar.h"
#include "showfotostackviewfavoritedlg.h"
#include "showfotostackviewfavoritelist.h"
#include "showfotostackviewfavoriteitem.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoStackViewFavorites::Private
{
public:

    explicit Private()
      : addBtn        (nullptr),
        delBtn        (nullptr),
        edtBtn        (nullptr),
        favoritesList (nullptr),
        topFavorites  (nullptr),
        sidebar       (nullptr)
    {
    }

public:

    static const QString            configFavoriteItemsEntry;
    static const QString            configFavoriteUrlsPrefixEntry;
    static const QString            configFavoriteNamePrefixEntry;
    static const QString            configFavoriteIconPrefixEntry;
    static const QString            configFavoriteTopItemExpandedEntry;

    QList<QAction*>                 actionsList;                    ///< used to shared actions with list-view context menu.
    QToolButton*                    addBtn;
    QToolButton*                    delBtn;
    QToolButton*                    edtBtn;
    ShowfotoStackViewFavoriteList*  favoritesList;
    QTreeWidgetItem*                topFavorites;
    ShowfotoStackViewSideBar*       sidebar;
};

const QString ShowfotoStackViewFavorites::Private::configFavoriteItemsEntry(QLatin1String("FavoriteItems"));
const QString ShowfotoStackViewFavorites::Private::configFavoriteUrlsPrefixEntry(QLatin1String("FavoriteUrls"));
const QString ShowfotoStackViewFavorites::Private::configFavoriteNamePrefixEntry(QLatin1String("FavoriteName"));
const QString ShowfotoStackViewFavorites::Private::configFavoriteIconPrefixEntry(QLatin1String("FavoriteIcon"));
const QString ShowfotoStackViewFavorites::Private::configFavoriteTopItemExpandedEntry(QLatin1String("FavoriteTopItemExpanded"));

ShowfotoStackViewFavorites::ShowfotoStackViewFavorites(ShowfotoStackViewSideBar* const sidebar)
    : QWidget(sidebar),
      d      (new Private)
{
    d->sidebar              = sidebar;
    QGridLayout* const grid = new QGridLayout(this);

    QLabel* const title     = new QLabel(this);
    title->setText(i18nc("@title", "Favorites"));

    // --------------------------------------------------------

    QAction* btnAction      = nullptr;

    btnAction               = new QAction(this);
    btnAction->setObjectName(QLatin1String("AddFavorite"));
    btnAction->setIcon(QIcon::fromTheme(QLatin1String("list-add")));
    btnAction->setText(i18nc("@action", "Add Favorite"));
    btnAction->setToolTip(i18nc("@info", "Add new bookmark to the list"));

    connect(btnAction, SIGNAL(triggered(bool)),
            this, SLOT(slotAddFavorite()));

    d->actionsList << btnAction;

    d->addBtn               = new QToolButton(this);
    d->addBtn->setDefaultAction(btnAction);
    d->addBtn->setFocusPolicy(Qt::NoFocus);
    d->addBtn->setEnabled(false);

    // ---

    btnAction               = new QAction(this);
    btnAction->setObjectName(QLatin1String("DelFavorite"));
    btnAction->setIcon(QIcon::fromTheme(QLatin1String("list-remove")));
    btnAction->setText(i18nc("@action", "Del Favorite"));
    btnAction->setToolTip(i18nc("@info", "Remove selected bookmark from the list"));

    connect(btnAction, SIGNAL(triggered(bool)),
            this, SLOT(slotDelFavorite()));

    d->actionsList << btnAction;

    d->delBtn               = new QToolButton(this);
    d->delBtn->setDefaultAction(btnAction);
    d->delBtn->setFocusPolicy(Qt::NoFocus);
    d->delBtn->setEnabled(false);

    // ---

    btnAction               = new QAction(this);
    btnAction->setObjectName(QLatin1String("EditFavorite"));
    btnAction->setIcon(QIcon::fromTheme(QLatin1String("document-edit")));
    btnAction->setText(i18nc("@action", "Edit Favorite"));
    btnAction->setToolTip(i18nc("@info", "Edit current bookmark from the list"));

    connect(btnAction, SIGNAL(triggered(bool)),
            this, SLOT(slotEdtFavorite()));

    d->actionsList << btnAction;

    d->edtBtn               = new QToolButton(this);
    d->edtBtn->setDefaultAction(btnAction);
    d->edtBtn->setFocusPolicy(Qt::NoFocus);
    d->edtBtn->setEnabled(false);

    // ---

    d->favoritesList        = new ShowfotoStackViewFavoriteList(this);

    grid->setAlignment(Qt::AlignTop);
    grid->addWidget(title,             0, 0, 1, 1);
    grid->addWidget(d->addBtn,         0, 2, 1, 1);
    grid->addWidget(d->delBtn,         0, 3, 1, 1);
    grid->addWidget(d->edtBtn,         0, 4, 1, 1);
    grid->addWidget(d->favoritesList,  1, 0, 1, 5);
    grid->setRowStretch(1, 10);
    grid->setColumnStretch(1, 10);
    grid->setContentsMargins(0, 0, 0, 0);

    // --------------------------------------------------------

    connect(d->favoritesList, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotFavoriteSelectionChanged()));

    connect(d->favoritesList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(slotFavoriteDoubleClicked(QTreeWidgetItem*)));

    connect(d->sidebar, SIGNAL(signalAddFavorite()),
            this, SLOT(slotAddFavorite()));

    connect(d->favoritesList, SIGNAL(signalAddFavorite()),
            this, SLOT(slotAddFavorite()));

    connect(d->favoritesList, SIGNAL(signalLoadContentsFromFiles(QStringList)),
            this, SIGNAL(signalLoadContentsFromFiles(QStringList)));
}

ShowfotoStackViewFavorites::~ShowfotoStackViewFavorites()
{
    delete d;
}

QAction* ShowfotoStackViewFavorites::toolBarAction(const QString& name) const
{
    foreach (QAction* const act, d->actionsList)
    {
        if (act && (act->objectName() == name))
        {
            return act;
        }
    }

    return nullptr;
}

QTreeWidgetItem* ShowfotoStackViewFavorites::topFavoritesItem() const
{
    return d->topFavorites;
}

void ShowfotoStackViewFavorites::loadContents()
{
    d->favoritesList->slotLoadContents();
}

void ShowfotoStackViewFavorites::slotAddFavorite()
{
    QString name     = i18n("New Favorite");
    QString icon     = QLatin1String("folder-favorites");
    QList<QUrl> urls = d->sidebar->urls();

    bool ok = ShowfotoStackViewFavoriteDlg::favoriteCreate(this, name, icon, urls);

    if (ok && !urls.isEmpty() && !name.isEmpty())
    {
        ShowfotoStackViewFavoriteItem* item = d->favoritesList->favoriteExists(name);

        if (!item)
        {
            item = new ShowfotoStackViewFavoriteItem(d->topFavorites);
            item->setName(name);
            item->setIcon(0, QIcon::fromTheme(icon));
            item->setUrls(urls);
            return;
        }

        QMessageBox::information(this,
                                 i18nc("@title: window", "Add New Favorite"),
                                 i18nc("@info", "The favorite named\n%1\nalready exists in the list.",
                                 item->name()));
    }
}

void ShowfotoStackViewFavorites::slotDelFavorite()
{
    QTreeWidgetItem* const item = d->favoritesList->currentItem();

    if (!item || (item->parent() != d->topFavorites))
    {
        return;
    }

    if (QMessageBox::question(this, i18nc("@title: window", "Confirm Favorite Deletion"),
                              i18nc("@info", "Are you sure you want to remove the favorite \"%1\"?", item->text(0)))
            != QMessageBox::Yes)
    {
        return;
    }

    d->favoritesList->removeItemWidget(item, 0);
    delete item;
}

void ShowfotoStackViewFavorites::slotEdtFavorite()
{
    ShowfotoStackViewFavoriteItem* const item = dynamic_cast<ShowfotoStackViewFavoriteItem*>(d->favoritesList->currentItem());

    if (!item || (item->parent() != d->topFavorites))
    {
        return;
    }

    QString name     = item->name();
    QString icon     = item->icon(0).name();
    QList<QUrl> urls = item->urls();

    bool ok = ShowfotoStackViewFavoriteDlg::favoriteEdit(this, name, icon, urls);

    if (ok && !urls.isEmpty() && !name.isEmpty())
    {
        ShowfotoStackViewFavoriteItem* const nitem = d->favoritesList->favoriteExists(name);

        if (!nitem)
        {
            item->setName(name);
            item->setIcon(0, QIcon::fromTheme(icon));
            item->setUrls(urls);
            return;
        }

        QMessageBox::information(this,
                                 i18nc("@title: window", "Edit Favorite"),
                                 i18nc("@info", "The favorite named\n%1\nalready exists in the list.",
                                 nitem->name()));
    }
}

void ShowfotoStackViewFavorites::slotFavoriteSelectionChanged()
{
    bool b                                    = true;
    ShowfotoStackViewFavoriteItem* const item = dynamic_cast<ShowfotoStackViewFavoriteItem*>(d->favoritesList->currentItem());

    if (!item || (item->parent() != d->topFavorites))
    {
        b = false;
    }

    d->delBtn->setEnabled(b);
    d->edtBtn->setEnabled(b);
}

void ShowfotoStackViewFavorites::slotFavoriteDoubleClicked(QTreeWidgetItem* item)
{
    ShowfotoStackViewFavoriteItem* const fitem = dynamic_cast<ShowfotoStackViewFavoriteItem*>(item);

    if (fitem)
    {
        emit signalLoadContentsFromFiles(fitem->urlsToPaths());
    }
}

void ShowfotoStackViewFavorites::slotItemListChanged(int nbitems)
{
    d->addBtn->setEnabled(nbitems > 0);
}

void ShowfotoStackViewFavorites::saveSettings(KConfigGroup& group)
{
    QString confEntry;
    ShowfotoStackViewFavoriteItem* item = nullptr;
    int nbItems                         = d->topFavorites->childCount();

    group.writeEntry(d->configFavoriteItemsEntry,            nbItems);
    group.writeEntry(d->configFavoriteTopItemExpandedEntry,  d->topFavorites->isExpanded());

    for (int i = 0 ; i < nbItems ; ++i)
    {
        item = dynamic_cast<ShowfotoStackViewFavoriteItem*>(d->topFavorites->child(i));

        if (item)
        {
            confEntry = QString::fromLatin1("%1_%2").arg(d->configFavoriteUrlsPrefixEntry).arg(i);
            group.writeEntry(confEntry, item->urls());
            confEntry = QString::fromLatin1("%1_%2").arg(d->configFavoriteNamePrefixEntry).arg(i);
            group.writeEntry(confEntry, item->name());
            confEntry = QString::fromLatin1("%1_%2").arg(d->configFavoriteIconPrefixEntry).arg(i);
            group.writeEntry(confEntry, item->icon(0).name());
        }
    }
}

void ShowfotoStackViewFavorites::readSettings(const KConfigGroup& group)
{
    d->favoritesList->clear();

    d->topFavorites          = new QTreeWidgetItem(d->favoritesList);
    d->topFavorites->setFlags(Qt::ItemIsEnabled);
    d->topFavorites->setExpanded(group.readEntry(d->configFavoriteTopItemExpandedEntry, true));
    d->topFavorites->setDisabled(false);
    d->topFavorites->setText(0, i18nc("@title", "My Favorites"));

    QString confEntry;
    int nbItems = group.readEntry(d->configFavoriteItemsEntry, 0);

    for (int i = 0 ; i < nbItems ; ++i)
    {
        ShowfotoStackViewFavoriteItem* const item = new ShowfotoStackViewFavoriteItem(d->topFavorites);

        confEntry       = QString::fromLatin1("%1_%2").arg(d->configFavoriteUrlsPrefixEntry).arg(i);
        item->setUrls(group.readEntry(confEntry, QList<QUrl>()));

        if (!item->urls().isEmpty())
        {
            confEntry       = QString::fromLatin1("%1_%2").arg(d->configFavoriteNamePrefixEntry).arg(i);
            item->setName(group.readEntry(confEntry, i18n("Unamed")));

            confEntry       = QString::fromLatin1("%1_%2").arg(d->configFavoriteIconPrefixEntry).arg(i);
            QString icoName = group.readEntry(confEntry, QString::fromLatin1("folder-favorites"));
            item->setIcon(0, QIcon::fromTheme(icoName));
        }
        else
        {
            delete item;
        }
    }
}

QList<QAction*> ShowfotoStackViewFavorites::pluginActions() const
{
    return d->sidebar->pluginActions();
}

} // namespace ShowFoto
