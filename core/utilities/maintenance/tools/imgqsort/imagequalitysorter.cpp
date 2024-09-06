/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-19
 * Description : image quality sorter maintenance tool
 *
 * SPDX-FileCopyrightText: 2013-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2013-2014 by Gowtham Ashok <gwty93 at gmail dot com>
 * SPDX-FileCopyrightText: 2021-2022 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "imagequalitysorter.h"

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

namespace Digikam
{

class Q_DECL_HIDDEN ImageQualitySorter::Private
{
public:

    Private() = default;

    QualityScanMode       mode      = ImageQualitySorter::NonAssignedItems;

    ImageQualityContainer quality;

    QStringList           allPicturesPath;

    AlbumList             albumList;

    MaintenanceThread*    thread    = nullptr;
};

ImageQualitySorter::ImageQualitySorter(QualityScanMode mode,
                                       const AlbumList& list,
                                       const ImageQualityContainer& quality,
                                       ProgressItem* const parent)
    : MaintenanceTool(QLatin1String("ImageQualitySorter"), parent),
      d              (new Private)
{
    d->mode       = mode;
    d->albumList  = list;
    d->quality    = quality;
    d->thread     = new MaintenanceThread(this);

    connect(d->thread, SIGNAL(signalCompleted()),
            this, SLOT(slotDone()));

    connect(d->thread, SIGNAL(signalAdvance(QImage)),
            this, SLOT(slotAdvance(QImage)));
}

ImageQualitySorter::~ImageQualitySorter()
{
    delete d;
}

void ImageQualitySorter::setUseMultiCoreCPU(bool b)
{
    d->thread->setUseMultiCore(b);
}

void ImageQualitySorter::slotCancel()
{
    d->thread->cancel();
    MaintenanceTool::slotCancel();
}

void ImageQualitySorter::slotStart()
{
    MaintenanceTool::slotStart();

    setLabel(i18n("Image Quality Sorter"));

    ProgressManager::addProgressItem(this);

    if (d->albumList.isEmpty())
    {
        d->albumList = AlbumManager::instance()->allPAlbums();
    }

    // Get all item in DB which do not have any Pick Label assigned.

    QStringList dirty = CoreDbAccess().db()->getItemsURLsWithTag(TagsCache::instance()->tagForPickLabel(NoPickLabel));

    // Get all digiKam albums collection pictures path, depending of d->rebuildAll flag.

    for (AlbumList::ConstIterator it = d->albumList.constBegin() ;
         !canceled() && (it != d->albumList.constEnd()) ; ++it)
    {
        QStringList aPaths;

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
                if (dirty.contains(path))
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

    d->thread->sortByImageQuality(d->allPicturesPath, d->quality);
    d->thread->start();
}

void ImageQualitySorter::slotAdvance(const QImage& img)
{
    setThumbnail(QIcon(QPixmap::fromImage(img)));
    advance(1);
}

} // namespace Digikam

#include "moc_imagequalitysorter.cpp"
