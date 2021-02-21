/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-11-21
 * Description : Batch Queue Manager GUI
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_BQM_QUEUE_MGR_WINDOW_PRIVATE_H
#define DIGIKAM_BQM_QUEUE_MGR_WINDOW_PRIVATE_H

#include "queuemgrwindow.h"

// Qt includes

#include <QLabel>
#include <QString>
#include <QAction>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QKeySequence>
#include <QMenuBar>
#include <QStatusBar>
#include <QMenu>
#include <QMessageBox>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>
#include <kactioncollection.h>
#include <kconfiggroup.h>
#include <ksharedconfig.h>

// Local includes

#include "drawdecoder.h"
#include "digikam_debug.h"
#include "actions.h"
#include "album.h"
#include "batchtoolsfactory.h"
#include "actionthread.h"
#include "queuepool.h"
#include "workflowmanager.h"
#include "queuelist.h"
#include "queuesettings.h"
#include "queuesettingsview.h"
#include "assignedlist.h"
#include "toolsettingsview.h"
#include "toolsview.h"
#include "componentsinfodlg.h"
#include "digikamapp.h"
#include "thememanager.h"
#include "dimg.h"
#include "dlogoaction.h"
#include "albummanager.h"
#include "imagewindow.h"
#include "thumbnailsize.h"
#include "sidebar.h"
#include "dnotificationwrapper.h"
#include "statusprogressbar.h"

namespace Digikam
{

class BatchToolsFactory;

class Q_DECL_HIDDEN QueueMgrWindow::Private
{

public:

    explicit Private()
        : busy                          (false),
          processingAllQueues           (false),
          currentQueueToProcess         (0),
          statusLabel                   (nullptr),
          clearQueueAction              (nullptr),
          removeItemsSelAction          (nullptr),
          removeItemsDoneAction         (nullptr),
          moveUpToolAction              (nullptr),
          moveDownToolAction            (nullptr),
          removeToolAction              (nullptr),
          clearToolsAction              (nullptr),
          runAction                     (nullptr),
          runAllAction                  (nullptr),
          stopAction                    (nullptr),
          removeQueueAction             (nullptr),
          newQueueAction                (nullptr),
          saveQueueAction               (nullptr),
          donateMoneyAction             (nullptr),
          contributeAction              (nullptr),
          rawCameraListAction           (nullptr),
          topSplitter                   (nullptr),
          bottomSplitter                (nullptr),
          verticalSplitter              (nullptr),
          batchToolsMgr                 (nullptr),
          statusProgressBar             (nullptr),
          thread                        (nullptr),
          toolsView                     (nullptr),
          toolSettings                  (nullptr),
          assignedList                  (nullptr),
          queuePool                     (nullptr),
          queueSettingsView             (nullptr),
          TOP_SPLITTER_CONFIG_KEY       (QLatin1String("BqmTopSplitter")),
          BOTTOM_SPLITTER_CONFIG_KEY    (QLatin1String("BqmBottomSplitter")),
          VERTICAL_SPLITTER_CONFIG_KEY  (QLatin1String("BqmVerticalSplitter"))
    {
    }

    bool                     busy;
    bool                     processingAllQueues;

    int                      currentQueueToProcess;

    QLabel*                  statusLabel;

    QAction*                 clearQueueAction;
    QAction*                 removeItemsSelAction;
    QAction*                 removeItemsDoneAction;
    QAction*                 moveUpToolAction;
    QAction*                 moveDownToolAction;
    QAction*                 removeToolAction;
    QAction*                 clearToolsAction;

    QAction*                 runAction;
    QAction*                 runAllAction;
    QAction*                 stopAction;
    QAction*                 removeQueueAction;
    QAction*                 newQueueAction;
    QAction*                 saveQueueAction;
    QAction*                 donateMoneyAction;
    QAction*                 contributeAction;
    QAction*                 rawCameraListAction;

    SidebarSplitter*         topSplitter;
    SidebarSplitter*         bottomSplitter;
    SidebarSplitter*         verticalSplitter;

    BatchToolsFactory*       batchToolsMgr;

    StatusProgressBar*       statusProgressBar;

    ActionThread*            thread;

    ToolsView*               toolsView;
    ToolSettingsView*        toolSettings;
    AssignedListView*        assignedList;
    QueuePool*               queuePool;
    QueueSettingsView*       queueSettingsView;

    const QString            TOP_SPLITTER_CONFIG_KEY;
    const QString            BOTTOM_SPLITTER_CONFIG_KEY;
    const QString            VERTICAL_SPLITTER_CONFIG_KEY;
};

} // namespace Digikam

#endif // DIGIKAM_BQM_QUEUE_MGR_WINDOW_PRIVATE_H
