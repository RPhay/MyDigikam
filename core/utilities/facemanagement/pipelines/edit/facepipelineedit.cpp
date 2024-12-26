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

#include "facepipelineedit.h"

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
#include "metadatahub.h"
#include "scancontroller.h"

namespace Digikam
{
// -----------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN FacePipelineEditCreator
{
public:

    FacePipelineEdit object;
};

Q_GLOBAL_STATIC(FacePipelineEditCreator, FacePipelineEditCreator)

// -----------------------------------------------------------------------------------------------

FacePipelineEdit::FacePipelineEdit()
    : FacePipelineBase(FaceScanSettings())
{
}

FacePipelineEdit::~FacePipelineEdit()
{
}

FacePipelineEdit* FacePipelineEdit::instance()
{
    return &FacePipelineEditCreator->object;
}

FaceTagsIface FacePipelineEdit::confirmFace(const ItemInfo& info,
                                            const FaceTagsIface& face,
                                            int tagId,
                                            bool retrain)
{
    MLPipelineQueue* const nextQueue       = queues.value(MLPipelineStage::Loader);
    FacePipelinePackageBase* const package = new FacePipelinePackageBase(info, face, tagId, face.region(), DImg(), FacePipelinePackageBase::EditPipelineAction::Confirm, retrain);
    enqueue(nextQueue, package);

    return (FaceTagsEditor::confirmedEntry(face, tagId, face.region()));
}

void FacePipelineEdit::removeFace(const ItemInfo& info,
                                  const FaceTagsIface& face)
{
    MLPipelineQueue* const nextQueue       = queues.value(MLPipelineStage::Writer);
    FacePipelinePackageBase* const package = new FacePipelinePackageBase(info, face, face.tagId(), face.region(), DImg(), FacePipelinePackageBase::EditPipelineAction::Remove, face.isConfirmedName());
    enqueue(nextQueue, package);
}

FaceTagsIface FacePipelineEdit::editTag(const ItemInfo& info,
                                        const FaceTagsIface& face,
                                        int newTagId)
{
    MLPipelineQueue* const nextQueue       = queues.value(MLPipelineStage::Writer);
    FacePipelinePackageBase* const package = new FacePipelinePackageBase(info, face, newTagId, face.region(), DImg(), FacePipelinePackageBase::EditPipelineAction::EditTag, face.isConfirmedName());
    enqueue(nextQueue, package);

    FaceTagsIface newFace(package->face);
    newFace.setTagId(newTagId);

    return newFace;
}

FaceTagsIface FacePipelineEdit::editRegion(const ItemInfo& info,
                                           const FaceTagsIface& face,
                                           const TagRegion& region,
                                           const DImg& image,
                                           bool retrain)
{
    MLPipelineQueue* const nextQueue       = queues.value(MLPipelineStage::Writer);
    FacePipelinePackageBase* const package = new FacePipelinePackageBase(info, face, face.tagId(), region, image, FacePipelinePackageBase::EditPipelineAction::EditRegion, retrain);
    enqueue(nextQueue, package);

    FaceTagsIface newFace(package->face);
    newFace.setRegion(TagRegion());

    return newFace;
}

FaceTagsIface FacePipelineEdit::addManually(const ItemInfo& info,
                                            const DImg& image,
                                            const TagRegion& region,
                                            bool retrain)
{
    MLPipelineQueue* const nextQueue       = queues.value(MLPipelineStage::Writer);
    FaceTagsIface face                     = FaceTagsEditor::unconfirmedEntry(info.id(), -1, region);
    FacePipelinePackageBase* const package = new FacePipelinePackageBase(info, face, face.tagId(), face.region(), image, FacePipelinePackageBase::EditPipelineAction::AddManually, retrain);
    enqueue(nextQueue, package);

    return face;
}

bool FacePipelineEdit::start()
{
    if (!started)
    {
        started = true;

        {
            // use the mutex to synchronize the start of the threads

            QMutexLocker lock(&mutex);

            // add the worker threads for this pipeline

            // addWorker(MLPipelineStage::Finder);
            addWorker(MLPipelineStage::Loader);
            addWorker(MLPipelineStage::Extractor);
            // addWorker(MLPipelineStage::Classifier);
            addWorker(MLPipelineStage::Writer);
        }

        return FacePipelineBase::start();
    }
    else
    {
        return true;
    }
}

bool FacePipelineEdit::loader()
{
    return commonFaceThumbnailLoader(QStringLiteral("FacePipelineEdit"),
                                     MLPipelineStage::Loader,
                                     MLPipelineStage::Extractor);
}

bool FacePipelineEdit::extractor()
{
    return commonFaceThumbnailExtractor(QStringLiteral("FacePipelineEdit"),
                                        MLPipelineStage::Extractor,
                                        MLPipelineStage::Writer);
}

bool FacePipelineEdit::writer()
{
    // All threads start with the same basic functions

    MLPipelineQueue* thisQueue = nullptr, *nextQueue = nullptr;
    stageStart(QThread::LowPriority, MLPipelineStage::Writer, MLPipelineStage::None, thisQueue, nextQueue);
    FacePipelinePackageBase* package = nullptr;
    QElapsedTimer timer;

    //--------------------------------------------------------------------------------

    FaceUtils utils;
    IdentityProvider* const idProvider             = IdentityProvider::instance();

    ThumbnailLoadThread* const thumbnailLoadThread = new ThumbnailLoadThread;
    // thumbnailLoadThread = ThumbnailLoadThread::defaultThread();

    thumbnailLoadThread->setPixmapRequested(false);
    thumbnailLoadThread->setThumbnailSize(ThumbnailLoadThread::maximumThumbnailSize());
    thumbnailLoadThread->setPriority(QThread::NormalPriority);

    while (!cancelled)
    {
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

            switch (package->action)
            {
                case FacePipelinePackageBase::EditPipelineAction::Confirm:
                {
                    FaceTagsIface confirmedFace = utils.confirmName(package->face, package->tagId, package->face.region());
                    Identity identity           = utils.identityForTag(confirmedFace.tagId());

                    if (0 != package->features.rows)
                    {
                        idProvider->addTraining(identity, confirmedFace.hash(), package->features);
                    }
                    else
                    {
                        qCDebug(DIGIKAM_FACESENGINE_LOG) << "FacePipelineEdit::writer(): bad mat";
                    }

                    break;
                }

                case FacePipelinePackageBase::EditPipelineAction::Remove:
                {
                    utils.removeFace(package->face);
                    break;
                }

                case FacePipelinePackageBase::EditPipelineAction::EditTag:
                {
                    // Change Tag operation.

                    utils.changeTag(package->face, package->tagId);

                    break;
                }

                case FacePipelinePackageBase::EditPipelineAction::EditRegion:
                {
                    if (package->face.region() != package->region)
                    {
                        utils.changeRegion(package->face, package->region);
                    }

                    break;
                }

                case FacePipelinePackageBase::EditPipelineAction::AddManually:
                {
                    utils.addManually(utils.unconfirmedEntry(package->info.id(), package->tagId, package->region));
                }

                    // if      (package->face.isNull())
                    // {
                    //     // Add Manually.
                    //
                    //     FaceTagsIface newFace = utils.unconfirmedEntry(package->info.id(), package->face.assignedTagId, package->face.assignedRegion);
                    //     utils.addManually(newFace);
                    //     // add << FacePipelineFaceTagsIface(newFace);
                    // }
                    // else if (package->face.assignedRegion.isValid())
                    // {
                    //     add << FacePipelineFaceTagsIface();
                    // }
            }

            // update the tags

            if (utils.normalTagChanged())
            {
                MetadataHub hub;
                hub.load(package->info);

                ScanController::FileMetadataWrite writeScope(package->info);
                writeScope.changed(hub.writeToMetadata(package->info, MetadataHub::WRITE_TAGS));
            }

            // store the thumbnails if we have an image

            if (!package->image.isNull())
            {
                QList<FaceTagsIface> faces;
                faces << package->face;
                utils.storeThumbnails(thumbnailLoadThread, package->info.filePath(), faces, package->image);
            }

            // retrain the face classifier if the retrain flag is set

            if (package->retrain)
            {
                FaceClassifier::instance()->retrain();
            }

            // send a notification that the image was processed

            notify(MLPipelineNotification::notifyProcessed,
                   package->info.name(),
                   package->info.filePath(),
                   package->faceRects.size(),
                   package->thumbnail);

            // delete the package

            delete package;

            // end pipeline stage specific code
            //////////////////////////////////////////////////////////////////////////////////////////////

            performanceProfileList[MLPipelineStage::Writer].elapsedTime   += timer.elapsed();
            performanceProfileList[MLPipelineStage::Writer].maxElapsedTime = qMax((qint64)performanceProfileList[MLPipelineStage::Writer].maxElapsedTime, timer.elapsed());
        }

        catch (const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }

        catch (...)
        {
            qCDebug(DIGIKAM_FACESENGINE_LOG) << "FacePipelineEdit::writer(): unknown error. Restarting...";

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

void FacePipelineEdit::addMoreWorkers()
{
    // edit pipeline is always single thread per stage
}

} // namespace Digikam

#include "moc_facepipelineedit.cpp"
