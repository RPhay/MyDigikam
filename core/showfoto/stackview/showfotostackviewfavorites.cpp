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
#include <QInputDialog>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "thumbnailsize.h"
#include "showfotostackviewsidebar.h"
#include "showfotostackviewfavoriteitemdlg.h"
#include "showfotostackviewfavoritelist.h"
#include "showfotostackviewfavoriteitem.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoStackViewFavorites::Private
{
public:

    explicit Private()
      : addBtn        (nullptr),
        fldBtn        (nullptr),
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
    QToolButton*                    addBtn;                         ///< Add favorite button.
    QToolButton*                    fldBtn;                         ///< Add sub-folder button.
    QToolButton*                    delBtn;
    QToolButton*                    edtBtn;
    ShowfotoStackViewFavoriteList*  favoritesList;
    ShowfotoStackViewFavoriteRoot*  topFavorites;
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
    d->addBtn->setEnabled(true);

    // ---

    btnAction               = new QAction(this);
    btnAction->setObjectName(QLatin1String("AddFolder"));
    btnAction->setIcon(QIcon::fromTheme(QLatin1String("folder-new")));
    btnAction->setText(i18nc("@action", "Add Sub-Folder"));
    btnAction->setToolTip(i18nc("@info", "Add new sub-folder to the list"));

    connect(btnAction, SIGNAL(triggered(bool)),
            this, SLOT(slotAddSubFolder()));

    d->actionsList << btnAction;

    d->fldBtn               = new QToolButton(this);
    d->fldBtn->setDefaultAction(btnAction);
    d->fldBtn->setFocusPolicy(Qt::NoFocus);
    d->fldBtn->setEnabled(true);

    // ---

    btnAction               = new QAction(this);
    btnAction->setObjectName(QLatin1String("DelItem"));
    btnAction->setIcon(QIcon::fromTheme(QLatin1String("list-remove")));
    btnAction->setText(i18nc("@action", "Delete Item"));
    btnAction->setToolTip(i18nc("@info", "Remove selected item from the list"));

    connect(btnAction, SIGNAL(triggered(bool)),
            this, SLOT(slotDelItem()));

    d->actionsList << btnAction;

    d->delBtn               = new QToolButton(this);
    d->delBtn->setDefaultAction(btnAction);
    d->delBtn->setFocusPolicy(Qt::NoFocus);
    d->delBtn->setEnabled(false);

    // ---

    btnAction               = new QAction(this);
    btnAction->setObjectName(QLatin1String("EditItem"));
    btnAction->setIcon(QIcon::fromTheme(QLatin1String("document-edit")));
    btnAction->setText(i18nc("@action", "Edit Item"));
    btnAction->setToolTip(i18nc("@info", "Edit current item from the list"));

    connect(btnAction, SIGNAL(triggered(bool)),
            this, SLOT(slotEditItem()));

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
    grid->addWidget(d->fldBtn,         0, 3, 1, 1);
    grid->addWidget(d->delBtn,         0, 4, 1, 1);
    grid->addWidget(d->edtBtn,         0, 5, 1, 1);
    grid->addWidget(d->favoritesList,  1, 0, 1, 6);
    grid->setRowStretch(1, 10);
    grid->setColumnStretch(1, 10);
    grid->setContentsMargins(0, 0, 0, 0);

    // --------------------------------------------------------

    connect(d->favoritesList, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotItemSelectionChanged()));

    connect(d->favoritesList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(slotFavoriteDoubleClicked(QTreeWidgetItem*)));

    connect(d->sidebar, SIGNAL(signalAddFavorite()),
            this, SLOT(slotAddFavorite()));

    connect(d->favoritesList, SIGNAL(signalAddFavorite()),
            this, SLOT(slotAddFavorite()));

    connect(d->favoritesList, SIGNAL(signalAddFavorite(QList<QUrl>,QUrl)),
            this, SLOT(slotAddFavorite(QList<QUrl>,QUrl)));

    connect(d->favoritesList, SIGNAL(signalLoadContentsFromFiles(QStringList,QString)),
            this, SIGNAL(signalLoadContentsFromFiles(QStringList,QString)));
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
    slotAddFavorite(d->sidebar->urls(), d->sidebar->currentUrl());
}

void ShowfotoStackViewFavorites::slotAddSubFolder()
{
    bool ok                                     = false;
    ShowfotoStackViewFavoriteBase* const parent = d->favoritesList->currentItem() ? dynamic_cast<ShowfotoStackViewFavoriteBase*>(d->favoritesList->currentItem())
                                                                                  : d->topFavorites;

    QString name = QInputDialog::getText(this,
                                         i18nc("@title", "New Sub-Folder"),
                                         i18nc("@label", "Sub-Folder Name:"),
                                         QLineEdit::Normal,
                                         QString(),
                                         &ok);

    if (
        !ok            ||
        name.isEmpty() ||
        !d->favoritesList->findFavoriteByHierarchy(ShowfotoStackViewFavoriteBase::hierarchyFromParent(name, parent))
       )
    {
        return;
    }

    ShowfotoStackViewFavoriteFolder* const folder = new ShowfotoStackViewFavoriteFolder(parent);
    folder->setName(name);
    parent->setExpanded(true);
}

void ShowfotoStackViewFavorites::slotAddFavorite(const QList<QUrl>& newUrls, const QUrl& current)
{
    QString name;
    QString desc;
    QDate date                                  = QDate::currentDate();
    QString icon                                = QLatin1String("folder-favorites");
    QList<QUrl> urls                            = newUrls;
    QUrl currentUrl                             = current;
    ShowfotoStackViewFavoriteBase* const parent = d->favoritesList->currentItem() ? dynamic_cast<ShowfotoStackViewFavoriteBase*>(d->favoritesList->currentItem())
                                                                                  : d->topFavorites;

    bool ok          = ShowfotoStackViewFavoriteItemDlg::favoriteDialog(d->favoritesList,
                                                                    name,
                                                                    desc,
                                                                    date,
                                                                    icon,
                                                                    urls,
                                                                    currentUrl,
                                                                    d->sidebar->iconSize(),
                                                                    d->sidebar->sortOrder(),
                                                                    d->sidebar->sortRole(),
                                                                    parent,
                                                                    true
                                                                   );

    if (ok)
    {
        ShowfotoStackViewFavoriteItem* const item = new ShowfotoStackViewFavoriteItem(parent);
        item->setName(name);
        item->setDescription(desc);
        item->setDate(date);
        item->setIcon(0, QIcon::fromTheme(icon));
        item->setUrls(urls);
        item->setCurrentUrl(currentUrl);
    }
}

void ShowfotoStackViewFavorites::slotDelItem()
{
    ShowfotoStackViewFavoriteFolder* const fitem = dynamic_cast<ShowfotoStackViewFavoriteFolder*>(d->favoritesList->currentItem());

    if (!fitem)
    {
        return;
    }

    if (QMessageBox::question(this, i18nc("@title: window", "Confirm Item Deletion"),
                              i18nc("@info", "Are you sure you want to remove the item \"%1\"\n"
                                             "and all nested items if any?", fitem->name()))
            != QMessageBox::Yes)
    {
        return;
    }

    d->favoritesList->removeItemWidget(fitem, 0);
    delete fitem;
}

void ShowfotoStackViewFavorites::slotEditItem()
{
    ShowfotoStackViewFavoriteItem* const item = dynamic_cast<ShowfotoStackViewFavoriteItem*>(d->favoritesList->currentItem());

    if (item)
    {
        QString name                                = item->name();
        QString desc                                = item->description();
        QDate date                                  = item->date();
        QString icon                                = item->icon(0).name();
        QList<QUrl> urls                            = item->urls();
        QUrl currentUrl                             = item->currentUrl();
        ShowfotoStackViewFavoriteBase* const parent = dynamic_cast<ShowfotoStackViewFavoriteBase*>(item->parent());

        bool ok = ShowfotoStackViewFavoriteItemDlg::favoriteDialog(d->favoritesList,
                                                               name,
                                                               desc,
                                                               date,
                                                               icon,
                                                               urls,
                                                               currentUrl,
                                                               d->sidebar->iconSize(),
                                                               d->sidebar->sortOrder(),
                                                               d->sidebar->sortRole(),
                                                               parent
                                                              );

        if (ok)
        {
            item->setName(name);
            item->setDescription(desc);
            item->setDate(date);
            item->setIcon(0, QIcon::fromTheme(icon));
            item->setUrls(urls);
            item->setCurrentUrl(currentUrl);
        }

        return;
    }

    ShowfotoStackViewFavoriteFolder* const fitem = dynamic_cast<ShowfotoStackViewFavoriteFolder*>(d->favoritesList->currentItem());

    if (fitem)
    {
        bool ok                                     = false;
        ShowfotoStackViewFavoriteBase* const parent = dynamic_cast<ShowfotoStackViewFavoriteBase*>(fitem->parent());

        QString name = QInputDialog::getText(this,
                                             i18nc("@title", "Edit Sub-Folder"),
                                             i18nc("@label", "Sub-Folder Name:"),
                                             QLineEdit::Normal,
                                             fitem->name(),
                                             &ok);

        if (
            !ok                     ||
            name.isEmpty()          ||
            (name == fitem->name()) ||
            !d->favoritesList->findFavoriteByHierarchy(ShowfotoStackViewFavoriteBase::hierarchyFromParent(name, parent))
           )
        {
            return;
        }

        fitem->setName(name);
    }
}

void ShowfotoStackViewFavorites::slotItemSelectionChanged()
{
    bool b                      = false;
    QTreeWidgetItem* const item = d->favoritesList->currentItem();

    if (item)
    {
        ShowfotoStackViewFavoriteBase* const fitem = dynamic_cast<ShowfotoStackViewFavoriteBase*>(item);

        if (fitem->type() != ShowfotoStackViewFavoriteBase::FavoriteRoot)
        {
            b = true;
        }
    }

    d->delBtn->setEnabled(b);
    d->edtBtn->setEnabled(b);
}

void ShowfotoStackViewFavorites::slotFavoriteDoubleClicked(QTreeWidgetItem* item)
{
    ShowfotoStackViewFavoriteItem* const fvitem = dynamic_cast<ShowfotoStackViewFavoriteItem*>(item);

    if (fvitem)
    {
        emit signalLoadContentsFromFiles(fvitem->urlsToPaths(), fvitem->currentUrl().toLocalFile());
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
    QDomElement docElem    = doc.documentElement();

    // ---

    QDomElement elemExpand = doc.createElement(QLatin1String("TopExpanded"));
    elemExpand.setAttribute(QLatin1String("value"), d->topFavorites->isExpanded());
    docElem.appendChild(elemExpand);

    // ---

    QDomElement foldList   = doc.createElement(QLatin1String("FoldersList"));
    docElem.appendChild(foldList);
    QTreeWidgetItemIterator it(d->favoritesList);

    while (*it)
    {
        ShowfotoStackViewFavoriteBase* const item = dynamic_cast<ShowfotoStackViewFavoriteBase*>(*it);

        if (item && (item->type() != ShowfotoStackViewFavoriteFolder::FavoriteRoot))
        {
            QDomElement elem = doc.createElement(QLatin1String("Folder"));

            QDomElement name = doc.createElement(QLatin1String("Name"));
            name.setAttribute(QLatin1String("value"), item->name());
            elem.appendChild(name);

            QDomElement hier = doc.createElement(QLatin1String("Hierarchy"));
            hier.setAttribute(QLatin1String("value"), item->hierarchy());
            elem.appendChild(hier);

            QDomElement fexp = doc.createElement(QLatin1String("IsExpanded"));
            fexp.setAttribute(QLatin1String("value"), item->isExpanded());
            elem.appendChild(fexp);

            foldList.appendChild(elem);
        }

        ++it;
    }

    // ---

    QDomElement elemList = doc.createElement(QLatin1String("FavoritesList"));
    docElem.appendChild(elemList);
    QTreeWidgetItemIterator it2(d->favoritesList);

    while (*it2)
    {
        ShowfotoStackViewFavoriteItem* const item = dynamic_cast<ShowfotoStackViewFavoriteItem*>(*it2);

        if (item)
        {
            QDomElement elem = doc.createElement(QLatin1String("Favorite"));

            QDomElement name = doc.createElement(QLatin1String("Name"));
            name.setAttribute(QLatin1String("value"), item->name());
            elem.appendChild(name);

            QDomElement desc = doc.createElement(QLatin1String("Description"));
            desc.setAttribute(QLatin1String("value"), item->description());
            elem.appendChild(desc);

            QDomElement hier = doc.createElement(QLatin1String("Hierarchy"));
            hier.setAttribute(QLatin1String("value"), item->hierarchy());
            elem.appendChild(hier);

            QDomElement date = doc.createElement(QLatin1String("Date"));
            date.setAttribute(QLatin1String("value"), item->date().toString());
            elem.appendChild(date);

            QDomElement icon = doc.createElement(QLatin1String("Icon"));
            icon.setAttribute(QLatin1String("value"), item->icon(0).name());
            elem.appendChild(icon);

            QDomElement iexp = doc.createElement(QLatin1String("IsExpanded"));
            iexp.setAttribute(QLatin1String("value"), item->isExpanded());
            elem.appendChild(iexp);

            QDomElement urls = doc.createElement(QLatin1String("UrlsList"));
            elem.appendChild(urls);

            foreach (const QUrl& url, item->urls())
            {
                QDomElement e = doc.createElement(QLatin1String("Url"));
                e.setAttribute(QLatin1String("value"), url.toLocalFile());
                urls.appendChild(e);
            }

            QDomElement curr = doc.createElement(QLatin1String("CurrentUrl"));
            curr.setAttribute(QLatin1String("value"), item->currentUrl().toLocalFile());
            elem.appendChild(curr);

            elemList.appendChild(elem);
        }

        ++it2;
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

    d->topFavorites          = new ShowfotoStackViewFavoriteRoot(d->favoritesList);

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

        // ---

        if (e1.tagName() == QLatin1String("FoldersList"))
        {
            for (QDomNode n2 = e1.firstChild() ; !n2.isNull() ; n2 = n2.nextSibling())
            {
                QDomElement e2 = n2.toElement();

                if (e2.tagName() == QLatin1String("Folder"))
                {
                    bool isExpanded = false;
                    QString name;
                    QString hierarchy;

                    for (QDomNode n3 = e2.firstChild() ; !n3.isNull() ; n3 = n3.nextSibling())
                    {
                        QDomElement e3  = n3.toElement();
                        QString name3   = e3.tagName();
                        QString val3    = e3.attribute(QLatin1String("value"));

                        if      (name3 == QLatin1String("Name"))
                        {
                            name = val3;
                        }
                        else if (name3 == QLatin1String("Hierarchy"))
                        {
                            hierarchy = val3;
                        }
                        else if (name3 == QLatin1String("IsExpanded"))
                        {
                            isExpanded = val3.toUInt();
                        }
                    }

                    QString phierarchy      = hierarchy.section(QLatin1Char('/'), 0, -3) + QLatin1String("/");
                    QTreeWidgetItem* parent = d->favoritesList->findFavoriteByHierarchy(phierarchy);

                    if (!parent)
                    {
                        continue;
                    }

                    ShowfotoStackViewFavoriteFolder* const folder = new ShowfotoStackViewFavoriteFolder(parent);
                    folder->setName(name);
                    folder->setHierarchy(hierarchy);
                    folder->setExpanded(isExpanded);
                }
            }

            continue;
        }

        // ---

        if (e1.tagName() == QLatin1String("FavoritesList"))
        {
            int unamedID = 1;

            for (QDomNode n2 = e1.firstChild() ; !n2.isNull() ; n2 = n2.nextSibling())
            {
                QDomElement e2 = n2.toElement();

                if (e2.tagName() == QLatin1String("Favorite"))
                {
                    bool isExpanded = true;
                    QDate date      = QDate::currentDate();
                    QString icon    = QString::fromLatin1("folder-favorites");
                    QString name;
                    QString desc;
                    QString hierarchy;
                    QList<QUrl> urls;
                    QString curr;

                    for (QDomNode n3 = e2.firstChild() ; !n3.isNull() ; n3 = n3.nextSibling())
                    {
                        QDomElement e3  = n3.toElement();
                        QString name3   = e3.tagName();
                        QString val3    = e3.attribute(QLatin1String("value"));

                        if      (name3 == QLatin1String("Name"))
                        {
                            if (val3.isEmpty())
                            {
                                val3 = i18nc("@title", "Unnamed") + QString::fromLatin1("_%1").arg(unamedID);
                                unamedID++;
                            }

                            name = val3;
                        }
                        else if (name3 == QLatin1String("Description"))
                        {
                            desc = val3;
                        }
                        else if (name3 == QLatin1String("Hierarchy"))
                        {
                            hierarchy = val3;
                        }
                        else if (name3 == QLatin1String("Date"))
                        {
                            if (!val3.isEmpty())
                            {
                                date = QDate::fromString(val3);
                            }
                        }
                        else if (name3 == QLatin1String("Icon"))
                        {
                            if (!val3.isEmpty())
                            {
                                icon = val3;
                            }
                        }
                        else if (name3 == QLatin1String("IsExpanded"))
                        {
                            if (!val3.isEmpty())
                            {
                                isExpanded = val3.toUInt();
                            }
                        }
                        else if (name3 == QLatin1String("UrlsList"))
                        {
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
                        }
                        else if (name3 == QLatin1String("CurrentUrl"))
                        {
                            curr = val3;
                        }
                    }

                    QTreeWidgetItem* fitem                    = d->favoritesList->findFavoriteByHierarchy(hierarchy);

                    if (!fitem)
                    {
                        continue;
                    }

                    ShowfotoStackViewFavoriteItem* const item = new ShowfotoStackViewFavoriteItem(d->topFavorites);
                    d->favoritesList->replaceItem(fitem, item);

                    item->setName(name);
                    item->setDescription(desc);
                    item->setHierarchy(hierarchy);
                    item->setDate(date);
                    item->setIcon(0, QIcon::fromTheme(icon));
                    item->setExpanded(isExpanded);

                    if (urls.isEmpty())
                    {
                        delete item;
                        continue;
                    }
                    else
                    {
                        item->setUrls(urls);
                    }

                    if (curr.isEmpty())
                    {
                        if (!item->urls().isEmpty())
                        {
                            curr = item->urls().first().toLocalFile();
                        }
                        else
                        {
                            curr = QString();
                        }
                    }

                    item->setCurrentUrl(QUrl::fromLocalFile(curr));
                }
            }

            continue;
        }
    }

    return true;
}

QList<QAction*> ShowfotoStackViewFavorites::pluginActions() const
{
    return d->sidebar->pluginActions();
}

} // namespace ShowFoto
