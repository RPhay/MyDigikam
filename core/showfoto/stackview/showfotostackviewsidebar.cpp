/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-09-27
 * Description : Side Bar Widget for the Showfoto stack view.
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

#include "showfotostackviewsidebar.h"

// Qt includes

#include <QApplication>
#include <QStyle>
#include <QIcon>
#include <QUndoStack>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include <QSplitter>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "showfoto.h"
#include "showfotostackviewlist.h"
#include "showfotothumbnailbar.h"
#include "showfotostackviewfavorites.h"
//#include "showfotostackviewbar.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoStackViewSideBar::Private
{

public:

    explicit Private()
      : pluginFingerPrint(QLatin1String("DPlugin::Generic::View")),
        view             (nullptr),
//      bar              (nullptr),
        favts            (nullptr),
        splitter         (nullptr),
        sortOrder        (Qt::AscendingOrder),
        role             (ShowfotoStackViewList::FileName)
    {
    }

    const QString                        pluginFingerPrint;
    static const QString                 configIconSizeEntry;
    static const QString                 configFavoritesVisibleEntry;
    static const QString                 configSplitterStateEntry;

    ShowfotoStackViewList*               view;

//    ShowfotoStackViewBar*                bar;
    ShowfotoStackViewFavorites*          favts;
    QSplitter*                           splitter;

    QList<DPluginAction*>                dpluginActions;
    QList<QAction*>                      pluginActions;
    Qt::SortOrder                        sortOrder;
    ShowfotoStackViewList::StackViewRole role;
};

const QString ShowfotoStackViewSideBar::Private::configIconSizeEntry(QLatin1String("Icon Size"));
const QString ShowfotoStackViewSideBar::Private::configFavoritesVisibleEntry(QLatin1String("Favorites Visible"));
const QString ShowfotoStackViewSideBar::Private::configSplitterStateEntry(QLatin1String("Splitter State"));

ShowfotoStackViewSideBar::ShowfotoStackViewSideBar(Showfoto* const parent)
    : QWidget          (parent),
      StateSavingObject(this),
      d                (new Private)
{
    setObjectName(QLatin1String("ShowfotoStackView Sidebar"));

    // --- Populate the view

//    d->bar                   = new ShowfotoStackViewBar(this);
    d->view                    = new ShowfotoStackViewList(this);
    d->view->setEnableToolTips(true);

    d->favts                   = new ShowfotoStackViewFavorites(this);

    d->splitter                = new QSplitter(Qt::Vertical, this);
    d->splitter->addWidget(d->view);
    d->splitter->addWidget(d->favts);
    d->splitter->setStretchFactor(0, 10);
    d->splitter->setStretchFactor(1, 3);

    QVBoxLayout* const layout  = new QVBoxLayout(this);
//    layout->addWidget(d->bar);
    layout->addWidget(d->splitter);
    layout->setContentsMargins(0, 0, 0, 0);

    // --- Setup connections

    connect(d->favts, SIGNAL(signalLoadContentsFromFiles(QStringList)),
            this, SIGNAL(signalLoadContentsFromFiles(QStringList)));
/*
    connect(d->bar, SIGNAL(signalSetup()),
            this, SIGNAL(signalSetup()));

    connect(d->bar, SIGNAL(signalShowBookfavts(bool)),
            this, SLOT(slotShowBookfavts(bool)));

    connect(d->bar, SIGNAL(signalViewModeChanged(int)),
            this, SLOT(slotViewModeChanged(int)));

    connect(d->bar, SIGNAL(signalIconSizeChanged(int)),
            d->view, SLOT(slotIconSizeChanged(int)));

    connect(d->bar, SIGNAL(signalGoHome()),
            this, SLOT(slotGoHome()));

    connect(d->bar, SIGNAL(signalGoUp()),
            this, SLOT(slotGoUp()));

    connect(d->bar, SIGNAL(signalLoadContents()),
            this, SLOT(slotLoadContents()));

    connect(d->favts, SIGNAL(signalLoadContents()),
            this, SLOT(slotLoadContents()));

    connect(d->bar, SIGNAL(signalCustomPathChanged(QString)),
            this, SLOT(slotCustomPathChanged(QString)));

    connect(d->bar, SIGNAL(signalTypeMimesChanged(QString)),
            this, SLOT(slotTypeMimesChanged(QString)));

    connect(d->bar, SIGNAL(signalGoNext()),
            this, SLOT(slotRedo()));

    connect(d->bar, SIGNAL(signalGoPrevious()),
            this, SLOT(slotUndo()));

*/

    connect(d->view, SIGNAL(signalAddFavorite()),
            this, SIGNAL(signalAddFavorite()));

    connect(d->view, SIGNAL(signalShowfotoItemInfoActivated(ShowfotoItemInfo)),
            this, SIGNAL(signalShowfotoItemInfoActivated(ShowfotoItemInfo)));

    connect(d->view, SIGNAL(signalItemListChanged(int)),
            d->favts, SLOT(slotItemListChanged(int)));
}

ShowfotoStackViewSideBar::~ShowfotoStackViewSideBar()
{
    delete d;
}

void ShowfotoStackViewSideBar::setThumbbar(ShowfotoThumbnailBar* const thumbbar)
{
    d->view->setThumbbar(thumbbar);
}

QList<QUrl> ShowfotoStackViewSideBar::urls() const
{
    return d->view->urls();
}

void ShowfotoStackViewSideBar::setSortOrder(int order)
{
    d->sortOrder = (order == ShowfotoItemSortSettings::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder; // Inverted compared to Thumbbar.
    d->view->sortItems(d->role, d->sortOrder);
}

void ShowfotoStackViewSideBar::setSortRole(int role)
{
    d->role = (ShowfotoStackViewList::StackViewRole)role;
    d->view->sortItems(d->role, d->sortOrder);
}

/*

void ShowfotoStackViewSideBar::slotViewModeChanged(int mode)
{
    switch (mode)
    {
        case ShowfotoStackViewList::ShortView:
        {
            d->view->setColumnHidden(ShowfotoStackViewList::FileSize, true);
            d->view->setColumnHidden(ShowfotoStackViewList::FileType, true);
            d->view->setColumnHidden(ShowfotoStackViewList::FileDate, true);
            d->view->setHeaderHidden(true);
            break;
        }

        default:    // ShowfotoStackViewList::DetailledView
        {
            d->view->setColumnHidden(ShowfotoStackViewList::FileSize, false);
            d->view->setColumnHidden(ShowfotoStackViewList::FileType, false);
            d->view->setColumnHidden(ShowfotoStackViewList::FileDate, false);
            d->view->setHeaderHidden(false);
            break;
        }
    }
}

void ShowfotoStackViewSideBar::slotShowBookfavts(bool visible)
{
    d->favts->setVisible(visible);
}
*/
void ShowfotoStackViewSideBar::registerPluginActions(const QList<DPluginAction*>& actions)
{
    d->dpluginActions = actions;

   foreach (QAction* const dpact, d->dpluginActions)
   {
       QAction* const act = new QAction(dpact->text(), this);
       act->setObjectName(dpact->objectName());
       act->setIcon(dpact->icon());
       act->setToolTip(dpact->toolTip());
       act->setData(d->pluginFingerPrint);

       connect(act, SIGNAL(triggered(bool)),
               this, SLOT(slotPluginActionTriggered()));

       d->pluginActions << act;
    }
}

void ShowfotoStackViewSideBar::slotPluginActionTriggered()
{
    QAction* const act = dynamic_cast<QAction*>(sender());

    if (act)
    {
        foreach (QAction* const dpact, d->dpluginActions)
        {
            if (act->objectName() == dpact->objectName())
            {
                d->favts->loadContents();
                QTimer::singleShot(1000, dpact, SLOT(trigger()));
                return;
            }
        }
    }
}

QList<QAction*> ShowfotoStackViewSideBar::pluginActions() const
{
    return d->pluginActions;
}

const QIcon ShowfotoStackViewSideBar::getIcon()
{
    return QIcon::fromTheme(QLatin1String("photos"));
}

const QString ShowfotoStackViewSideBar::getCaption()
{
    return i18nc("@title: file system tree", "Stack");
}

void ShowfotoStackViewSideBar::doLoadState()
{
    KConfigGroup group = getConfigGroup();

    d->favts->readSettings(group);
/*
    d->bar->setBookfavtsVisible(group.readEntry(entryName(d->configBookfavtsVisibleEntry),     false));
*/
    int iconSize     = group.readEntry(entryName(d->configIconSizeEntry),                      (int)ShowfotoStackViewList::SizeSmall);
    d->view->setIconSize(QSize(iconSize, iconSize));
    QByteArray state = group.readEntry(entryName(d->configSplitterStateEntry),                 QByteArray());

    if (!state.isEmpty())
    {
        d->splitter->restoreState(QByteArray::fromBase64(state));
    }
}

void ShowfotoStackViewSideBar::doSaveState()
{
    KConfigGroup group = getConfigGroup();

    d->favts->saveSettings(group);
/*
    group.writeEntry(entryName(d->configBookfavtsVisibleEntry),     d->bar->bookfavtsVisible());
*/
    group.writeEntry(entryName(d->configIconSizeEntry),             d->view->iconSize().width());
    group.writeEntry(entryName(d->configSplitterStateEntry),        d->splitter->saveState().toBase64());
    group.sync();
}

} // namespace ShowFoto
