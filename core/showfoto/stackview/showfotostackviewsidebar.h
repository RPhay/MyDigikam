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

#ifndef SHOWFOTO_STACK_VIEW_SIDEBAR_H
#define SHOWFOTO_STACK_VIEW_SIDEBAR_H

// QT includes

#include <QList>
#include <QUrl>
#include <QListView>
#include <QPixmap>
#include <QWidget>
#include <QEvent>
#include <QAction>
#include <QStringList>
#include <QModelIndex>

// Local includes

#include "statesavingobject.h"
#include "showfotoitemsortsettings.h"
#include "dpluginaction.h"

using namespace Digikam;

namespace ShowFoto
{

class ShowfotoThumbnailBar;
class Showfoto;

class ShowfotoStackViewSideBar : public QWidget,
                                 public StateSavingObject
{
    Q_OBJECT

public:

    explicit ShowfotoStackViewSideBar(Showfoto* const parent);
    ~ShowfotoStackViewSideBar()               override;

    void setThumbbar(ShowfotoThumbnailBar* const thumbbar);

    void          doLoadState()               override;
    void          doSaveState()               override;
    const QIcon   getIcon();
    const QString getCaption();

    void setSortOrder(int order);
    void setSortRole(int role);

    void registerPluginActions(const QList<DPluginAction*>& actions);
    QList<QAction*> pluginActions()     const;

    QList<QUrl> urls()                  const;

private Q_SLOTS:
/*
    void slotViewModeChanged(int);
    void slotShowBookmarks(bool);
*/
    void slotPluginActionTriggered();

Q_SIGNALS:

    void signalShowfotoItemInfoActivated(const ShowfotoItemInfo& info);
    void signalLoadContentsFromFiles(const QStringList& files);

/*
    void signalAddBookmark();
    void signalSetup();
*/
private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOWFOTO_STACK_VIEW_SIDEBAR_H
