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

    explicit Private()
      : mode     (AutotagsAssignmentScanMode::NonAssignedItems),
        modelType(DetectorModel::YOLOV5NANO),
        thread   (nullptr)
    {
    }

    AutotagsAssignmentScanMode mode;

    QStringList                allPicturesPath;

    AlbumList                  albumList;

    MaintenanceThread*         thread;

    int                        modelType;
};

AutotagsAssignment::AutotagsAssignment(AutotagsAssignmentScanMode mode,
                                       const AlbumList& list,
                                       int modelType,
                                       ProgressItem* const parent)
    : MaintenanceTool(QLatin1String("AutotagsAssignment"), parent),
      d              (new Private)
{
    d->mode      = mode;
    d->albumList = list;
    d->modelType = modelType;
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
    // any path containing root Path "auto" as "auto/car", "auto/bus", ...

    QList<QString> predTagPaths = AutoTagsAssign().getPredefinedTagsPath();
    QStringList assignedItems;

    for (auto path : predTagPaths)
    {
        assignedItems += CoreDbAccess().db()->getItemsURLsWithTag(TagsCache::instance()->tagForPath(path));
    }

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
            aPaths = CoreDbAccess().db()->getItemURLsInAlbum((*it)->id());
        }
        else if ((*it)->type() == Album::TAG)
        {
            aPaths = CoreDbAccess().db()->getItemURLsInTag((*it)->id());
        }

        if (d->mode == NonAssignedItems)
        {
            Q_FOREACH (const QString& path, aPaths)
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

    d->thread->generateTags(d->allPicturesPath, d->modelType);
    d->thread->start();
}

void AutotagsAssignment::slotAdvance(const QImage& img)
{
    setThumbnail(QPixmap::fromImage(img));
    advance(1);
}

} // namespace Digikam
