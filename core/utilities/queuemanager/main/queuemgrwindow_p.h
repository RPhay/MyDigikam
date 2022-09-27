/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-11-21
 * Description : Batch Queue Manager GUI
 *
 * SPDX-FileCopyrightText: 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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

#if !defined(Q_OS_DARWIN) && defined(Q_CC_GNU)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <klocalizedstring.h>
#include <kactioncollection.h>
#include <kconfiggroup.h>
#include <ksharedconfig.h>

// Restore warnings
#if !defined(Q_OS_DARWIN) && defined(Q_CC_GNU)
#   pragma GCC diagnostic pop
#endif

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic pop
#endif

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
