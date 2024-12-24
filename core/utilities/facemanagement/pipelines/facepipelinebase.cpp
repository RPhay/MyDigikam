/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-11-10
 * Description : Integrated, multithread face detection / recognition
 *
 * SPDX-FileCopyrightText: 2024      by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024      by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "facepipelinebase.h"

// Qt includes

#include <QtConcurrent>
#include <QThread>
#include <QList>

// local includes

#include "digikam_debug.h"
#include "digikam_opencv.h"
#include "sharedqueue.h"
#include "facepipelinepackagebase.h"
#include "thumbnailloadthread.h"
#include "dnnsfaceextractor.h"

namespace Digikam
{

FacePipelineBase::FacePipelineBase(const FaceScanSettings& _settings) :
                                   MLPipelineFoundation(),
                                   settings(_settings)
{
}

FacePipelineBase::~FacePipelineBase()
{
}

bool FacePipelineBase::start()
{
    return MLPipelineFoundation::start();
}

void FacePipelineBase::cancel()
{
    return MLPipelineFoundation::cancel();
}

bool FacePipelineBase::commonFaceThumbnailLoader(const QString& pipelineName, MLPipelineStage thisStage, MLPipelineStage nextStage)
{
    // All threads start with the same basic functions
    MLPipelineQueue *thisQueue = nullptr, *nextQueue = nullptr;
    stageStart(QThread::LowPriority, thisStage, nextStage, thisQueue, nextQueue);
    FacePipelinePackageBase* package = nullptr;
    QElapsedTimer timer;
    //--------------------------------------------------------------------------------

    ThumbnailLoadThread* thumbnailLoadThread = new ThumbnailLoadThread;
    // ThumbnailLoadThread* thumbnailLoadThread = ThumbnailLoadThread::defaultThread();

    thumbnailLoadThread->setPixmapRequested(false);
    thumbnailLoadThread->setThumbnailSize(ThumbnailLoadThread::maximumThumbnailSize());
    thumbnailLoadThread->setPriority(QThread::NormalPriority);

    ThumbnailImageCatcher* catcher = new ThumbnailImageCatcher(thumbnailLoadThread);

    catcher->setActive(true);

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
            performanceProfileList[thisStage].maxQueueCount = qMax(performanceProfileList[thisStage].maxQueueCount, thisQueue->size());
            ++performanceProfileList[thisStage].itemCount;

            timer.start();

            //////////////////////////////////////////////////////////////////////////////////////////////
            // start pipeline stage specific code

            catcher->thread()->find(ItemInfo::thumbnailIdentifier(package->face.imageId()), package->face.region().toRect());
            catcher->enqueue();
            QList<QImage> images = catcher->waitForThumbnails();

            if (images.size() && !images[0].isNull())
            {
                package->thumbnail = images[0];
                package->thumbnailIcon = QIcon(DImg(package->thumbnail).smoothScale(48, 48, Qt::KeepAspectRatio).convertToPixmap());

                enqueue(nextQueue, package);
            }
            else
            {
                // send a notification that the file was skipped

                notify(MLPipelineNotification::notifySkipped, package->info.name(), package->info.filePath(), 1, DImg());

                // delete the package since it is not needed

                delete package;            
            }

            // end pipeline stage specific code
            //////////////////////////////////////////////////////////////////////////////////////////////

            performanceProfileList[thisStage].elapsedTime += timer.elapsed();
            performanceProfileList[thisStage].maxElapsedTime = qMax(performanceProfileList[thisStage].maxElapsedTime, timer.elapsed());
        }
        catch(const std::exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << pipelineName << "::loader(): unknown error. " << e.what() << "    Restarting...";
            if (package)
            {
                delete package;
            }
        }
        catch(...)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << pipelineName << "::loader(): unknown error.  Restarting...";
            if (package)
            {
                delete package; 
            }
        }        
    }

    catcher->setActive(false);

    delete catcher;
    delete thumbnailLoadThread;

    //--------------------------------------------------------------------------------
    // all threads end with the same basic functions
    stageEnd(thisStage, nextStage);

   return true;
}

bool FacePipelineBase::commonFaceThumbnailExtractor(const QString& pipelineName, MLPipelineStage thisStage, MLPipelineStage nextStage)
{
    // All threads start with the same basic functions
    MLPipelineQueue *thisQueue = nullptr, *nextQueue = nullptr;
    stageStart(QThread::LowPriority, thisStage, nextStage, thisQueue, nextQueue);
    FacePipelinePackageBase* package = nullptr;
    QElapsedTimer timer;
    //--------------------------------------------------------------------------------
    
    DNNSFaceExtractor           extractor;

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
            performanceProfileList[MLPipelineStage::Extractor].maxQueueCount = qMax(performanceProfileList[MLPipelineStage::Extractor].maxQueueCount, thisQueue->size());
            ++performanceProfileList[MLPipelineStage::Extractor].itemCount;

            timer.start();

            //////////////////////////////////////////////////////////////////////////////////////////////
            // start pipeline stage specific code

            cv::Mat uImage, cvImage, uFeatures;
            cv::Mat mImage;

            QImage inputImage(package->thumbnail.copy());

            if (inputImage.format() != QImage::Format_ARGB32_Premultiplied)
            {
                inputImage = inputImage.convertToFormat(QImage::Format_ARGB32_Premultiplied);
            }

            mImage = cv::Mat(inputImage.height(), inputImage.width(), CV_8UC4, inputImage.scanLine(0), inputImage.bytesPerLine()); //.getUMat(cv::ACCESS_RW);
            // uImage = mImage.getUMat(cv::ACCESS_RW);
            uImage = mImage;
            // inputImage.detach();
            cv::cvtColor(uImage, cvImage, CV_RGBA2RGB);

            uFeatures = extractor.getFaceEmbedding(cvImage);

            package->features = uFeatures;

            enqueue(nextQueue, package);

            // if (0 == uFeatures.rows)
            // {
            //     // no faces found

            //     if (true)
            //     {
            //         QString filename = QLatin1String("/Users/michmill/Downloads/thumbnails/") + package->info.name();        
            //         filename = filename.first(filename.lastIndexOf(QLatin1String("."))) + pipelineName + QStringLiteral(".jpg");
            //         qCCritical(DIGIKAM_FACESENGINE_LOG) << pipelineName << "::extractor(): writing. " << filename << package->info;
            //         cv::imwrite(filename.toLocal8Bit().data(), cvImage);
            //     }
            // }

            // end pipeline stage specific code
            //////////////////////////////////////////////////////////////////////////////////////////////

            performanceProfileList[MLPipelineStage::Extractor].elapsedTime += timer.elapsed();
            performanceProfileList[MLPipelineStage::Extractor].maxElapsedTime = qMax(performanceProfileList[MLPipelineStage::Extractor].maxElapsedTime, timer.elapsed());
        }
        catch(const std::exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << pipelineName << "::extractor(): unknown error. " << e.what() << "    Restarting...";
            if (package)
            {
                delete package;
            }
        }
        catch(...)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << pipelineName << "::extractor(): unknown error.  Restarting...";
            if (package)
            {
                delete package;
            }
        }
    }

    //--------------------------------------------------------------------------------
    // all threads end with the same basic functions
    stageEnd(thisStage, nextStage);

    return true;
}

bool FacePipelineBase::enqueue(MLPipelineQueue* thisQueue, MLPipelinePackageFoundation* package)
{
    // calculate the package size.  Only big items need to be checked

    package->size = static_cast<FacePipelinePackageBase*>(package)->image.size().width() * static_cast<FacePipelinePackageBase*>(package)->image.size().height() * 4;

    return MLPipelineFoundation::enqueue(thisQueue, package);
}

}

#include "moc_facepipelinebase.cpp"
