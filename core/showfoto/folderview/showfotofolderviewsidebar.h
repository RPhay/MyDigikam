/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : Side Bar Widget for the Showfoto folder view.
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

#ifndef SHOWFOTO_FOLDER_VIEW_SIDEBAR_H
#define SHOWFOTO_FOLDER_VIEW_SIDEBAR_H

// QT includes

#include <QListView>
#include <QPixmap>
#include <QWidget>
#include <QEvent>
#include <QAction>
#include <QModelIndex>

// Local includes

#include "statesavingobject.h"
#include "showfotoitemsortsettings.h"
#include "dpluginaction.h"

using namespace Digikam;

namespace ShowFoto
{

class Showfoto;

class ShowfotoFolderViewSideBar : public QWidget,
                                  public StateSavingObject
{
    Q_OBJECT

public:

    explicit ShowfotoFolderViewSideBar(Showfoto* const parent);
    ~ShowfotoFolderViewSideBar()              override;

    void          setActive(bool active);
    void          doLoadState()               override;
    void          doSaveState()               override;
    void          applySettings();
    const QIcon   getIcon();
    const QString getCaption();

    QString currentFolder()             const;
    QString currentPath()               const;

    void setCurrentPath(const QString& newPathNative);
    void setCurrentPathWithoutUndo(const QString& newPath);

    void setSortOrder(int order);
    void setSortRole(int role);

    void loadContents(const QModelIndex& index);

    void registerPluginActions(const QList<DPluginAction*>& actions);
    QList<QAction*> pluginActions()     const;

public Q_SLOTS:

    void slotUndo();
    void slotRedo();
    void slotGoHome();
    void slotGoUp();
    void slotLoadContents();

private Q_SLOTS:

    void slotCustomPathChanged(const QString&);
    void slotViewModeChanged(int);
    void slotShowBookmarks(bool);
    void slotPluginActionTriggered(QAction*);
    void slotTypeMimesChanged(const QString& patterns);

Q_SIGNALS:

    void signalAddBookmark();
    void signalSetup();
    void signalCurrentPathChanged(const QString& path);

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOWFOTO_FOLDER_VIEW_SIDEBAR_H
