/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Integrated, multithread face detection / recognition
 *
 * SPDX-FileCopyrightText: 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "facepipeline_p.h"

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "facepreviewloader.h"
#include "parallelpipes.h"
#include "scanstatefilter.h"

namespace Digikam
{

FacePipeline::Private::Private(FacePipeline* const qq)
    : q(qq)
{
}

void FacePipeline::Private::processBatch(const QList<ItemInfo>& infos)
{
    if (databaseFilter)
    {
        infosForFiltering += infos.size();
        databaseFilter->process(infos);
    }
    else
    {
        for (const ItemInfo& info : std::as_const(infos))
        {
            send(buildPackage(info));
        }
    }
}

/// called by filter
void FacePipeline::Private::sendFromFilter(const QList<FacePipelineExtendedPackage::Ptr>& packages)
{
    infosForFiltering -= packages.size();

    for (const FacePipelineExtendedPackage::Ptr& package : std::as_const(packages))
    {
        send(package);
    }
}

/// called by filter
void FacePipeline::Private::skipFromFilter(const QList<ItemInfo>& infosForSkipping)
{
    infosForFiltering -= infosForSkipping.size();

    Q_EMIT q->skipped(infosForSkipping);

    // everything skipped?

    checkFinished();
}

FacePipelineExtendedPackage::Ptr FacePipeline::Private::filterOrBuildPackage(const ItemInfo& info)
{
    if (databaseFilter)
    {
        return databaseFilter->filter(info);
    }
    else
    {
        return buildPackage(info);
    }
}

FacePipelineExtendedPackage::Ptr FacePipeline::Private::buildPackage(const ItemInfo& info)
{
    FacePipelineExtendedPackage::Ptr package(new FacePipelineExtendedPackage);
    package->info     = info;
    package->filePath = info.filePath();

    return package;
}

FacePipelineExtendedPackage::Ptr FacePipeline::Private::buildPackage(const ItemInfo& info,
                                                                     const FacePipelineFaceTagsIface& face,
                                                                     const DImg& image)
{
    FacePipelineFaceTagsIfaceList faces;
    faces << face;

    return buildPackage(info, faces, image);
}

FacePipelineExtendedPackage::Ptr FacePipeline::Private::buildPackage(const ItemInfo& info,
                                                                     const FacePipelineFaceTagsIfaceList& faces,
                                                                     const DImg& image)
{
    FacePipelineExtendedPackage::Ptr package = buildPackage(info);
    package->databaseFaces                   = faces;
    package->image                           = image;

    return package;
}

void FacePipeline::Private::send(const FacePipelineExtendedPackage::Ptr& package)
{
    start();
    ++totalPackagesAdded;

    Q_EMIT q->processing(*package);

    if (senderFlowControl(package))
    {
        ++packagesOnTheRoad;

        Q_EMIT startProcess(package);
    }
}

void FacePipeline::Private::finishProcess(FacePipelineExtendedPackage::Ptr package)
{
    --packagesOnTheRoad;

    Q_EMIT q->processed(*package);
    Q_EMIT q->progressValueChanged(1.0F - (float(packagesOnTheRoad + delayedPackages.size()) / totalPackagesAdded));

    package = nullptr;

    if (previewThread)
    {
        previewThread->checkRestart();
    }

    receiverFlowControl();

    checkFinished();
}

bool FacePipeline::Private::senderFlowControl(const FacePipelineExtendedPackage::Ptr& package)
{
    if (packagesOnTheRoad > maxPackagesOnTheRoad)
    {
        delayedPackages << package;

        return false;
    }

    return true;
}

void FacePipeline::Private::receiverFlowControl()
{
    if (!delayedPackages.isEmpty() && packagesOnTheRoad <= maxPackagesOnTheRoad)
    {
        --totalPackagesAdded; // do not add twice
        send(delayedPackages.takeFirst());
    }
}

bool FacePipeline::Private::hasFinished() const
{
    return !packagesOnTheRoad && !infosForFiltering;
}

void FacePipeline::Private::checkFinished()
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "Check for finish: " << packagesOnTheRoad << "packages,"
                                 << infosForFiltering << "infos to filter, hasFinished()" << hasFinished();

    if (hasFinished())
    {
        totalPackagesAdded = 0;

        Q_EMIT q->finished();

        // stop threads

        stop();
    }
}

void FacePipeline::Private::start()
{
    if (started)
    {
        return;
    }

    Q_EMIT q->scheduled();

    WorkerObject*  workerObject = nullptr;
    ParallelPipes* pipes        = nullptr;

    for (QObject* const element : std::as_const(pipeline))
    {
        if      ((workerObject = qobject_cast<WorkerObject*>(element)))
        {
            workerObject->schedule();
        }
        else if ((pipes = qobject_cast<ParallelPipes*>(element)))
        {
            pipes->schedule();
        }
    }

    started = true;
    waiting = false;

    Q_EMIT q->started(i18n("Applying face changes"));
}

void FacePipeline::Private::stop()
{
    if (!started)
    {
        return;
    }

    if (previewThread)
    {
        previewThread->cancel();
    }

    for (ThumbnailLoadThread* const thread : std::as_const(thumbnailLoadThreads))
    {
        thread->stopAllTasks();
    }

    WorkerObject* workerObject = nullptr;
    ParallelPipes* pipes       = nullptr;
    DynamicThread* thread      = nullptr;

    for (QObject* const element : std::as_const(pipeline))
    {
        if      ((workerObject = qobject_cast<WorkerObject*>(element)))
        {
            workerObject->deactivate();
        }
        else if ((pipes = qobject_cast<ParallelPipes*>(element)))
        {
            pipes->deactivate();
        }
        else if ((thread = qobject_cast<DynamicThread*>(element)))
        {
            thread->stop();
        }
    }

    started = false;
    waiting = true;
}

void FacePipeline::Private::wait()
{
    if (!waiting)
    {
        return;
    }

    if (previewThread)
    {
        previewThread->wait();
    }

    for (ThumbnailLoadThread* const thread : std::as_const(thumbnailLoadThreads))
    {
        thread->wait();
    }

    WorkerObject* workerObject = nullptr;
    ParallelPipes* pipes       = nullptr;
    DynamicThread* thread      = nullptr;

    for (QObject* const element : std::as_const(pipeline))
    {
        if      ((workerObject = qobject_cast<WorkerObject*>(element)))
        {
            workerObject->wait();
        }
        else if ((pipes = qobject_cast<ParallelPipes*>(element)))
        {
            pipes->wait();
        }
        else if ((thread = qobject_cast<DynamicThread*>(element)))
        {
            thread->wait();
        }
    }

    waiting = false;
}

void FacePipeline::Private::applyPriority()
{
    WorkerObject*  workerObject = nullptr;
    ParallelPipes* pipes        = nullptr;

    for (QObject* const element : std::as_const(pipeline))
    {
        if      ((workerObject = qobject_cast<WorkerObject*>(element)))
        {
            workerObject->setPriority(priority);
        }
        else if ((pipes = qobject_cast<ParallelPipes*>(element)))
        {
            pipes->setPriority(priority);
        }
    }

    for (ThumbnailLoadThread* const thread : std::as_const(thumbnailLoadThreads))
    {
        thread->setPriority(priority);
    }
}

ThumbnailLoadThread* FacePipeline::Private::createThumbnailLoadThread()
{
    ThumbnailLoadThread* const thumbnailLoadThread = new ThumbnailLoadThread;
    thumbnailLoadThread->setPixmapRequested(false);
    thumbnailLoadThread->setThumbnailSize(ThumbnailLoadThread::maximumThumbnailSize());

    // Image::recommendedSizeForRecognition()

    thumbnailLoadThread->setPriority(priority);

    thumbnailLoadThreads << thumbnailLoadThread;

    return thumbnailLoadThread;
}

} // namespace Digikam

#include "moc_facepipeline_p.cpp"
