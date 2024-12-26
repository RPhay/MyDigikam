/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-11-10
 * Description : Performs face detection and recognition
 *
 * SPDX-FileCopyrightText: 2024      by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024      by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "facepipelinerecognize.h"

// Qt includes

#include <QList>
#include <QSet>
#include <QElapsedTimer>
#include <QRectF>

// Local includes

#include "digikam_debug.h"
#include "digikam_opencv.h"
#include "sharedqueue.h"
#include "album.h"
#include "iteminfo.h"
#include "coredb.h"
#include "facescansettings.h"
#include "dimg.h"
#include "previewloadthread.h"
#include "faceutils.h"
#include "facepipelinepackagebase.h"
#include "thumbnailloadthread.h"
#include "identityprovider.h"
#include "identity.h"
#include "dnnsfaceextractor.h"

namespace Digikam
{

FacePipelineRecognize::FacePipelineRecognize(const FaceScanSettings& _settings)
    : FacePipelineBase(_settings)
{
}

FacePipelineRecognize::~FacePipelineRecognize()
{
}

bool FacePipelineRecognize::start()
{
    QVariantMap params;
    params[QLatin1String("detectAccuracy")]       = settings.detectAccuracy;
    params[QLatin1String("detectModel")]          = settings.detectModel;
    params[QLatin1String("detectSize")]           = settings.detectSize;
    params[QLatin1String("recognizeAccuracy")]    = settings.recognizeAccuracy;
    params[QLatin1String("recognizeModel")]       = settings.recognizeModel;
    detector.setParameters(params);

    {
        // use the mutex to synchronize the start of the threads

        QMutexLocker lock(&mutex);

        // add the worker threads for this pipeline

        addWorker(MLPipelineStage::Finder);
        addWorker(MLPipelineStage::Loader);
        addWorker(MLPipelineStage::Extractor);
        addWorker(MLPipelineStage::Classifier);
        addWorker(MLPipelineStage::Writer);
    }

    return FacePipelineBase::start();
}

void FacePipelineRecognize::cancel()
{
    FacePipelineBase::cancel();
}

bool FacePipelineRecognize::finder()
{
    // All threads start with the same basic functions

    MLPipelineQueue* thisQueue = nullptr, *nextQueue = nullptr;
    stageStart(QThread::LowPriority, MLPipelineStage::Finder, MLPipelineStage::Loader, thisQueue, nextQueue);
    QElapsedTimer timer;

    //--------------------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////////////////////////
    // start pipeline stage specific code

    bool moreCpu = false;
    FaceUtils utils;

    timer.start();

    // get the IDs to process

    QSet<qlonglong> filter;

    for (const Album* const album : std::as_const(settings.albums))
    {
        if (cancelled)
        {
            break;
        }

        if (!album->isTrashAlbum())
        {
            // get the image IDs for the album

            QList<qlonglong> imageIds = CoreDbAccess().db()->getImageIds(album->id(), DatabaseItem::Status::Visible, true);

            // quick check if we should add threads.

            if (!moreCpu)
            {
                moreCpu = checkMoreWorkers(totalItemCount, imageIds.size(), settings.useFullCpu);
            }

            // iterate over the image IDs and add unique IDs to the queue for processing

            for (qlonglong imageId : std::as_const(imageIds))
            {
                ++performanceProfileList[MLPipelineStage::Finder].itemCount;

                // filter out duplicate image IDs

                if (!filter.contains(imageId))
                {
                    QList<FaceTagsIface> faces = utils.unconfirmedFaceTagsIfaces(imageId);

                    for (const FaceTagsIface& face : std::as_const(faces))
                    {
                        ++totalItemCount;
                        filter << imageId;
                        enqueue(nextQueue, new FacePipelinePackageBase(imageId, face));
                    }
                }
            }
        }
    }

    for (const ItemInfo& info : std::as_const(settings.infos))
    {
        ++performanceProfileList[MLPipelineStage::Finder].itemCount;

        // filter out duplicate image IDs

        qlonglong imageId = info.id();

        if (!filter.contains(imageId))
        {
            QList<FaceTagsIface> faces = utils.unconfirmedFaceTagsIfaces(imageId);

            for (const FaceTagsIface& face : std::as_const(faces))
            {
                ++totalItemCount;
                filter << imageId;
                enqueue(nextQueue, new FacePipelinePackageBase(imageId, face));
            }
        }
    }

    // update the progress bar with the new number of items to process

    Q_EMIT signalUpdateItemCount(totalItemCount);

    // end pipeline stage specific code
    //////////////////////////////////////////////////////////////////////////////////////////////

    performanceProfileList[MLPipelineStage::Finder].itemCount   = totalItemCount;
    performanceProfileList[MLPipelineStage::Finder].elapsedTime = timer.elapsed();

    //--------------------------------------------------------------------------------
    // all threads end with the same basic functions

    stageEnd(MLPipelineStage::Finder, MLPipelineStage::Loader);

    return true;
}

bool FacePipelineRecognize::loader()
{
    return commonFaceThumbnailLoader(QStringLiteral("FacePipelineRecognize"), MLPipelineStage::Loader, MLPipelineStage::Extractor);
}

bool FacePipelineRecognize::extractor()
{
    return commonFaceThumbnailExtractor(QStringLiteral("FacePipelineRecognize"), MLPipelineStage::Extractor, MLPipelineStage::Classifier);
}

bool FacePipelineRecognize::classifier()
{
    // All threads start with the same basic functions

    MLPipelineQueue* thisQueue = nullptr, *nextQueue = nullptr;
    stageStart(QThread::LowPriority, MLPipelineStage::Classifier, MLPipelineStage::Writer, thisQueue, nextQueue);
    FacePipelinePackageBase* package = nullptr;
    QElapsedTimer timer;

    //--------------------------------------------------------------------------------

    FaceClassifier* const classifier = FaceClassifier::instance();
    classifier->setParameters(settings);

    while (!cancelled)
    {
        package = nullptr;

        try
        {
            package = static_cast<FacePipelinePackageBase*>(dequeue(thisQueue));

            if (queueEndSignal() == package)
            {
                // end of queue signal

                break;
            }

            performanceProfileList[MLPipelineStage::Classifier].maxQueueCount = qMax(performanceProfileList[MLPipelineStage::Classifier].maxQueueCount, thisQueue->size());
            ++performanceProfileList[MLPipelineStage::Classifier].itemCount;

            timer.start();

            //////////////////////////////////////////////////////////////////////////////////////////////
            // start pipeline stage specific code

            // verify the feature mat is not empty

            if (0 != package->features.rows)
            {
                // classify the features

                package->label = classifier->predict(package->features);
            }

            // -1 means no match suggested
            // pass the package to the next stage if we have a suggestion

            if (-1 != package->label)
            {
                enqueue(nextQueue, package);
            }
            else
            {
                // no suggested match found, so notify the user

                notify(MLPipelineNotification::notifyProcessed, package->info.name(), package->info.filePath(), 1, package->thumbnail);

                // delete the package

                delete package;
            }

            // end pipeline stage specific code
            //////////////////////////////////////////////////////////////////////////////////////////////

            performanceProfileList[MLPipelineStage::Classifier].elapsedTime   += timer.elapsed();
            performanceProfileList[MLPipelineStage::Classifier].maxElapsedTime = qMax((qint64)performanceProfileList[MLPipelineStage::Classifier].maxElapsedTime, timer.elapsed());
        }

        catch (const std::exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "FacePipelineRecognize::classifier(): unknown error. " << e.what() << "    Restarting...";

            if (package)
            {
                delete package;
            }
        }

        catch (...)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "FacePipelineRecognize::classifier(): unknown error.  Restarting...";

            if (package)
            {
                delete package;
            }
        }
    }

    //--------------------------------------------------------------------------------
    // all threads end with the same basic functions

    stageEnd(MLPipelineStage::Classifier, MLPipelineStage::Writer);

    return true;
}

bool FacePipelineRecognize::writer()
{
    // All threads start with the same basic functions

    MLPipelineQueue* thisQueue = nullptr, *nextQueue = nullptr;
    stageStart(QThread::LowPriority, MLPipelineStage::Writer, MLPipelineStage::None, thisQueue, nextQueue);
    FacePipelinePackageBase* package = nullptr;
    QElapsedTimer timer;

    //--------------------------------------------------------------------------------

    FaceUtils utils;
    IdentityProvider* const idProvider = IdentityProvider::instance();

    while (!cancelled)
    {
        package = nullptr;

        try
        {
            package = static_cast<FacePipelinePackageBase*>(dequeue(thisQueue));

            if (queueEndSignal() == package)
            {
                // end of queue signal

                break;
            }

            performanceProfileList[MLPipelineStage::Writer].maxQueueCount = qMax(performanceProfileList[MLPipelineStage::Writer].maxQueueCount, thisQueue->size());
            ++performanceProfileList[MLPipelineStage::Writer].itemCount;

            timer.start();

            //////////////////////////////////////////////////////////////////////////////////////////////
            // start pipeline stage specific code

            if (-1 != package->label)
            {
                Identity identity = idProvider->identity(package->label);
                int tagId         = FaceTags::getOrCreateTagForIdentity(identity.attributesMap());
                utils.changeSuggestedName(package->face, tagId);
            }

            // send a notification that the image was processed

            notify(MLPipelineNotification::notifyProcessed, package->info.name(), package->info.filePath(), 1, package->thumbnail);

            // delete the package

            delete package;

            // end pipeline stage specific code
            //////////////////////////////////////////////////////////////////////////////////////////////

            performanceProfileList[MLPipelineStage::Writer].elapsedTime   += timer.elapsed();
            performanceProfileList[MLPipelineStage::Writer].maxElapsedTime = qMax((qint64)performanceProfileList[MLPipelineStage::Writer].maxElapsedTime, timer.elapsed());
        }

        catch (const std::exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "FacePipelineRecognize::writer(): unknown error. " << e.what() << "  Restarting...";

            if (package)
            {
                delete package;
            }
        }

        catch (...)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "FacePipelineRecognize::writer(): unknown error.  Restarting...";

            if (package)
            {
                delete package;
            }
        }
    }

    //--------------------------------------------------------------------------------
    // all threads end with the same basic functions

    stageEnd(MLPipelineStage::Writer, MLPipelineStage::None);

    return true;
}

void FacePipelineRecognize::addMoreWorkers()
{
    // use the performanceProfile metrics to find the slowest stages
    // and add more workers to those stages

    // for the recognition pipeline, the extractor is the slowest stage
    // so add 1 more loader and 2 more extractors

    addWorker(Loader);
    addWorker(Extractor);
    addWorker(Extractor);
}

} // namespace Digikam

#include "moc_facepipelinerecognize.cpp"
