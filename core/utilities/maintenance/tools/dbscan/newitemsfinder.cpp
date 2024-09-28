/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-01-20
 * Description : new items finder.
 *
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2012      by Andi Clemens <andi dot clemens at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "newitemsfinder.h"

// Qt includes

#include <QApplication>
#include <QTimer>
#include <QIcon>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "scancontroller.h"

namespace Digikam
{

class Q_DECL_HIDDEN NewItemsFinder::Private
{
public:

    Private() = default;

    FinderMode  mode    = CompleteCollectionScan;

    bool        cancel  = false;

    QStringList foldersToScan;
    QStringList foldersScanned;
};

NewItemsFinder::NewItemsFinder(const FinderMode mode,
                               const QStringList& foldersToScan,
                               ProgressItem* const parent)
    : MaintenanceTool(QLatin1String("NewItemsFinder"), parent),
      d              (new Private)
{
    d->mode          = mode;
    d->foldersToScan = foldersToScan;
    d->foldersToScan.sort();
}

NewItemsFinder::~NewItemsFinder()
{
    delete d;
}

void NewItemsFinder::slotStart()
{
    MaintenanceTool::slotStart();

    setShowAtStart(true);
    setLabel(i18n("Find new items"));
    setThumbnail(QIcon::fromTheme(QLatin1String("view-refresh")).pixmap(22));

    ProgressManager::addProgressItem(this);

    switch (d->mode)
    {
        case ScanDeferredFiles:
        {
            connectToScanController();

            qCDebug(DIGIKAM_GENERAL_LOG) << "scan mode: ScanDeferredFiles";

            ScanController::instance()->completeCollectionScanInBackground(false);
            ScanController::instance()->allowToScanDeferredFiles();
            break;
        }

        case CompleteCollectionScan:
        {
            connectToScanController();

            qCDebug(DIGIKAM_GENERAL_LOG) << "scan mode: CompleteCollectionScan";

            ScanController::instance()->completeCollectionScanInBackground(false);

            if (d->cancel)
            {
                break;
            }

            ScanController::instance()->allowToScanDeferredFiles();
            ScanController::instance()->completeCollectionScanInBackground(true);
            break;
        }

        case ScheduleCollectionScan:
        {
            d->foldersScanned.clear();

            // If we are scanning for newly imported files, we need to have the folders for scanning...

            if (d->foldersToScan.isEmpty())
            {
                qCWarning(DIGIKAM_GENERAL_LOG) << "NewItemsFinder called without any folders. Wrong call.";

                slotDone();

                return;
            }

            qCDebug(DIGIKAM_GENERAL_LOG) << "scan mode: ScheduleCollectionScan :: " << d->foldersToScan;

            connect(ScanController::instance(), SIGNAL(partialScanDone(QString)),
                    this, SLOT(slotPartialScanDone(QString)));

            setTotalItems(d->foldersToScan.count());

            for (const QString& folder : std::as_const(d->foldersToScan))
            {
                if (d->cancel)
                {
                    break;
                }

                ScanController::instance()->scheduleCollectionScan(folder);
            }

            break;
        }
    }
}

void NewItemsFinder::connectToScanController()
{
    // Common connections to ScanController

    connect(ScanController::instance(), SIGNAL(collectionScanStarted(QString)),
            this, SLOT(slotScanStarted(QString)));

    connect(ScanController::instance(), SIGNAL(totalFilesToScan(int)),
            this, SLOT(slotTotalFilesToScan(int)));

    connect(ScanController::instance(), SIGNAL(startScanningAlbum(QString,QString)),
            this, SLOT(slotStartScanningAlbum(QString,QString)));

    connect(ScanController::instance(), SIGNAL(filesScanned(int)),
            this, SLOT(slotFilesScanned(int)));

    connect(ScanController::instance(), SIGNAL(completeScanDone()),
            this, SLOT(slotDone()));
}

void NewItemsFinder::slotScanStarted(const QString& info)
{
    qCDebug(DIGIKAM_GENERAL_LOG) << info;
    setStatus(info);
}

void NewItemsFinder::slotTotalFilesToScan(int t)
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "total scan value : " << t;
    setTotalItems(t);
}

void NewItemsFinder::slotStartScanningAlbum(const QString& albumRoot, const QString& album)
{
    QString lbl = i18n("Scanning from: %1\n", albumRoot);
    lbl.append(i18n("Album: %1", album));
    setLabel(lbl);
}

void NewItemsFinder::slotFilesScanned(int s)
{
    //qCDebug(DIGIKAM_GENERAL_LOG) << "file scanned : " << s;
    advance(s);
}

void NewItemsFinder::slotCancel()
{
    d->cancel = true;

    ScanController::instance()->cancelCompleteScan();
    MaintenanceTool::slotCancel();
}

void NewItemsFinder::slotPartialScanDone(const QString& path)
{
    // Check if path scanned is included in planned list.

    if (d->foldersToScan.contains(path) && !d->foldersScanned.contains(path))
    {
        d->foldersScanned.append(path);
        d->foldersScanned.sort();

        QString lbl = i18n("Scanned:\n%1", path);
        setLabel(lbl);

        advance(1);

        // Check if all planned scanning is done

        if (d->foldersScanned == d->foldersToScan)
        {
            slotDone();
        }
    }
}

} // namespace Digikam

#include "moc_newitemsfinder.cpp"
