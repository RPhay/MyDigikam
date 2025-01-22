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

#include "facepipelineretrain.h"

// Qt includes

#include <QList>
#include <QSet>
#include <QElapsedTimer>
#include <QRectF>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_opencv.h"
#include "sharedqueue.h"
#include "collectionmanager.h"
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
#include "faceclassifier.h"

namespace Digikam
{

FacePipelineRetrain::FacePipelineRetrain(const FaceScanSettings& _settings)
    : FacePipelineBase(_settings)
{
}

FacePipelineRetrain::~FacePipelineRetrain()
{
}

bool FacePipelineRetrain::start()
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

bool FacePipelineRetrain::finder()
{
    // All threads start with the same basic functions

    MLPipelineQueue* thisQueue = nullptr, *nextQueue = nullptr;
    stageStart(QThread::LowPriority, MLPipelineStage::Finder, MLPipelineStage::Loader, thisQueue, nextQueue);
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

            if (!moreCpu)
            {
                moreCpu = checkMoreWorkers(totalItemCount, imageIds.size(), settings.useFullCpu);
            }

            for (qlonglong imageId : std::as_const(imageIds))
            {
                // filter out duplicate image IDs

                if (!filter.contains(imageId))
                {
                    QList<FaceTagsIface> faces = utils.confirmedFaceTagsIfaces(imageId);

                    for (const FaceTagsIface& face : std::as_const(faces))
                    {
                        ++totalItemCount;

                        enqueue(nextQueue, new FacePipelinePackageBase(imageId, face));
                    }
                }
            }
        }
    }

    Q_EMIT signalUpdateItemCount(totalItemCount);

    pipelinePerformanceEnd(MLPipelineStage::Finder, totalItemCount, timer);

    //--------------------------------------------------------------------------------
    // all threads end with the same basic functions

    stageEnd(MLPipelineStage::Finder, MLPipelineStage::Loader);

    return true;
}

bool FacePipelineRetrain::loader()
{
    return commonFaceThumbnailLoader(QStringLiteral("FacePipelineRetrain"),
                                     QThread::LowPriority,
                                     MLPipelineStage::Loader,
                                     MLPipelineStage::Extractor);
}

bool FacePipelineRetrain::extractor()
{
    return commonFaceThumbnailExtractor(QStringLiteral("FacePipelineRetrain"),
                                        QThread::LowPriority,
                                        MLPipelineStage::Extractor,
                                        MLPipelineStage::Writer,
                                        true);
}

bool FacePipelineRetrain::classifier()
{
    return false;
}

bool FacePipelineRetrain::writer()
{
    MLPIPELINE_STAGE_START(QThread::LowPriority, MLPipelineStage::Writer, MLPipelineStage::None);
    FacePipelinePackageBase* package = nullptr;

    /* =========================================================================================
     * Pipeline stage specific initialization code
     *
     * Use the block from here to MLPIPELINE_LOOP_START to initialize the stage.
     * The code in this block is run once per stage initialization. The number of instances
     * is at least 1. More instances are created by addMoreWorkers if needed.
     */

    FaceUtils utils;
    IdentityProvider* const idProvider = IdentityProvider::instance();

    // clear all identites and training from recognition DB before
    // we start looping through the results from the pipeline

    idProvider->clearAllTraining();

    MLPIPELINE_LOOP_START(MLPipelineStage::Writer, thisQueue);
    package = static_cast<FacePipelinePackageBase*>(mlpackage);

    /* =========================================================================================
     * Start pipeline stage specific loop
     *
     * All code from here to MLPIPELINE_LOOP_END is in a try/catch block and loop.
     * This loop is run once per image.
     */

    {
        QString displayName;
        if (0 != package->features.rows)
        {
            Identity identity = utils.identityForTag(package->face.tagId());
            displayName = identity.attribute(QStringLiteral("name"));

            if (package->useForTraining)
            {
                idProvider->addTraining(identity, package->face.hash(), package->features);
            }
            else
            {
                qCDebug(DIGIKAM_FACESENGINE_LOG) << "FacePipelineEdit::writer(): not using for training: " << package->info.filePath();
            }
        }
        else
        {
            qCDebug(DIGIKAM_FACESENGINE_LOG) << "FacePipelineEdit::writer(): bad mat";
        }

        QString albumName = CollectionManager::instance()->albumRootLabel(package->info.albumRootId());

        // send a notification that the image was processed

        notify(MLPipelineNotification::notifyProcessed,
                package->info.name(),
                albumName + package->info.relativePath(),
                displayName,
                (displayName.isEmpty() ? 0 : 1),
                package->thumbnail);

        // delete the package

        delete package;
    }

    /* =========================================================================================
     * End pipeline stage specific loop
     */

    MLPIPELINE_LOOP_END(MLPipelineStage::Writer, "FacePipelineRetrain::writer");

    /* =========================================================================================
     * Pipeline stage specific cleanup
     *
     * Use the block from here to MLPIPELINE_STAGE_END to clean up any resources used by the stage.
     */

    // retrain the classifier after all results have been processed

    FaceClassifier::instance()->retrain();

    MLPIPELINE_STAGE_END(MLPipelineStage::Writer, MLPipelineStage::None);
}

void FacePipelineRetrain::addMoreWorkers()
{
    addWorker(Loader);
    addWorker(Extractor);
    addWorker(Extractor);
}

} // namespace Digikam

#include "moc_facepipelineretrain.cpp"
