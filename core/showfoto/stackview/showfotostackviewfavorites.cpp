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
#include <QIcon>
#include <QMessageBox>
#include <QFile>
#include <QDate>
#include <QDomDocument>
#include <QDomElement>
#include <QTextStream>
#include <QTextCodec>

// KDE includes

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
        file = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QLatin1String("/favorites.xml");
    }

public:

    QList<QAction*>                 actionsList;                    ///< used to shared actions with list-view context menu.
    QToolButton*                    addBtn;
    QToolButton*                    delBtn;
    QToolButton*                    edtBtn;
    ShowfotoStackViewFavoriteList*  favoritesList;
    QTreeWidgetItem*                topFavorites;
    ShowfotoStackViewSideBar*       sidebar;
    QString                         file;
};

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

    connect(d->favoritesList, SIGNAL(signalAddFavorite(QList<QUrl>)),
            this, SLOT(slotAddFavorite(QList<QUrl>)));

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
    slotAddFavorite(d->sidebar->urls());
}

void ShowfotoStackViewFavorites::slotAddFavorite(const QList<QUrl>& newUrls)
{
    QString name;
    QString desc;
    QDate date       = QDate::currentDate();;
    QString icon     = QLatin1String("folder-favorites");
    QList<QUrl> urls = newUrls;

    bool ok          = ShowfotoStackViewFavoriteDlg::favoriteCreate(d->favoritesList, name, desc, date, icon, urls);

    if (ok)
    {
        ShowfotoStackViewFavoriteItem* const item = new ShowfotoStackViewFavoriteItem(d->topFavorites);
        item->setName(name);
        item->setDescription(desc);
        item->setDate(date);
        item->setIcon(0, QIcon::fromTheme(icon));
        item->setUrls(urls);
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
    QString desc     = item->description();
    QDate date       = item->date();
    QString icon     = item->icon(0).name();
    QList<QUrl> urls = item->urls();

    bool ok = ShowfotoStackViewFavoriteDlg::favoriteEdit(d->favoritesList, name, desc, date, icon, urls);

    if (ok)
    {
        item->setName(name);
        item->setDescription(desc);
        item->setDate(date);
        item->setIcon(0, QIcon::fromTheme(icon));
        item->setUrls(urls);
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

bool ShowfotoStackViewFavorites::saveSettings()
{
    QDomDocument doc(QLatin1String("favorites"));
    doc.setContent(QLatin1String("<!DOCTYPE XMLFavorites><favorites version=\"1.0\" client=\"showfoto\" encoding=\"UTF-8\"/>"));
    QDomElement docElem = doc.documentElement();

    ShowfotoStackViewFavoriteItem* item = nullptr;
    int nbItems                         = d->topFavorites->childCount();

    QDomElement elemExpand = doc.createElement(QLatin1String("TopExpanded"));
    elemExpand.setAttribute(QLatin1String("value"), d->topFavorites->isExpanded());
    docElem.appendChild(elemExpand);

    QDomElement elemList = doc.createElement(QLatin1String("FavoritesList"));
    docElem.appendChild(elemList);

    for (int i = 0 ; i < nbItems ; ++i)
    {
        item = dynamic_cast<ShowfotoStackViewFavoriteItem*>(d->topFavorites->child(i));

        if (item)
        {
            QDomElement elem = doc.createElement(QLatin1String("Favorite"));

            QDomElement name = doc.createElement(QLatin1String("Name"));
            name.setAttribute(QLatin1String("value"), item->name());
            elem.appendChild(name);

            QDomElement desc = doc.createElement(QLatin1String("Description"));
            desc.setAttribute(QLatin1String("value"), item->description());
            elem.appendChild(desc);

            QDomElement date = doc.createElement(QLatin1String("Date"));
            date.setAttribute(QLatin1String("value"), item->date().toString());
            elem.appendChild(date);

            QDomElement icon = doc.createElement(QLatin1String("Icon"));
            icon.setAttribute(QLatin1String("value"), item->icon(0).name());
            elem.appendChild(icon);

            QDomElement urls = doc.createElement(QLatin1String("UrlsList"));
            elem.appendChild(urls);

            foreach (const QUrl& url, item->urls())
            {
                QDomElement e = doc.createElement(QLatin1String("Url"));
                e.setAttribute(QLatin1String("value"), url.toLocalFile());
                urls.appendChild(e);
            }

            elemList.appendChild(elem);
        }
    }

    QFile file(d->file);

    if (!file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    QTextStream stream(&file);
    stream.setCodec(QTextCodec::codecForName("UTF-8"));
    stream.setAutoDetectUnicode(true);
    stream << doc.toString();
    file.close();

    return true;
}

bool ShowfotoStackViewFavorites::readSettings()
{
    d->favoritesList->clear();

    d->topFavorites          = new QTreeWidgetItem(d->favoritesList);
    d->topFavorites->setFlags(Qt::ItemIsEnabled);
    d->topFavorites->setDisabled(false);
    d->topFavorites->setText(0, i18nc("@title", "My Favorites"));

    QFile file(d->file);

    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QDomDocument doc(QLatin1String("favorites"));

    if (!doc.setContent(&file))
    {
        return false;
    }

    QDomElement docElem = doc.documentElement();

    if (docElem.tagName() != QLatin1String("favorites"))
    {
        return false;
    }

    for (QDomNode n1 = docElem.firstChild() ; !n1.isNull() ; n1 = n1.nextSibling())
    {
        QDomElement e1 = n1.toElement();

        if (e1.isNull())
        {
            continue;
        }

        if (e1.tagName() == QLatin1String("TopExpanded"))
        {
            d->topFavorites->setExpanded((bool)e1.attribute(QLatin1String("value")).toUInt());
            continue;
        }

        if (e1.tagName() == QLatin1String("FavoritesList"))
        {
            for (QDomNode n2 = e1.firstChild() ; !n2.isNull() ; n2 = n2.nextSibling())
            {
                QDomElement e2 = n2.toElement();

                if (e2.tagName() == QLatin1String("Favorite"))
                {
                    ShowfotoStackViewFavoriteItem* const item = new ShowfotoStackViewFavoriteItem(d->topFavorites);

                    for (QDomNode n3 = e2.firstChild() ; !n3.isNull() ; n3 = n3.nextSibling())
                    {
                        QDomElement e3 = n3.toElement();
                        QString name3  = e3.tagName();
                        QString val3   = e3.attribute(QLatin1String("value"));

                        if      (name3 == QLatin1String("Name"))
                        {
                            if (val3.isEmpty())
                            {
                                val3 = i18nc("@title", "Unnamed");
                            }

                            item->setName(val3);
                        }
                        else if (name3 == QLatin1String("Description"))
                        {
                            item->setDescription(val3);
                        }
                        if      (name3 == QLatin1String("Date"))
                        {
                            QDate date = QDate::currentDate();

                            if (!val3.isEmpty())
                            {
                                date = QDate::fromString(val3);
                            }

                            item->setDate(date);
                        }
                        else if (name3 == QLatin1String("Icon"))
                        {
                            if (val3.isEmpty())
                            {
                                val3 = QString::fromLatin1("folder-favorites");
                            }

                            item->setIcon(0, QIcon::fromTheme(val3));
                        }
                        else if (name3 == QLatin1String("UrlsList"))
                        {
                            QList<QUrl> urls;

                            for (QDomNode n4 = e3.firstChild() ; !n4.isNull() ; n4 = n4.nextSibling())
                            {
                                QDomElement e4 = n4.toElement();
                                QString name4  = e4.tagName();
                                QString val4   = e4.attribute(QLatin1String("value"));

                                if (name4 == QLatin1String("Url"))
                                {
                                    if (!val4.isEmpty())
                                    {
                                        urls.append(QUrl::fromLocalFile(val4));
                                    }
                                }
                            }

                            if (urls.isEmpty())
                            {
                                delete item;
                                continue;
                            }
                            else
                            {
                                item->setUrls(urls);
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}

QList<QAction*> ShowfotoStackViewFavorites::pluginActions() const
{
    return d->sidebar->pluginActions();
}

} // namespace ShowFoto
