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

#include "facepipelinereset.h"

// Qt includes

#include <QList>
#include <QSet>
#include <QElapsedTimer>
#include <QRectF>

// local includes

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

FacePipelineReset::FacePipelineReset(const FaceScanSettings& _settings) :
                                       FacePipelineBase(_settings)
{
}

FacePipelineReset::~FacePipelineReset()
{
}

bool FacePipelineReset::start()
{
    {
        // use the mutex to synchronize the start of the threads

        QMutexLocker lock(&mutex);

        // add the worker threads for this pipeline

        addWorker(MLPipelineStage::Finder);
        // addWorker(MLPipelineStage::Loader);
        // addWorker(MLPipelineStage::Extractor);
        // addWorker(MLPipelineStage::Classifier);
        addWorker(MLPipelineStage::Writer);
    }

    return FacePipelineBase::start();
}

void FacePipelineReset::cancel()
{
    FacePipelineBase::cancel();
}

bool FacePipelineReset::finder()
{
    // All threads start with the same basic functions
    MLPipelineQueue *thisQueue = nullptr, *nextQueue = nullptr;
    stageStart(QThread::LowPriority, MLPipelineStage::Finder, MLPipelineStage::Writer, thisQueue, nextQueue);
    QElapsedTimer timer;
    //--------------------------------------------------------------------------------

    FaceUtils utils;
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
            QList<qlonglong> imageIds = CoreDbAccess().db()->getImageIds(album->id(), DatabaseItem::Status::Visible, true);
            
            if (!moreCpu && settings.useFullCpu && (totalItemCount + imageIds.size())>25 && QThread::idealThreadCount()>4)
            {
                moreCpu = true;

                int newInstances = (QThread::idealThreadCount() / 4) - 1;
                for (int i = 0; i < newInstances; ++i)
                {
                    Q_EMIT signalAddMoreWorkers();
                }
            }

            for(qlonglong imageId : std::as_const(imageIds))
            {
                // filter out duplicate image IDs

                if (!filter.contains(imageId))
                {
                    ++totalItemCount;
                    enqueue(nextQueue, new FacePipelinePackageBase(imageId));
                }
            }
        }
    }

    Q_EMIT signalUpdateItemCount(totalItemCount);    

    performanceProfileList[MLPipelineStage::Finder].itemCount = totalItemCount;
    performanceProfileList[MLPipelineStage::Finder].elapsedTime = timer.elapsed();

    //--------------------------------------------------------------------------------
    // all threads end with the same basic functions
    stageEnd(MLPipelineStage::Finder, MLPipelineStage::Writer);

    return true;
}

bool FacePipelineReset::writer()
{
    // All threads start with the same basic functions
    MLPipelineQueue *thisQueue = nullptr, *nextQueue = nullptr;
    stageStart(QThread::LowPriority, MLPipelineStage::Writer, MLPipelineStage::None, thisQueue, nextQueue);
    FacePipelinePackageBase* package = nullptr;
    QElapsedTimer timer;
    //--------------------------------------------------------------------------------

    FaceUtils utils;
    FaceTagsEditor editor;
    IdentityProvider* idProvider = IdentityProvider::instance();
    QIcon icon = QIcon::fromTheme(QLatin1String("person"));

    // clear all identites and training from recognition DB before
    // we start looping through the results from the pipeline

    idProvider->clearAllTraining();

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

            // remove the face tags from the image

            QList<FaceTagsIface> databaseFaces = editor.unconfirmedFaceTagsIfaces(package->info.id());

            editor.removeFaces(databaseFaces);

            databaseFaces = editor.unconfirmedNameFaceTagsIfaces(package->info.id());

            editor.removeFaces(databaseFaces);

            editor.removeAllFaces(package->info.id());

            // mark the image as unscanned

            utils.markAsScanned(package->info, false);

            // send a notification that the image was processed

            notify(MLPipelineNotification::notifyProcessed, package->info.name(), package->info.filePath(), package->faceRects.size(), icon);

            // delete the package

            delete package;

            performanceProfileList[MLPipelineStage::Writer].elapsedTime += timer.elapsed();
            performanceProfileList[MLPipelineStage::Writer].maxElapsedTime = qMax(performanceProfileList[MLPipelineStage::Writer].maxElapsedTime, timer.elapsed());
        }
        catch(const std::exception& e)
        {
            qCDebug(DIGIKAM_FACESENGINE_LOG) << "FacePipelineReset::writer(): unknown error.  Restarting...";
            std::cerr << e.what() << '\n';
        }
        catch(...)
        {
            qCDebug(DIGIKAM_FACESENGINE_LOG) << "FacePipelineReset::writer(): unknown error.  Restarting...";
            if (package)
            {
                delete package;
            }
        }        
    }

    // retrain the classifier after all results have been processed
    FaceClassifier::instance()->retrain();

    //--------------------------------------------------------------------------------
    // all threads end with the same basic functions
    stageEnd(MLPipelineStage::Writer, MLPipelineStage::None);

    return true;
}

void FacePipelineReset::addMoreWorkers()
{
    // reset pipeline is always single thread per stage
}

}

#include "moc_facepipelinereset.cpp"
