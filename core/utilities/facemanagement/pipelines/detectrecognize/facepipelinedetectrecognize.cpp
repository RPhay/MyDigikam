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

#include "facepipelinedetectrecognize.h"

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
#include "faceclassifier.h"
#include "identityprovider.h"

namespace Digikam
{

FacePipelineDetectRecognize::FacePipelineDetectRecognize(const FaceScanSettings& _settings)
    : FacePipelineBase(_settings)
{
}

FacePipelineDetectRecognize::~FacePipelineDetectRecognize()
{
    if (faceDetector)
    {
        delete faceDetector;
    }

    // extractor is singleton, so no need to delete it
}

bool FacePipelineDetectRecognize::start()
{

    // create the face detector and extractor

    try
    {
        faceDetector = new DNNFaceDetectorYuNet;
        faceExtractor = static_cast<DNNModelSFace*>(DNNModelManager::instance()->getModel(QStringLiteral("sface"),
                                                                                          DNNModelUsage::DNNUsageFaceRecognition));
        faceExtractor->getNet();
    }

    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';

        if (faceDetector)
        {
            delete faceDetector;
            faceDetector = nullptr;
        }

        // extractor is singleton, so no need to delete it
    }

    catch (...)
    {
        if (faceDetector)
        {
            delete faceDetector;
            faceDetector = nullptr;
        }
    }

    // check if the detector and extractor were created

    if ((nullptr == faceDetector) || (nullptr == faceExtractor) || !faceExtractor->modelLoaded)
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

    faceDetector->setFaceDetectionSize(settings.detectSize);
    faceDetector->uiConfidenceThreshold = settings.detectAccuracy;

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

bool FacePipelineDetectRecognize::finder()
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

    for (const ItemInfo& info : std::as_const(settings.infos))
    {
        // filter out duplicate image IDs

        qlonglong imageId = info.id();

        if (!filter.contains(imageId))
        {
            ++totalItemCount;
            filter << imageId;
            enqueue(nextQueue, new FacePipelinePackageBase(imageId));
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

bool FacePipelineDetectRecognize::loader()
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

            pipelinePerformanceStart(Loader, timer);

            //////////////////////////////////////////////////////////////////////////////////////////////
            // start pipeline stage specific code

            // check if the ID is for an image (not video or other file type)

            bool sendNotification = true;

            if (DatabaseItem::Category::Image == package->info.category())
            {
                // load high quality image for detection

                package->image = PreviewLoadThread::loadHighQualitySynchronously(package->info.filePath());

                // check for corrupted images that can't be loaded

                if (!package->image.isNull())
                {
                    // send to the next stage

                    enqueue(nextQueue, package);

                    sendNotification = false;
                }
            }

            if (sendNotification)
            {
                // send a notification that the file was skipped

                notify(MLPipelineNotification::notifySkipped,
                       package->info.name(),
                       package->info.filePath(),
                       package->faceRects.size(),
                       package->thumbnailIcon);

                // delete the package since it is not needed

                delete package;
            }

            // end pipeline stage specific code
            //////////////////////////////////////////////////////////////////////////////////////////////

            pipelinePerformanceEnd(MLPipelineStage::Loader, timer);
        }

        catch (const std::exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "FacePipelineDetectRecognize::loader(): unknown error. "
                                                << e.what() << " Restarting...";

            if (package)
            {
                delete package;
            }
        }

        catch (...)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "FacePipelineDetectRecognize::loader(): unknown error.  Restarting...";

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

bool FacePipelineDetectRecognize::extractor()
{
    // All threads start with the same basic functions

    MLPipelineQueue* thisQueue = nullptr, *nextQueue = nullptr;
    stageStart(QThread::NormalPriority, MLPipelineStage::Extractor, MLPipelineStage::Classifier, thisQueue, nextQueue);
    FacePipelinePackageBase* package = nullptr;
    QElapsedTimer timer;

    //--------------------------------------------------------------------------------

    FaceUtils utils;

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

            // create a thumbnail for the notification

            package->thumbnailIcon = QIcon(package->image.smoothScale(48, 48, Qt::KeepAspectRatio).convertToPixmap());

            // copy the image to a cv::Mat

            cv::UMat cvUImage;
            qCDebug(DIGIKAM_FACESENGINE_LOG) << QStringLiteral("bytesDepth:") << package->image.bytesDepth()
                                             << QStringLiteral("  bitsDepth:") << package->image.bitsDepth()
                                             << QStringLiteral("  Alpha:") << package->image.hasAlpha()
                                             << QStringLiteral("  Image name:") << package->image.originalFilePath();

            int type                 = (package->image.sixteenBit() ? CV_16UC4 : CV_8UC4);

            // create a cv::Mat image from the QImage and move it to the GPU with a cv::UMat

            cv::UMat cvUImageWrapper = cv::Mat(package->image.height(),
                                               package->image.width(),
                                               type,
                                               package->image.bits())
                                       .getUMat(cv::ACCESS_FAST);

            // prepare the UMat image for processing

            // DImg is always 4 channel. convert to 3 channel RGB

            cvtColor(cvUImageWrapper, cvUImage, cv::COLOR_RGBA2RGB);

            // convert to 8 bit if 16 bit

            if (type == CV_16UC4)
            {
                cvUImage.convertTo(cvUImage, CV_8UC3, 1 / 256.0);
            }

            // resize the image if needed. Only resize if the image is larger than the input size of the detector

            cv::Size inputImageSize = faceDetector->nnInputSizeRequired();

            if (std::max(cvUImageWrapper.cols, cvUImageWrapper.rows) > std::max(inputImageSize.width, inputImageSize.height))
            {
                // Image should be resized. YuNet image sizes are much more flexible than SSD and YOLO
                // so we just need to make sure no one bound exceeds the max. No padding needed.

                float resizeFactor      = std::min(static_cast<float>(inputImageSize.width)  / static_cast<float>(cvUImageWrapper.cols),
                                                   static_cast<float>(inputImageSize.height) / static_cast<float>(cvUImageWrapper.rows));

                int newWidth            = (int)(resizeFactor * cvUImageWrapper.cols);
                int newHeight           = (int)(resizeFactor * cvUImageWrapper.rows);
                cv::resize(cvUImage, cvUImage, cv::Size(newWidth, newHeight));
            }

            // we are done with the cvUImageWrapper, so release it

            cvUImageWrapper.release();

            // detect any faces in the image

            cv::UMat udetectionResults  = faceDetector->callModel(cvUImage);

            // process detected faces

            if (udetectionResults.rows > 0)
            {
                cv::Mat detectionResults = udetectionResults.getMat(cv::ACCESS_READ);

                // get list of previously confirmed faces

                QList<FaceTagsIface> faces = utils.confirmedFaceTagsIfaces(package->info.id());
                QList<QRectF> faceFRects;

                // Loop through the faces found.

                for (int i = 0 ; i < detectionResults.rows ; ++i)
                {
                    // Create the rect of the face.

                    int X       = static_cast<int>(detectionResults.at<float>(i, 0));
                    int Y       = static_cast<int>(detectionResults.at<float>(i, 1));
                    int width   = static_cast<int>(detectionResults.at<float>(i, 2));
                    int height  = static_cast<int>(detectionResults.at<float>(i, 3));

                    // Add the rect to result list.

                    faceFRects << QRectF(qreal(X)      / qreal(cvUImage.cols),
                                         qreal(Y)      / qreal(cvUImage.rows),
                                         qreal(width)  / qreal(cvUImage.cols),
                                         qreal(height) / qreal(cvUImage.rows));

                    // check if rect is already assigned to a face to filter out confirmed and ignored faces

                    bool found = false;

                    if (faces.size() > 0)
                    {
                        for (int j = 0; j < faces.size(); ++j)
                        {
                            // compute current image relative rect

                            QRect rect = QRect(package->image.width()  * faceFRects[i].x(),
                                               package->image.height() * faceFRects[i].y(),
                                               package->image.width()  * faceFRects[i].width(),
                                               package->image.height() * faceFRects[i].height());

                            if (faces[j].region().intersects(TagRegion(rect), 0.85))
                            {
                                found = true;

                                break;
                            }
                        }
                    }

                    // if face is not found (confirmed), then extract for classification

                    if (!found)
                    {
                        cv::UMat ualignedFace, uface_features;
                        cv::Mat face_features;

                        // extract the face vectors (features) for classification

                        {
                            QMutexLocker lock(&(faceExtractor->mutex));

                            faceExtractor->getNet()->alignCrop(cvUImage, udetectionResults.row(i), ualignedFace);

                            faceExtractor->getNet()->feature(ualignedFace, uface_features);

                            face_features = uface_features.getMat(cv::ACCESS_READ);
                        }

                        // normalize the face features if we have any

                        if (0 < face_features.rows)
                        {
                            // normalize the face features

                            cv::Mat normalized_features;
                            normalize(face_features, normalized_features);

                            // add the face features and face rect to the package

                            package->featuresList << normalized_features;
                            package->faceRects << faceFRects[i];
                        }
                    }
                }
            }

            // send the package to the next stage

            enqueue(nextQueue, package);

            // end pipeline stage specific code
            //////////////////////////////////////////////////////////////////////////////////////////////

            pipelinePerformanceEnd(MLPipelineStage::Extractor, timer);
        }

        catch (const std::exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "FacePipelineDetectRecognize::extractor(): unknown extractor error. "
                                                << e.what() << " Restarting...";

            if (package)
            {
                delete package;
            }
        }

        catch (...)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "FacePipelineDetectRecognize::extractor(): unknown extractor error. Restarting...";

            if (package)
            {
                delete package;
            }
        }
    }

    //--------------------------------------------------------------------------------
    // all threads end with the same basic functions

    stageEnd(MLPipelineStage::Extractor, MLPipelineStage::Classifier);

    return true;
}

bool FacePipelineDetectRecognize::classifier()
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

            pipelinePerformanceStart(MLPipelineStage::Classifier, timer);

            //////////////////////////////////////////////////////////////////////////////////////////////
            // start pipeline stage specific code

            for (int i = 0 ; i < package->featuresList.size() ; ++i)
            {
                // verify the feature mat is not empty

                if (0 < package->featuresList[i].rows)
                {
                    // classify the features

                    package->labelList << classifier->predict(package->featuresList[i]);
                }
                else
                {
                    package->labelList << -1;
                }
            }

            enqueue(nextQueue, package);

            // end pipeline stage specific code
            //////////////////////////////////////////////////////////////////////////////////////////////

            pipelinePerformanceEnd(MLPipelineStage::Classifier, timer);
        }

        catch (const std::exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "FacePipelineRecognize::classifier(): unknown error. "
                                                << e.what() << " Restarting...";

            if (package)
            {
                delete package;
            }
        }

        catch (...)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "FacePipelineRecognize::classifier(): unknown error. Restarting...";

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

bool FacePipelineDetectRecognize::writer()
{
    // All threads start with the same basic functions

    MLPipelineQueue* thisQueue = nullptr, *nextQueue = nullptr;
    stageStart(QThread::LowPriority, MLPipelineStage::Writer, MLPipelineStage::None, thisQueue, nextQueue);
    FacePipelinePackageBase* package = nullptr;
    QElapsedTimer timer;

    //--------------------------------------------------------------------------------

    IdentityProvider* const idProvider             = IdentityProvider::instance();
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
                QList<Identity> identities;

                for (int i = 0 ; i < package->faceRects.size() ; ++i)
                {
                    if (package->labelList[i] != -1)
                    {
                        Identity identity = idProvider->identity(package->labelList[i]);
                        identities << identity;
                    }
                    else
                    {
                        identities << Identity();
                    }
                }

                QList<FaceTagsIface> databaseFaces = utils.writeUnconfirmedResults(package->info.id(),
                                                                                   package->faceRects,
                                                                                   identities,
                                                                                   package->image.originalSize());

                // store the thumbnails

                if (!package->image.isNull())
                {
                    utils.storeThumbnails(thumbnailLoadThread, package->info.filePath(),
                                          databaseFaces, package->image);
                }

                // QList<FaceTagsIface> faces = utils.unconfirmedFaceTagsIfaces(package->info.id());
                //
                // for (int i = 0 ; i < package->faceRects.size() ; ++i)
                // {
                //     if (-1 != package->labelList[i])
                //     {
                //         int faceIndex = -1;
                //
                //         QRect rect = QRect(package->image.width() * package->faceRects[i].x(),
                //                            package->image.height() * package->faceRects[i].y(),
                //                            package->image.width() * package->faceRects[i].width(),
                //                            package->image.height() * package->faceRects[i].height());
                //         qCDebug(DIGIKAM_FACESENGINE_LOG) << "test face rect:" << rect;
                //
                //         for (int j = 0 ; j < databaseFaces.size() ; ++j)
                //         {
                //             qCDebug(DIGIKAM_FACESENGINE_LOG) << "unconfirmed face rect:" << databaseFaces[j].region();
                //
                //             if (databaseFaces[j].region().intersects(TagRegion(rect), 0.85))
                //             {
                //                 faceIndex = j;
                //                 break;
                //             }
                //         }
                //
                //         if (-1 != faceIndex)
                //         {
                //             Identity identity = idProvider->identity(package->labelList[i]);
                //             int tagId         = FaceTags::getOrCreateTagForIdentity(identity.attributesMap());
                //             utils.changeSuggestedName(databaseFaces[faceIndex], tagId);
                //         }
                //     }
                // }
            }

            // send a notification that the image was processed

            notify(MLPipelineNotification::notifyProcessed,
                   package->info.name(),
                   package->info.filePath(),
                   package->faceRects.size(),
                   package->thumbnailIcon);

            // delete the package

            delete package;

            // end pipeline stage specific code
            //////////////////////////////////////////////////////////////////////////////////////////////

            pipelinePerformanceEnd(MLPipelineStage::Writer, timer);
        }

        catch (const std::exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "FacePipelineDetectRecognize::writer(): unknown writer error. "
                                                << e.what() << " Restarting...";

            if (package)
            {
                delete package;
            }
        }

        catch (...)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << "FacePipelineDetectRecognize::writer(): unknown writer error. Restarting...";

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

void FacePipelineDetectRecognize::addMoreWorkers()
{
    // use the performanceProfile metrics to find the slowest stages
    // and add more workers to those stages

    // for the detection pipeline, the loader is the slowest stage
    // so add 3 more loaders and 2 more extractors

    addWorker(Loader);
    addWorker(Loader);
    addWorker(Loader);
    addWorker(Extractor);
    addWorker(Extractor);
}

} // namespace Digikam

#include "moc_facepipelinedetectrecognize.cpp"
