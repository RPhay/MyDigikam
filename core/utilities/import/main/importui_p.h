/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-09-16
 * Description : Camera interface
 *
 * SPDX-FileCopyrightText: 2004-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_IMPORTUI_PRIVATE_H
#define DIGIKAM_IMPORTUI_PRIVATE_H

#include "importui.h"

// Qt includes

#include <QCheckBox>
#include <QCloseEvent>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPointer>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSplitter>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QKeySequence>
#include <QInputDialog>
#include <QMenuBar>
#include <QMenu>
#include <QIcon>
#include <QMessageBox>
#include <QStatusBar>
#include <QApplication>
#include <QActionGroup>
#include <QLocale>
#include <QDateTime>
#include <QString>
#include <QUrl>

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
#include <kselectaction.h>

// Restore warnings
#if !defined(Q_OS_DARWIN) && defined(Q_CC_GNU)
#   pragma GCC diagnostic pop
#endif

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic pop
#endif

// Local includes

#include "digikam_config.h"
#include "digikam_debug.h"
#include "digikam_globals.h"
#include "drawdecoder.h"
#include "dlayoutbox.h"
#include "dexpanderbox.h"
#include "dfileselector.h"
#include "cameramessagebox.h"
#include "advancedrenamemanager.h"
#include "album.h"
#include "albummanager.h"
#include "applicationsettings.h"
#include "albumselectdialog.h"
#include "cameracontroller.h"
#include "camerafolderdialog.h"
#include "camerainfodialog.h"
#include "cameralist.h"
#include "cameranamehelper.h"
#include "cameratype.h"
#include "capturedlg.h"
#include "collectionlocation.h"
#include "collectionmanager.h"
#include "collectionscanner.h"
#include "componentsinfodlg.h"
#include "dlogoaction.h"
#include "coredbdownloadhistory.h"
#include "dzoombar.h"
#include "fileactionmngr.h"
#include "freespacewidget.h"
#include "iccsettings.h"
#include "importitempropertiessidebar.h"
#include "importsettings.h"
#include "importview.h"
#include "imagedialog.h"
#include "dnotificationwrapper.h"
#include "newitemsfinder.h"
#include "parsesettings.h"
#include "renamecustomizer.h"
#include "scancontroller.h"
#include "setup.h"
#include "sidebar.h"
#include "statusprogressbar.h"
#include "thememanager.h"
#include "thumbnailsize.h"
#include "importthumbnailmodel.h"
#include "itempropertiestab.h"
#include "camerathumbsctrl.h"
#include "importfiltercombobox.h"
#include "albumcustomizer.h"
#include "advancedsettings.h"
#include "dngconvertsettings.h"
#include "scriptingsettings.h"
#include "filterstatusbar.h"
#include "dnotificationwidget.h"
#include "dfileoperations.h"
#include "setupcamera.h"

namespace Digikam
{

class Q_DECL_HIDDEN ImportUI::Private
{
public:

    Private()
      : waitAutoRotate                  (false),
        deleteAfter                     (false),
        busy                            (false),
        closed                          (false),
        downloadMenu                    (nullptr),
        deleteMenu                      (nullptr),
        imageMenu                       (nullptr),
        cameraCancelAction              (nullptr),
        cameraCaptureAction             (nullptr),
        cameraInfoAction                (nullptr),
        increaseThumbsAction            (nullptr),
        decreaseThumbsAction            (nullptr),
        zoomFitToWindowAction           (nullptr),
        zoomTo100percents               (nullptr),
        deleteAction                    (nullptr),
        deleteNewAction                 (nullptr),
        deleteAllAction                 (nullptr),
        deleteSelectedAction            (nullptr),
        downloadAction                  (nullptr),
        downloadNewAction               (nullptr),
        downloadAllAction               (nullptr),
        downloadSelectedAction          (nullptr),
        downloadDelNewAction            (nullptr),
        downloadDelAllAction            (nullptr),
        downloadDelSelectedAction       (nullptr),
        lockAction                      (nullptr),
        selectAllAction                 (nullptr),
        selectInvertAction              (nullptr),
        selectLockedItemsAction         (nullptr),
        selectNewItemsAction            (nullptr),
        selectNoneAction                (nullptr),
        uploadAction                    (nullptr),
        markAsDownloadedAction          (nullptr),
        resumeAction                    (nullptr),
        pauseAction                     (nullptr),
        connectAction                   (nullptr),
        itemSortAction                  (nullptr),
        itemSortOrderAction             (nullptr),
        itemsGroupAction                (nullptr),
        showPreferencesAction           (nullptr),
        showLogAction                   (nullptr),
        showBarAction                   (nullptr),
        imageViewSelectionAction        (nullptr),
        iconViewAction                  (nullptr),
        camItemPreviewAction            (nullptr),

#ifdef HAVE_MARBLE

        mapViewAction                   (nullptr),

#endif // HAVE_MARBLE

        viewCMViewAction                (nullptr),
        cameraActions                   (nullptr),
        advBox                          (nullptr),
        splitter                        (nullptr),
        camThumbsCtrl                   (nullptr),
        controller                      (nullptr),
/*
        //historyUpdater                (0),
*/
        view                            (nullptr),
        renameCustomizer                (nullptr),
        albumCustomizer                 (nullptr),
        advancedSettings                (nullptr),
        dngConvertSettings              (nullptr),
        scriptingSettings               (nullptr),
        filterStatusBar                 (nullptr),
        rightSideBar                    (nullptr),
        zoomBar                         (nullptr),
        statusProgressBar               (nullptr),
        albumLibraryFreeSpace           (nullptr),
        cameraFreeSpace                 (nullptr),
        progressTimer                   (nullptr),
        progressValue                   (0),
        historyView                     (nullptr),
        filterComboBox                  (nullptr),
        errorWidget                     (nullptr)
    {
    }

    static const QString                     configGroupName;
    static const QString                     configUseFileMetadata;
    static const QString                     configUseDefaultTargetAlbum;
    static const QString                     configLastTargetAlbum;
    static const QString                     configDefaultTargetAlbumId;
    static const QString                     configFileSaveConflictRule;
    static const QString                     importFiltersConfigGroupName;

    bool                                     waitAutoRotate;
    bool                                     deleteAfter;
    bool                                     busy;
    bool                                     closed;

    QString                                  cameraTitle;

    QHash<QString, QPair<QString, QString> > downloadedInfoHash;
    QHash<QString, QDateTime>                downloadedDateHash;
    QStringList                              downloadedItemList;
    QStringList                              currentlyDeleting;
    QStringList                              foldersToScan;

    QMenu*                                   downloadMenu;
    QMenu*                                   deleteMenu;
    QMenu*                                   imageMenu;

    QAction*                                 cameraCancelAction;
    QAction*                                 cameraCaptureAction;
    QAction*                                 cameraInfoAction;
    QAction*                                 increaseThumbsAction;
    QAction*                                 decreaseThumbsAction;
    QAction*                                 zoomFitToWindowAction;
    QAction*                                 zoomTo100percents;
    QMenu*                                   deleteAction;
    QAction*                                 deleteNewAction;
    QAction*                                 deleteAllAction;
    QAction*                                 deleteSelectedAction;
    QMenu*                                   downloadAction;
    QAction*                                 downloadNewAction;
    QAction*                                 downloadAllAction;
    QAction*                                 downloadSelectedAction;
    QAction*                                 downloadDelNewAction;
    QAction*                                 downloadDelAllAction;
    QAction*                                 downloadDelSelectedAction;
    QAction*                                 lockAction;
    QAction*                                 selectAllAction;
    QAction*                                 selectInvertAction;
    QAction*                                 selectLockedItemsAction;
    QAction*                                 selectNewItemsAction;
    QAction*                                 selectNoneAction;
    QAction*                                 uploadAction;
    QAction*                                 markAsDownloadedAction;
    QAction*                                 resumeAction;
    QAction*                                 pauseAction;
    QAction*                                 connectAction;
    KSelectAction*                           itemSortAction;
    KSelectAction*                           itemSortOrderAction;
    KSelectAction*                           itemsGroupAction;
    QAction*                                 showPreferencesAction;
    QAction*                                 showLogAction;
    QAction*                                 showBarAction;
    KSelectAction*                           imageViewSelectionAction;
    QAction*                                 iconViewAction;
    QAction*                                 camItemPreviewAction;

#ifdef HAVE_MARBLE

    QAction*                                 mapViewAction;

#endif // HAVE_MARBLE

    QAction*                                 viewCMViewAction;

    QActionGroup*                            cameraActions;

    QUrl                                     lastDestURL;

    DExpanderBox*                            advBox;

    SidebarSplitter*                         splitter;

    CameraThumbsCtrl*                        camThumbsCtrl;
    CameraController*                        controller;
/*
    CameraHistoryUpdater*                    historyUpdater;
*/
    ImportView*                              view;

    RenameCustomizer*                        renameCustomizer;
    AlbumCustomizer*                         albumCustomizer;
    AdvancedSettings*                        advancedSettings;
    DNGConvertSettings*                      dngConvertSettings;
    ScriptingSettings*                       scriptingSettings;

    FilterStatusBar*                         filterStatusBar;
    ImportItemPropertiesSideBarImport*       rightSideBar;

    DZoomBar*                                zoomBar;
    StatusProgressBar*                       statusProgressBar;

    FreeSpaceWidget*                         albumLibraryFreeSpace;
    FreeSpaceWidget*                         cameraFreeSpace;

    QTimer*                                  progressTimer;

    float                                    progressValue;

    DHistoryView*                            historyView;
    ImportFilterComboBox*                    filterComboBox;
/*
    CHUpdateItemMap                          map;
*/
    DNotificationWidget*                     errorWidget;
};

const QString ImportUI::Private::configGroupName(QLatin1String("Camera Settings"));
const QString ImportUI::Private::configUseFileMetadata(QLatin1String("UseFileMetadata"));
const QString ImportUI::Private::configUseDefaultTargetAlbum(QLatin1String("UseDefaultTargetAlbum"));
const QString ImportUI::Private::configLastTargetAlbum(QLatin1String("LastTargetAlbum"));
const QString ImportUI::Private::configDefaultTargetAlbumId(QLatin1String("DefaultTargetAlbumId"));
const QString ImportUI::Private::configFileSaveConflictRule(QLatin1String("FileSaveConflictRule"));
const QString ImportUI::Private::importFiltersConfigGroupName(QLatin1String("Import Filters"));

} // namespace Digikam

#endif // DIGIKAM_IMPORTUI_PRIVATE_H
