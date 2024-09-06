/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-09-02
 * Description : Autotags Assignment maintenance tool
 *
 * SPDX-FileCopyrightText: 2023 by Quoc Hung Tran <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "autotagsassignment.h"

// Qt includes

#include <QString>
#include <QIcon>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_globals.h"
#include "dimg.h"
#include "coredb.h"
#include "albummanager.h"
#include "coredbaccess.h"
#include "tagscache.h"
#include "maintenancethread.h"
#include "autotagsassign.h"

namespace Digikam
{

class Q_DECL_HIDDEN AutotagsAssignment::Private
{
public:

    Private() = default;

    AutotagsAssignmentScanMode mode            = AutotagsAssignmentScanMode::NonAssignedItems;

    QStringList                allPicturesPath;
    QStringList                languages;

    AlbumList                  albumList;

    MaintenanceThread*         thread          = nullptr;

    int                        modelType       = DetectorModel::YOLOV5NANO;
};

AutotagsAssignment::AutotagsAssignment(AutotagsAssignmentScanMode mode,
                                       const AlbumList& list,
                                       int modelType,
                                       const QStringList& langs,
                                       ProgressItem* const parent)
    : MaintenanceTool(QLatin1String("AutotagsAssignment"), parent),
      d              (new Private)
{
    d->mode      = mode;
    d->albumList = list;
    d->modelType = modelType;
    d->languages = langs;
    d->thread    = new MaintenanceThread(this);

    connect(d->thread, SIGNAL(signalCompleted()),
            this, SLOT(slotDone()));

    connect(d->thread, SIGNAL(signalAdvance(QImage)),
            this, SLOT(slotAdvance(QImage)));
}

AutotagsAssignment::~AutotagsAssignment()
{
    delete d;
}

void AutotagsAssignment::setUseMultiCoreCPU(bool b)
{
    d->thread->setUseMultiCore(b);
}

void AutotagsAssignment::slotCancel()
{
    d->thread->cancel();
    MaintenanceTool::slotCancel();
}

/**
 * This function is dedicated to collect all picture data paths to be ready for the started stage
 */
void AutotagsAssignment::slotStart()
{
    MaintenanceTool::slotStart();

    setLabel(i18n("Autotags Assignment"));

    ProgressManager::addProgressItem(this);

    if (d->albumList.isEmpty())
    {
        d->albumList = AlbumManager::instance()->allPAlbums();
    }

    // Get all item in DB which do not have any auto Tag assigned.
    // Any images containing root Path "auto".

    const QString rootTag     = QLatin1String("auto/");
    QStringList assignedItems = CoreDbAccess().db()->getItemURLsInTag(TagsCache::instance()->getOrCreateTag(rootTag), true);

    // Get all digiKam albums collection pictures path, depending of d->rebuildAll flag.

    for (AlbumList::ConstIterator it = d->albumList.constBegin() ;
         !canceled() && (it != d->albumList.constEnd()) ; ++it)
    {
        QStringList aPaths;

        if (!(*it))
        {
            continue;
        }

        if      ((*it)->type() == Album::PHYSICAL)
        {
            const auto pathes = CoreDbAccess().db()->getItemURLsInAlbum((*it)->id());

            for (const QString& path : pathes)
            {
                if (!aPaths.contains(path))
                {
                    aPaths << path;
                }
            }
        }
        else if ((*it)->type() == Album::TAG)
        {
            const auto pathes = CoreDbAccess().db()->getItemURLsInTag((*it)->id());

            for (const QString& path : pathes)
            {
                if (!aPaths.contains(path))
                {
                    aPaths << path;
                }
            }
        }

        if (d->mode == NonAssignedItems)
        {
            for (const QString& path : std::as_const(aPaths))
            {
                if (!assignedItems.contains(path))
                {
                    d->allPicturesPath += path;
                }
            }
        }
        else  // AllItems
        {
            d->allPicturesPath += aPaths;
        }
    }

    if (d->allPicturesPath.isEmpty())
    {
        slotDone();

        return;
    }

    setTotalItems(d->allPicturesPath.count());

    d->thread->generateTags(d->allPicturesPath, d->modelType, d->languages);
    d->thread->start();
}

void AutotagsAssignment::slotAdvance(const QImage& img)
{
    setThumbnail(QPixmap::fromImage(img));
    advance(1);
}

} // namespace Digikam

#include "moc_autotagsassignment.cpp"
