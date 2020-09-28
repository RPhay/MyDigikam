/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Integrated, multithread face detection / recognition
 *
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "scanstatefilter.h"

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

ScanStateFilter::ScanStateFilter(FacePipeline::FilterMode mode, FacePipeline::Private* const d)
    : d     (d),
      mode  (mode)
{
    connect(this, SIGNAL(infosToDispatch()),
            this, SLOT(dispatch()));
}

ScanStateFilter::~ScanStateFilter()
{
}

FacePipelineExtendedPackage::Ptr ScanStateFilter::filter(const ItemInfo& info)
{
    FaceUtils utils;

    switch (mode)
    {
        case FacePipeline::ScanAll:
        {
            return d->buildPackage(info);
        }

        case FacePipeline::SkipAlreadyScanned:
        {
            if (!utils.hasBeenScanned(info))
            {
                return d->buildPackage(info);
            }

            break;
        }

        case FacePipeline::ReadUnconfirmedFaces:
        case FacePipeline::ReadFacesForTraining:
        case FacePipeline::ReadConfirmedFaces:
        {
            QList<FaceTagsIface> databaseFaces;

            if      (mode == FacePipeline::ReadUnconfirmedFaces)
            {
                databaseFaces = utils.unconfirmedFaceTagsIfaces(info.id());

            }
            else if (mode == FacePipeline::ReadFacesForTraining)
            {
                databaseFaces = utils.databaseFacesForTraining(info.id());
            }
            else
            {
                databaseFaces = utils.confirmedFaceTagsIfaces(info.id());
            }

            if (!databaseFaces.isEmpty())
            {
                FacePipelineExtendedPackage::Ptr package = d->buildPackage(info);
                package->databaseFaces                   = databaseFaces;
/*
                qCDebug(DIGIKAM_GENERAL_LOG) << "Prepared package with" << databaseFaces.size();
*/
                package->databaseFaces.setRole(FacePipelineFaceTagsIface::ReadFromDatabase);

                if (tasks)
                {
                    package->databaseFaces.setRole(tasks);
                }

                return package;
            }

            break;
        }
    }

    return FacePipelineExtendedPackage::Ptr();
}

void ScanStateFilter::process(const QList<ItemInfo>& infos)
{
    QMutexLocker lock(threadMutex());
    toFilter << infos;
/*
    qCDebug(DIGIKAM_GENERAL_LOG) << "Received" << infos.size() << "images for filtering";
*/
    start(lock);
}

void ScanStateFilter::process(const ItemInfo& info)
{
    QMutexLocker lock(threadMutex());
    toFilter << info;
    start(lock);
}

void ScanStateFilter::run()
{
    while (runningFlag())
    {
        // get todo list

        QList<ItemInfo> todo;
        {
            QMutexLocker lock(threadMutex());

            if (!toFilter.isEmpty())
            {
                todo = toFilter;
                toFilter.clear();
            }
            else
            {
                stop(lock);
            }
        }

        // process list

        if (!todo.isEmpty())
        {
            QList<FacePipelineExtendedPackage::Ptr> itemsToSend;
            QList<ItemInfo>                         itemsToSkip;

            foreach (const ItemInfo& info, todo)
            {
                FacePipelineExtendedPackage::Ptr package = filter(info);

                if (package)
                {
                    itemsToSend << package;
                }
                else
                {
                    itemsToSkip << info;
                }
            }
/*
            qCDebug(DIGIKAM_GENERAL_LOG) << "Filtered" << todo.size() << "images, send" << send.size() << "skip" << skip.size();
*/
            {
                QMutexLocker lock(threadMutex());
                toSend      << itemsToSend;
                toBeSkipped << itemsToSkip;
            }

            emit infosToDispatch();
        }
    }
}

void ScanStateFilter::dispatch()
{
    QList<FacePipelineExtendedPackage::Ptr> itemsToSend;
    QList<ItemInfo>                         itemsToSkip;

    {
        QMutexLocker lock(threadMutex());
        itemsToSend = toSend;
        toSend.clear();
        itemsToSkip = toBeSkipped;
        toBeSkipped.clear();
    }
/*
    qCDebug(DIGIKAM_GENERAL_LOG) << "Dispatching, sending" << send.size() << "skipping" << skip.size();
*/
    if (!itemsToSkip.isEmpty())
    {
        d->skipFromFilter(itemsToSkip);
    }

    if (!itemsToSend.isEmpty())
    {
        d->sendFromFilter(itemsToSend);
    }
}

} // namespace Digikam
