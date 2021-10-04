/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : a tool bar for Showfoto folder view
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

#ifndef SHOWFOTO_FOLDER_VIEW_BAR_H
#define SHOWFOTO_FOLDER_VIEW_BAR_H

// Qt includes

#include <QWidget>
#include <QList>
#include <QAction>

// Local includes

#include "slideshowsettings.h"
#include "dlayoutbox.h"
#include "dpluginaction.h"

using namespace Digikam;

namespace ShowFoto
{

class ShowfotoFolderViewList;
class ShowfotoFolderViewSideBar;

class ShowfotoFolderViewBar : public DVBox
{
    Q_OBJECT

public:

    enum FolderViewTypeMime
    {
        TYPE_MIME_JPEG = 0,
        TYPE_MIME_TIFF,
        TYPE_MIME_PNG,
        TYPE_MIME_PGF,
        TYPE_MIME_HEIF,
        TYPE_MIME_DNG,
        TYPE_MIME_RAW,
        TYPE_MIME_NORAW,
        TYPE_MIME_ALL
    };

public:

    explicit ShowfotoFolderViewBar(ShowfotoFolderViewSideBar* const parent);
    ~ShowfotoFolderViewBar()                          override;

    void setIconSize(int size);
    int  iconSize()                             const;

    void setFolderViewMode(int mode);
    int  folderViewMode()                       const;

    void setFolderViewTypeMime(int mime);
    int  folderViewTypeMime()                   const;

    void setBookmarksVisible(bool b);
    bool bookmarksVisible()                     const;

    void setCurrentPath(const QString& path);
    QString currentPath()                       const;

    QAction* toolBarAction(const QString& name) const;
    QList<QAction*> pluginActions()             const;

    void registerPluginActions(const QList<DPluginAction*>& actions);

public Q_SLOTS:

    void slotPreviousEnabled(bool);
    void slotNextEnabled(bool);

Q_SIGNALS:

    void signalTypeMimesChanged(const QString&);
    void signalViewModeChanged(int);
    void signalShowBookmarks(bool);
    void signalSetup();
    void signalIconSizeChanged(int);
    void signalGoHome();
    void signalGoUp();
    void signalGoPrevious();
    void signalGoNext();
    void signalCustomPathChanged(const QString&);
    void signalLoadContents();
    void signalPluginActionTriggered(QAction*);

private Q_SLOTS:

    void slotCustomPathChanged();
    void slotIconSizeChanged(int);
    void slotOptionsChanged(QAction*);
    void slotTypeMimesChanged(int index);

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOWFOTO_FOLDER_VIEW_BAR_H
