/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-01-31
 * Description : maintenance manager
 *
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2012      by Andi Clemens <andi dot clemens at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "maintenancemngr.h"

// Qt includes

#include <QString>
#include <QElapsedTimer>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"
#include "albummanager.h"
#include "applicationsettings.h"
#include "maintenancesettings.h"
#include "newitemsfinder.h"
#include "thumbsgenerator.h"
#include "fingerprintsgenerator.h"
#include "duplicatesfinder.h"
#include "autotagsassignment.h"
#include "imagequalitysorter.h"
#include "metadatasynchronizer.h"
#include "dnotificationwrapper.h"
#include "progressmanager.h"
#include "facesdetector.h"
#include "dbcleaner.h"

namespace Digikam
{

class Q_DECL_HIDDEN MaintenanceMngr::Private
{
public:

    Private() = default;

public:

    bool                   running               = false;

    QElapsedTimer          duration;

    MaintenanceSettings    settings;

    NewItemsFinder*        newItemsFinder        = nullptr;
    ThumbsGenerator*       thumbsGenerator       = nullptr;
    FingerPrintsGenerator* fingerPrintsGenerator = nullptr;
    DuplicatesFinder*      duplicatesFinder      = nullptr;
    MetadataSynchronizer*  metadataSynchronizer  = nullptr;
    AutotagsAssignment*    autotagsAssignment    = nullptr;
    ImageQualitySorter*    imageQualitySorter    = nullptr;
    FacesDetector*         facesDetector         = nullptr;
    DbCleaner*             databaseCleaner       = nullptr;
};

MaintenanceMngr::MaintenanceMngr(QObject* const parent)
    : QObject(parent),
      d      (new Private)
{
    connect(ProgressManager::instance(), SIGNAL(progressItemCompleted(ProgressItem*)),
            this, SLOT(slotToolCompleted(ProgressItem*)));

    connect(ProgressManager::instance(), SIGNAL(progressItemCanceled(ProgressItem*)),
            this, SLOT(slotToolCanceled(ProgressItem*)));
}

MaintenanceMngr::~MaintenanceMngr()
{
    delete d;
}

bool MaintenanceMngr::isRunning() const
{
    return d->running;
}

void MaintenanceMngr::setSettings(const MaintenanceSettings& settings)
{
    d->settings = settings;
    qCDebug(DIGIKAM_GENERAL_LOG) << d->settings;

    d->duration.start();
    stage1();
}

void MaintenanceMngr::slotToolCompleted(ProgressItem* tool)
{
    // At each stage, relevant tool instance is set to zero to prevent redondant call to this slot
    // from ProgressManager. This will disable multiple triggering in this method.
    // There is no memory leak. Each tool instance are delete later by ProgressManager.

    if      (tool == dynamic_cast<ProgressItem*>(d->newItemsFinder))
    {
        d->newItemsFinder = nullptr;

        // Update albums and tags for the other tools

        if (d->settings.wholeAlbums)
        {
            d->settings.albums = AlbumManager::instance()->allPAlbums();
        }

        if (d->settings.wholeTags)
        {
            d->settings.tags = AlbumManager::instance()->allTAlbums();
        }

        stage2();
    }
    else if (tool == dynamic_cast<ProgressItem*>(d->databaseCleaner))
    {
        d->databaseCleaner = nullptr;
        stage3();
    }
    else if (tool == dynamic_cast<ProgressItem*>(d->thumbsGenerator))
    {
        d->thumbsGenerator = nullptr;
        stage4();
    }
    else if (tool == dynamic_cast<ProgressItem*>(d->fingerPrintsGenerator))
    {
        d->fingerPrintsGenerator = nullptr;
        stage5();
    }
    else if (tool == dynamic_cast<ProgressItem*>(d->duplicatesFinder))
    {
        d->duplicatesFinder = nullptr;
        stage6();
    }
    else if (tool == dynamic_cast<ProgressItem*>(d->facesDetector))
    {
        d->facesDetector = nullptr;
        stage7();
    }
    else if(tool == dynamic_cast<ProgressItem*>(d->autotagsAssignment))
    {
        d->autotagsAssignment = nullptr;
        stage8();
    }
   else if (tool == dynamic_cast<ProgressItem*>(d->imageQualitySorter))
    {
        d->imageQualitySorter = nullptr;
        stage9();
    }
    else if (tool == dynamic_cast<ProgressItem*>(d->metadataSynchronizer))
    {
        d->metadataSynchronizer = nullptr;
        done();
    }
}

void MaintenanceMngr::slotToolCanceled(ProgressItem* tool)
{
    if (
        (tool == dynamic_cast<ProgressItem*>(d->newItemsFinder))        ||
        (tool == dynamic_cast<ProgressItem*>(d->thumbsGenerator))       ||
        (tool == dynamic_cast<ProgressItem*>(d->fingerPrintsGenerator)) ||
        (tool == dynamic_cast<ProgressItem*>(d->duplicatesFinder))      ||
        (tool == dynamic_cast<ProgressItem*>(d->databaseCleaner))       ||
        (tool == dynamic_cast<ProgressItem*>(d->facesDetector))         ||
        (tool == dynamic_cast<ProgressItem*>(d->imageQualitySorter))    ||
        (tool == dynamic_cast<ProgressItem*>(d->metadataSynchronizer))  ||
        (tool == dynamic_cast<ProgressItem*>(d->autotagsAssignment))
       )
    {
        cancel();
    }
}

void MaintenanceMngr::stage1()
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "stage1";

    if (d->settings.newItems)
    {
        if (d->settings.wholeAlbums)
        {
            d->newItemsFinder = new NewItemsFinder();
        }
        else
        {
            QStringList paths;

            for (Album* const a : std::as_const(d->settings.albums))
            {
                PAlbum* const palbum = dynamic_cast<PAlbum*>(a);

                if (palbum)
                {
                    paths << palbum->folderPath();
                }
            }

            d->newItemsFinder = new NewItemsFinder(NewItemsFinder::ScheduleCollectionScan, paths);
        }

        d->newItemsFinder->setNotificationEnabled(false);
        d->newItemsFinder->start();
    }
    else
    {
        stage2();
    }
}

void MaintenanceMngr::stage2()
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "stage2";

    if (d->settings.databaseCleanup)
    {
        d->databaseCleaner = new DbCleaner(d->settings.cleanThumbDb,
                                           d->settings.cleanFacesDb,
                                           d->settings.cleanSimilarityDb,
                                           d->settings.shrinkDatabases);
        d->databaseCleaner->setNotificationEnabled(false);
        d->databaseCleaner->setUseMultiCoreCPU(d->settings.useMutiCoreCPU);
        d->databaseCleaner->start();
    }
    else
    {
        stage3();
    }
}

void MaintenanceMngr::stage3()
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "stage3";

    if (d->settings.thumbnails)
    {
        bool rebuildAll = (d->settings.scanThumbs == false);
        AlbumList list;
        list << d->settings.albums;
        list << d->settings.tags;

        d->thumbsGenerator = new ThumbsGenerator(rebuildAll, list);
        d->thumbsGenerator->setNotificationEnabled(false);
        d->thumbsGenerator->setUseMultiCoreCPU(d->settings.useMutiCoreCPU);
        d->thumbsGenerator->start();
    }
    else
    {
        stage4();
    }
}

void MaintenanceMngr::stage4()
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "stage4";

    if (d->settings.fingerPrints)
    {
        bool rebuildAll = (d->settings.scanFingerPrints == false);
        AlbumList list;
        list << d->settings.albums;
        list << d->settings.tags;

        d->fingerPrintsGenerator = new FingerPrintsGenerator(rebuildAll, list);
        d->fingerPrintsGenerator->setNotificationEnabled(false);
        d->fingerPrintsGenerator->setUseMultiCoreCPU(d->settings.useMutiCoreCPU);
        d->fingerPrintsGenerator->start();
    }
    else
    {
        stage5();
    }
}

void MaintenanceMngr::stage5()
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "stage5";

    if (d->settings.duplicates)
    {
        d->duplicatesFinder = new DuplicatesFinder(d->settings.albums, d->settings.tags, (int)HaarIface::AlbumTagRelation::NoMix,
                                                   d->settings.minSimilarity, d->settings.maxSimilarity,(int)d->settings.duplicatesRestriction);
        d->duplicatesFinder->setNotificationEnabled(false);
        d->duplicatesFinder->start();
    }
    else
    {
        stage6();
    }
}

void MaintenanceMngr::stage6()
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "stage6";

    if (d->settings.faceManagement)
    {
        // NOTE : Use multi-core CPU option is passed through FaceScanSettings

        d->settings.faceSettings.wholeAlbums = d->settings.wholeAlbums;
        d->settings.faceSettings.useFullCpu  = d->settings.useMutiCoreCPU;
        d->settings.faceSettings.useYoloV3   = ApplicationSettings::instance()->getFaceDetectionYoloV3();
        d->settings.faceSettings.accuracy    = ApplicationSettings::instance()->getFaceDetectionAccuracy();
        d->facesDetector                     = new FacesDetector(d->settings.faceSettings);
        d->facesDetector->setNotificationEnabled(false);
        d->facesDetector->start();
    }
    else
    {
        stage7();
    }
}

void MaintenanceMngr::stage7()
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "stage7";

    if (d->settings.autotagsAssignment)
    {
        AlbumList list;
        list << d->settings.albums;
        list << d->settings.tags;

        d->autotagsAssignment = new AutotagsAssignment((AutotagsAssignment::AutotagsAssignmentScanMode)d->settings.autotaggingScanMode,
                                                       list, d->settings.modelSelectionMode, d->settings.autotagsLanguages);
        d->autotagsAssignment->setNotificationEnabled(false);
        d->autotagsAssignment->setUseMultiCoreCPU(d->settings.useMutiCoreCPU);
        d->autotagsAssignment->start();
    }
    else
    {
        stage8();
    }
}

void MaintenanceMngr::stage8()
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "stage8";

    if (d->settings.qualitySort)
    {
        AlbumList list;
        list << d->settings.albums;
        list << d->settings.tags;

        d->imageQualitySorter = new ImageQualitySorter((ImageQualitySorter::QualityScanMode)d->settings.qualityScanMode,
                                                       list, d->settings.quality);
        d->imageQualitySorter->setNotificationEnabled(false);
        d->imageQualitySorter->setUseMultiCoreCPU(d->settings.useMutiCoreCPU);
        d->imageQualitySorter->start();
    }
    else
    {
        stage9();
    }
}

void MaintenanceMngr::stage9()
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "stage9";

    if (d->settings.metadataSync)
    {
        AlbumList list;
        list << d->settings.albums;
        list << d->settings.tags;
        d->metadataSynchronizer = new MetadataSynchronizer(list, MetadataSynchronizer::SyncDirection(d->settings.syncDirection));
        d->metadataSynchronizer->setNotificationEnabled(false);

        // See Bug #329091 : Multicore CPU support with Exiv2 sound problematic, even with 0.25 release.

        d->metadataSynchronizer->setUseMultiCoreCPU(false);
        d->metadataSynchronizer->start();
    }
    else
    {
        done();
    }
}

void MaintenanceMngr::done()
{
    d->running = false;
    QTime t    = QTime::fromMSecsSinceStartOfDay(d->duration.elapsed());

    // Pop-up a message to bring user when all is done.

    DNotificationWrapper(QLatin1String("digiKam Maintenance"), // not i18n
                         i18n("All operations are done.\nDuration: %1", t.toString()),
                         qApp->activeWindow(), i18n("digiKam Maintenance"));

    Q_EMIT signalComplete();
}

void MaintenanceMngr::cancel()
{
    d->running = false;

    Q_EMIT signalComplete();
}

} // namespace Digikam

#include "moc_maintenancemngr.cpp"
