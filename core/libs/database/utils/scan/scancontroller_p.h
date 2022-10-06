/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-10-28
 * Description : scan item controller - private containers.
 *
 * SPDX-FileCopyrightText: 2005-2006 by Tom Albers <tomalbers at kde dot nl>
 * SPDX-FileCopyrightText: 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2007-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_SCAN_CONTROLLER_P_H
#define DIGIKAM_SCAN_CONTROLLER_P_H

#include "scancontroller.h"

// Qt includes

#include <QStringList>
#include <QFileInfo>
#include <QPixmap>
#include <QIcon>
#include <QTime>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QTimer>
#include <QEventLoop>
#include <QApplication>
#include <QMessageBox>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "coredbaccess.h"
#include "collectionmanager.h"
#include "collectionlocation.h"
#include "filereadwritelock.h"
#include "coredbwatch.h"
#include "dprogressdlg.h"
#include "dmetadata.h"
#include "coredb.h"
#include "albummanager.h"
#include "album.h"
#include "coredbschemaupdater.h"
#include "iteminfo.h"

namespace Digikam
{

/*
 * This class is derived from the LoadingCacheFileWatch,
 * which means it has the full functionality of the class
 * and only extends it by listening to CollectionScanner information
 */
class Q_DECL_HIDDEN ScanControllerLoadingCacheFileWatch : public LoadingCacheFileWatch
{
    Q_OBJECT

public:

    ScanControllerLoadingCacheFileWatch();

private Q_SLOTS:

    void slotImageChanged(const ImageChangeset& changeset);

private:

    // Disable
    explicit ScanControllerLoadingCacheFileWatch(QObject*) = delete;
};

// ------------------------------------------------------------------------------

class Q_DECL_HIDDEN SimpleCollectionScannerObserver : public CollectionScannerObserver
{
public:

    explicit SimpleCollectionScannerObserver(bool* const var);

    bool continueQuery() override;

public:

    bool* m_continue;
};

// ------------------------------------------------------------------------------

class Q_DECL_HIDDEN ScanController::Private
{
public:

    explicit Private();


    QPixmap albumPixmap();
    QPixmap rootPixmap();
    QPixmap actionPixmap();
    QPixmap errorPixmap();
    QPixmap restartPixmap();

    void garbageCollectHints(bool setAccessTime);

public:

    bool                            running;
    bool                            needsInitialization;
    bool                            needsCompleteScan;
    bool                            needsUpdateUniqueHash;
    bool                            idle;

    int                             scanSuspended;

    QStringList                     scanTasks;

    QStringList                     completeScanDeferredAlbums;
    bool                            deferFileScanning;
    bool                            finishScanAllowed;

    QMutex                          mutex;
    QWaitCondition                  condVar;

    bool                            continueInitialization;
    bool                            continueScan;
    bool                            continuePartialScan;

    bool                            fileWatchInstalled;

    QEventLoop*                     eventLoop;

    QTimer*                         showTimer;
    QTimer*                         relaxedTimer;
    QTimer*                         externalTimer;

    QPixmap                         albumPix;
    QPixmap                         rootPix;
    QPixmap                         actionPix;
    QPixmap                         errorPix;

    CollectionScannerHintContainer* hints;

    QDateTime                       lastHintAdded;

    DProgressDlg*                   progressDialog;

    ScanController::Advice          advice;

    bool                            needTotalFiles;
    bool                            performFastScan;
    int                             totalFilesToScan;

    QList<qlonglong>                newIdsList;
};

// ------------------------------------------------------------------------------

class Q_DECL_HIDDEN ScanControllerCreator
{
public:

    ScanController object;
};

} // namespace Digikam

#endif // DIGIKAM_SCAN_CONTROLLER_P_H
