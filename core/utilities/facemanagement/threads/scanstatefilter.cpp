/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Integrated, multithread face detection / recognition
 *
 * SPDX-FileCopyrightText: 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "scanstatefilter.h"

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

ScanStateFilter::ScanStateFilter(FacePipeline::FilterMode fmode, FacePipeline::Private* const dd)
    : d   (dd),
      mode(fmode)
{
    connect(this, SIGNAL(signalInfosToDispatch()),
            this, SLOT(dispatch()));
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

            for (const ItemInfo& info : std::as_const(todo))
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

            Q_EMIT signalInfosToDispatch();
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

#include "moc_scanstatefilter.cpp"
