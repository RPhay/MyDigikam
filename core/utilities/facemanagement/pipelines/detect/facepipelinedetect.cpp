/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-11-10
 * Description : Performs face detection and recognition
 *
 * SPDX-FileCopyrightText: 2024-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "facepipelinedetect.h"

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

namespace Digikam
{

FacePipelineDetect::FacePipelineDetect(const FaceScanSettings& _settings)
    : FacePipelineBase(_settings)
{
}

FacePipelineDetect::~FacePipelineDetect()
{
    if (detector)
    {
        delete detector;
    }
}

bool FacePipelineDetect::start()
{
    // create the face detector

    try
    {
        detector = new FaceDetector;
    }

    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        detector = nullptr;
    }

    catch (...)
    {
        detector = nullptr;
    }

    // check if the detector was created

    if (nullptr == detector)
    {
        return false;
    }

    // set the detector parameters

    QVariantMap params;
    params[QLatin1String("detectAccuracy")]       = settings.detectAccuracy;
    params[QLatin1String("detectModel")]          = settings.detectModel;
    params[QLatin1String("detectSize")]           = settings.detectSize;
    params[QLatin1String("recognizeAccuracy")]    = settings.recognizeAccuracy;
    params[QLatin1String("recognizeModel")]       = settings.recognizeModel;
    detector->setParameters(params);

    {
        // use the mutex to synchronize the start of the threads

        QMutexLocker lock(&mutex);

        // add the worker threads for this pipeline

        addWorker(MLPipelineStage::Finder);
        addWorker(MLPipelineStage::Loader);
        addWorker(MLPipelineStage::Extractor);
        // addWorker(MLPipelineStage::Classifier);    // no classifier in detection
        addWorker(MLPipelineStage::Writer);
    }

    return FacePipelineBase::start();
}

bool FacePipelineDetect::finder()
{
    // All threads start with the same basic functions

    MLPipelineQueue* thisQueue = nullptr, *nextQueue = nullptr;
    stageStart(QThread::LowPriority, MLPipelineStage::Finder, MLPipelineStage::Loader, thisQueue, nextQueue);
    QElapsedTimer timer;

    //--------------------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////////////////////////
    // start pipeline stage specific code

    bool moreCpu = false;

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

            QList<qlonglong> imageIds = CoreDbAccess().db()->getImageIds(album->id(),
                                                                         DatabaseItem::Status::Visible,
                                                                         (FaceScanSettings::AlreadyScannedHandling::Skip != settings.alreadyScannedHandling));

            // quick check if we should add threads.

            if (!moreCpu)
            {
                moreCpu = checkMoreWorkers(totalItemCount, imageIds.size(), settings.useFullCpu);
            }

            // iterate over the image IDs and add unique IDs to the queue for processing

            for (qlonglong imageId : std::as_const(imageIds))
            {
                // filter out duplicate image IDs

                if (!filter.contains(imageId))
                {
                    ++totalItemCount;
                    filter << imageId;
                    enqueue(nextQueue, new FacePipelinePackageBase(imageId));
                }
            }
        }
    }

    // update the progress bar with the new number of items to process

    Q_EMIT signalUpdateItemCount(totalItemCount);

    // end pipeline stage specific code
    //////////////////////////////////////////////////////////////////////////////////////////////

    pipelinePerformanceEnd(MLPipelineStage::Finder, totalItemCount, timer);

    //--------------------------------------------------------------------------------
    // all threads end with the same basic functions

    stageEnd(MLPipelineStage::Finder, MLPipelineStage::Loader);

    return true;
}

bool FacePipelineDetect::loader()
{
    // All threads start with the same basic functions

    MLPipelineQueue* thisQueue = nullptr, *nextQueue = nullptr;
    stageStart(QThread::LowPriority, MLPipelineStage::Loader, MLPipelineStage::Extractor, thisQueue, nextQueue);
    FacePipelinePackageBase* package = nullptr;
    QElapsedTimer timer;

    //--------------------------------------------------------------------------------

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

            pipelinePerformanceStart(MLPipelineStage::Loader, timer);

            //////////////////////////////////////////////////////////////////////////////////////////////
            // start pipeline stage specific code

            // check if the ID is for an image (not video or other file type)

            if (DatabaseItem::Category::Image == package->info.category())
            {
                // load high quality image for detection

                package->image = PreviewLoadThread::loadHighQualitySynchronously(package->info.filePath());

                qCDebug(DIGIKAM_FACESENGINE_LOG) << "Image format:" << package->image.format() << " color depth:" << package->image.bitsDepth() << "  filename:" << package->info.filePath();

                // send to the next stage

                enqueue(nextQueue, package);
            }
            else
            {
                // send a notification that the file was skipped

                notify(MLPipelineNotification::notifySkipped,
                       package->info.name(),
                       package->info.filePath(),
                       package->faceRects.size(),
                       DImg());

                // delete the package since it is not needed

                delete package;
            }

            // end pipeline stage specific code
            //////////////////////////////////////////////////////////////////////////////////////////////

            pipelinePerformanceEnd(Loader, timer);
        }

        catch (const std::exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "FacePipelineDetect::loader(): unknown error. "
                                                << e.what() << " Restarting...";

            if (package)
            {
                delete package;
            }
        }

        catch (...)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "FacePipelineDetect::loader(): unknown error. Restarting...";

            if (package)
            {
                delete package;
            }
        }
    }

    //--------------------------------------------------------------------------------
    // all threads end with the same basic functions

    stageEnd(MLPipelineStage::Loader, MLPipelineStage::Extractor);

    return true;
}

bool FacePipelineDetect::extractor()
{
    // All threads start with the same basic functions

    MLPipelineQueue* thisQueue = nullptr, *nextQueue = nullptr;
    stageStart(QThread::NormalPriority, MLPipelineStage::Extractor, MLPipelineStage::Writer, thisQueue, nextQueue);
    FacePipelinePackageBase* package = nullptr;
    QElapsedTimer timer;

    //--------------------------------------------------------------------------------

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

            pipelinePerformanceStart(MLPipelineStage::Extractor, timer);

            //////////////////////////////////////////////////////////////////////////////////////////////
            // start pipeline stage specific code

            // convert the DImg to cv::UMat

            // create 48x48 thumbnail for notification

            // pass UMat to detector

            package->faceRects = detector->detectFaces(package->image);

            enqueue(nextQueue, package);

            // end pipeline stage specific code
            //////////////////////////////////////////////////////////////////////////////////////////////

            pipelinePerformanceEnd(Extractor, timer);
        }

        catch (const std::exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "FacePipelineDetect::extractor(): unknown extractor error. "
                                                << e.what() << " Restarting...";

            if (package)
            {
                delete package;
            }
        }

        catch (...)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "FacePipelineDetect::extractor(): unknown extractor error. Restarting...";

            if (package)
            {
                delete package;
            }
        }
    }

    //--------------------------------------------------------------------------------
    // all threads end with the same basic functions

    stageEnd(MLPipelineStage::Extractor, MLPipelineStage::Writer);

    return true;
}

bool FacePipelineDetect::writer()
{
    // All threads start with the same basic functions

    MLPipelineQueue* thisQueue = nullptr, *nextQueue = nullptr;
    stageStart(QThread::LowPriority, MLPipelineStage::Writer, MLPipelineStage::None, thisQueue, nextQueue);
    FacePipelinePackageBase* package = nullptr;
    QElapsedTimer timer;

    //--------------------------------------------------------------------------------

    FaceUtils utils;
    ThumbnailLoadThread* const thumbnailLoadThread = ThumbnailLoadThread::defaultThread();

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

            pipelinePerformanceStart(MLPipelineStage::Writer, timer);

            //////////////////////////////////////////////////////////////////////////////////////////////
            // start pipeline stage specific code

            switch (settings.alreadyScannedHandling)
            {
                case FaceScanSettings::Rescan:
                {
                    // remove old unconfirmed face rects

                    QList<FaceTagsIface> oldEntries = utils.unconfirmedFaceTagsIfaces(package->info.id());
                    utils.removeFaces(oldEntries);

                    break;
                }

                case FaceScanSettings::ClearAll:
                {
                    // remove all face rects

                    utils.removeAllFaces(package->info.id());

                    break;
                }

                // case FaceScanSettings::Merge:
                // {
                //     // Filter out similar rects
                //     break;
                // }
                case FaceScanSettings::RecognizeOnly:
                case FaceScanSettings::Skip:
                {
                    // do nothing
                    // Skipped images were skipped in the finder stage
                    // RecognizeOnly defines the pipeline

                    break;
                }
            }

            // mark the image as scanned

            utils.markAsScanned(package->info);

            // write the new face rects to the database

            if (package->faceRects.size())
            {
                QList<FaceTagsIface> databaseFaces = utils.writeUnconfirmedResults(package->info.id(),
                                                                                   package->faceRects,
                                                                                   QList<Identity>(),
                                                                                   package->image.originalSize());

                // store the thumbnails

                if (!package->image.isNull())
                {
                    utils.storeThumbnails(thumbnailLoadThread, package->info.filePath(),
                                          databaseFaces, package->image);
                }

            }

            // send a notification that the image was processed

            notify(MLPipelineNotification::notifyProcessed,
                   package->info.name(),
                   package->info.filePath(),
                   package->faceRects.size(),
                   package->image);

            // delete the package

            delete package;

            // end pipeline stage specific code
            //////////////////////////////////////////////////////////////////////////////////////////////

            pipelinePerformanceEnd(MLPipelineStage::Writer, timer);
        }

        catch (const std::exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "FacePipelineDetect::writer(): unknown writer error. "
                                                << e.what() << " Restarting...";

            if (package)
            {
                delete package;
            }
        }

        catch (...)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "FacePipelineDetect::writer(): unknown writer error. Restarting...";

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

void FacePipelineDetect::addMoreWorkers()
{
    // use the performanceProfile metrics to find the slowest stages
    // and add more workers to those stages

    // for the detection pipeline, the loader is the slowest stage
    // so add 3 more loaders and 1 more extractor

    addWorker(Loader);
    addWorker(Loader);
    addWorker(Loader);
    addWorker(Extractor);
}

} // namespace Digikam

#include "moc_facepipelinedetect.cpp"
