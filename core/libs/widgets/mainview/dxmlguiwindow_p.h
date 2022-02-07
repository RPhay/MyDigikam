/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-04-29
 * Description : digiKam XML GUI window - Private container.
 *
 * Copyright (C) 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DXML_GUI_WINDOW_P_H
#define DIGIKAM_DXML_GUI_WINDOW_P_H

#include "dxmlguiwindow.h"

// Qt includes

#include <QString>
#include <QList>
#include <QMap>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QToolButton>
#include <QEvent>
#include <QHoverEvent>
#include <QApplication>
#include <QKeySequence>
#include <QMenuBar>
#include <QStatusBar>
#include <QScreen>
#include <QMenu>
#include <QUrl>
#include <QDomDocument>
#include <QUrlQuery>
#include <QIcon>
#include <QDir>
#include <QFileInfo>
#include <QResource>
#include <QStandardPaths>

// KDE includes

#include <kconfiggroup.h>
#include <ktogglefullscreenaction.h>
#include <ktoolbar.h>
#include <ktoggleaction.h>
#include <kstandardaction.h>
#include <kactioncollection.h>
#include <klocalizedstring.h>
#include <kwindowconfig.h>
#include <ksharedconfig.h>
#include <kshortcutsdialog.h>
#include <kedittoolbar.h>
#include <kxmlguifactory.h>

#ifdef HAVE_KNOTIFYCONFIG
#   include <knotifyconfigwidget.h>
#endif

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "daboutdata.h"
#include "dpluginloader.h"
#include "webbrowserdlg.h"
#include "solidhardwaredlg.h"

namespace Digikam
{

class Q_DECL_HIDDEN DXmlGuiWindow::Private
{
public:

    explicit Private()
      : fullScreenHideToolBars  (false),
        fullScreenHideThumbBar  (true),
        fullScreenHideSideBars  (false),
        fullScreenHideStatusBar (false),
        fsOptions               (FS_NONE),
        fullScreenAction        (nullptr),
        fullScreenParent        (nullptr),
        fullScreenBtn           (nullptr),
        dirtyMainToolBar        (false),
        thumbbarVisibility      (true),
        menubarVisibility       (true),
        statusbarVisibility     (true),
        dbStatAction            (nullptr),
        libsInfoAction          (nullptr),
        showMenuBarAction       (nullptr),
        showStatusBarAction     (nullptr),
        about                   (nullptr),
        anim                    (nullptr)
    {
    }

public:

    /**
     * Settings taken from managed window configuration to handle toolbar visibility in full-screen mode
     */
    bool                     fullScreenHideToolBars;

    /**
     * Settings taken from managed window configuration to handle thumbbar visibility in full-screen mode
     */
    bool                     fullScreenHideThumbBar;

    /**
     * Settings taken from managed window configuration to handle toolbar visibility in full-screen mode
     */
    bool                     fullScreenHideSideBars;

    /**
     * Settings taken from managed window configuration to handle statusbar visibility in full-screen mode
     */
    bool                     fullScreenHideStatusBar;

    /**
     * Full-Screen options. See FullScreenOptions enum and setFullScreenOptions() for details.
     */
    int                      fsOptions;

    /**
     * Action plug in managed window to switch fullscreen state
     */
    KToggleFullScreenAction* fullScreenAction;

    /**
     * Used under MacOS only. See bug #414117
     */
    QWidget*                 fullScreenParent;

    /**
     * Show only if toolbar is hidden
     */
    QToolButton*             fullScreenBtn;

    /**
     * Used by slotToggleFullScreen() to manage state of full-screen button on managed window
     */
    bool                     dirtyMainToolBar;

    /**
     * Store previous visibility of toolbars before ful-screen mode.
     */
    QMap<KToolBar*, bool>    toolbarsVisibility;

    /**
     * Store previous visibility of thumbbar before ful-screen mode.
     */
    bool                     thumbbarVisibility;

    /**
     * Store previous visibility of menubar before ful-screen mode.
     */
    bool                     menubarVisibility;

    /**
     * Store previous visibility of statusbar before ful-screen mode.
     */
    bool                     statusbarVisibility;

    // Common Help actions
    QAction*                 dbStatAction;
    QAction*                 libsInfoAction;
    QAction*                 showMenuBarAction;
    QAction*                 showStatusBarAction;
    DAboutData*              about;
    DLogoAction*             anim;

    QString                  configGroupName;
};

} // namespace Digikam

#endif // DIGIKAM_DXML_GUI_WINDOW_P_H
