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

#include "facepipelinerecognize.h"

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
#include "album.h"
#include "iteminfo.h"
#include "coredb.h"
#include "facescansettings.h"
#include "dimg.h"
#include "faceutils.h"
#include "facepipelinepackagebase.h"
#include "identityprovider.h"
#include "identity.h"
#include "dnnsfaceextractor.h"
#include "faceclassifier.h"

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

bool FacePipelineRecognize::finder()
{
    MLPIPELINE_FINDER_START(MLPipelineStage::Loader);

    /* =========================================================================================
     * Pipeline finder specific initialization code
     *
     * Use the block from here to MLPIPELINE_FINDER_END to find the IDs images to process.
     * The code in this block is run once per stage initialization. The number of instances
     * is alaways 1.
     */

    // get the IDs to process

    FaceUtils utils;
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

    /* =========================================================================================
     * Pipeline finder specific cleanup
     *
     * Use the block from here to MLPIPELINE_FINDER_END to clean up any resources used by the stage.
     */

    MLPIPELINE_FINDER_END(MLPipelineStage::Loader);
}

bool FacePipelineRecognize::loader()
{
    return commonFaceThumbnailLoader(QStringLiteral("FacePipelineRecognize"),
                                     MLPipelineStage::Loader,
                                     MLPipelineStage::Extractor);
}

bool FacePipelineRecognize::extractor()
{
    return commonFaceThumbnailExtractor(QStringLiteral("FacePipelineRecognize"),
                                        MLPipelineStage::Extractor,
                                        MLPipelineStage::Classifier);
}

bool FacePipelineRecognize::classifier()
{
    MLPIPELINE_STAGE_START(QThread::LowPriority, MLPipelineStage::Classifier, MLPipelineStage::Writer);
    FacePipelinePackageBase* package = nullptr;

    /* =========================================================================================
     * Pipeline stage specific initialization code
     *
     * Use the block from here to MLPIPELINE_LOOP_START to initialize the stage.
     * The code in this block is run once per stage initialization. The number of instances
     * is at least 1. More instances are created by addMoreWorkers if needed.
     */

    FaceClassifier* const classifier = FaceClassifier::instance();
    classifier->setParameters(settings);

    MLPIPELINE_LOOP_START(MLPipelineStage::Classifier, thisQueue);
    package = static_cast<FacePipelinePackageBase*>(mlpackage);

    /* =========================================================================================
     * Start pipeline stage specific loop
     *
     * All code from here to MLPIPELINE_LOOP_END is in a try/catch block and loop.
     * This loop is run once per image.
     */
    {
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

            notify(MLPipelineNotification::notifyProcessed,
                   package->info.name() + QStringLiteral("\n"),
                   package->info.relativePath(),
                   0,
                   package->thumbnail);

            // delete the package

            delete package;
        }
    }
    /* =========================================================================================
     * End pipeline stage specific loop
     */

    MLPIPELINE_LOOP_END(MLPipelineStage::Classifier, "FacePipelineRecognize::classifier");

    /* =========================================================================================
     * Pipeline stage specific cleanup
     *
     * Use the block from here to MLPIPELINE_STAGE_END to clean up any resources used by the stage.
     */

    MLPIPELINE_STAGE_END(MLPipelineStage::Classifier, MLPipelineStage::Writer);
}

bool FacePipelineRecognize::writer()
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

    MLPIPELINE_LOOP_START(MLPipelineStage::Writer, thisQueue);
    package = static_cast<FacePipelinePackageBase*>(mlpackage);

    /* =========================================================================================
     * Start pipeline stage specific loop
     *
     * All code from here to MLPIPELINE_LOOP_END is in a try/catch block and loop.
     * This loop is run once per image.
     */

    QString displayName = package->info.name() + QStringLiteral("\n");
    int matches = 0;

    if (-1 != package->label)
    {
        Identity identity = idProvider->identity(package->label);
        int tagId         = FaceTags::getOrCreateTagForIdentity(identity.attributesMap());
        utils.changeSuggestedName(package->face, tagId);
        displayName      += identity.attribute(QStringLiteral("name"));
        ++matches;
    }

    // send a notification that the image was processed

    notify(MLPipelineNotification::notifyProcessed,
           displayName,
           package->info.relativePath(),
           matches,
           package->thumbnail);

    // delete the package

    delete package;

    /* =========================================================================================
     * End pipeline stage specific loop
     */

    MLPIPELINE_LOOP_END(MLPipelineStage::Writer, "FacePipelineRecognize::writer");

    /* =========================================================================================
     * Pipeline stage specific cleanup
     *
     * Use the block from here to MLPIPELINE_STAGE_END to clean up any resources used by the stage.
     */

    MLPIPELINE_STAGE_END(MLPipelineStage::Writer, MLPipelineStage::None);
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
