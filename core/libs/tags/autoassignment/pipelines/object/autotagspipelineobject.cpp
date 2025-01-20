/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-11-10
 * Description : Performs autotags object detection and classification
 *
 * SPDX-FileCopyrightText: 2024-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "autotagspipelineobject.h"

// C++ includes

#include <vector>

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
#include "autotagsscansettings.h"
#include "dimg.h"
#include "previewloadthread.h"
#include "autotagspipelinepackagebase.h"
#include "dnnmodelconfig.h"
#include "autotagsclassifiersoftmax.h"
#include "autotagsclassifiermultiyolo.h"
#include "scancontroller.h"
#include "metadatahub.h"
#include "tagscache.h"
#include "localizeselector.h"
#include "qtopencvimg.h"

namespace Digikam
{

AutotagsPipelineObject::AutotagsPipelineObject(const AutotagsScanSettings& _settings)
    : AutotagsPipelineBase(_settings)
{
}

AutotagsPipelineObject::~AutotagsPipelineObject()
{
    if (autotagsClassifier)
    {
        delete autotagsClassifier;
    }
}

bool AutotagsPipelineObject::start()
{
    // create the image classification model

    try
    {
        switch (settings.objectDetectModel)
        {
            case AutotagsScanSettings::ObjectDetectionModel::YOLOV11NANO:
            {
                model = static_cast<DNNModelNet*>(DNNModelManager::instance()->getModel(QStringLiteral("YOLOv11-nano"), DNNModelUsage::DNNUsageObjectDetection));

                break;
            }

            case AutotagsScanSettings::ObjectDetectionModel::YOLOV11XLARGE:
            {
                model = static_cast<DNNModelNet*>(DNNModelManager::instance()->getModel(QStringLiteral("YOLOv11-xl"), DNNModelUsage::DNNUsageObjectDetection));

                break;
            }

            case AutotagsScanSettings::ObjectDetectionModel::RESNET152:
            {
                model = static_cast<DNNModelNet*>(DNNModelManager::instance()->getModel(QStringLiteral("ResNet152_v2"), DNNModelUsage::DNNUsageImageClassification));

                break;
            }

            default:
            {
                qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "AutotagsPipelineObject::start(): Unknown object detection model. ";

                return false;
            }
        }

        model->getNet();

        const DNNModelConfig* configModel = static_cast<DNNModelConfig*>(DNNModelManager::instance()->getModel(model->info.classList,
                                                                                                               DNNModelUsage::DNNUsageObjectDetection));

        if (configModel)
        {
            if (
                (AutotagsScanSettings::ObjectDetectionModel::YOLOV11NANO   == settings.objectDetectModel) ||
                (AutotagsScanSettings::ObjectDetectionModel::YOLOV11XLARGE == settings.objectDetectModel)
               )
            {
                autotagsClassifier = new AutotagsClassifierYolo(model->getThreshold(settings.uiConfidenceThreshold), configModel->getModelPath());
                static_cast<AutotagsClassifierYolo*>(autotagsClassifier)->setParams(AutotagsClassifierYolo::YoloVersion::YOLOv11,
                                                                                    QSize(model->info.imageSize,
                                                                                    model->info.imageSize));
            }
            else
            {
                autotagsClassifier = new AutotagsClassifierSoftmax(model->getThreshold(settings.uiConfidenceThreshold), configModel->getModelPath());
            }
        }
    }

    catch (const std::exception& e)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "AutotagsPipelineObject::start(): Unable to load model. " << e.what();

        if (model)
        {
            model = nullptr;
        }
    }

    catch (...)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "AutotagsPipelineObject::start(): Unknown error. Unable to load model. ";

        if (model)
        {
            model = nullptr;
        }
    }

    // check if the model and classifier were created

    if ((nullptr == model) || (nullptr == autotagsClassifier))
    {
        return false;
    }

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

    return AutotagsPipelineBase::start();
}

bool AutotagsPipelineObject::finder()
{
    if (settings.bqmMode)
    {
        return true;
    }

    MLPIPELINE_FINDER_START(MLPipelineStage::Loader);

    /* =========================================================================================
     * Pipeline finder specific initialization code
     *
     * Use the block from here to MLPIPELINE_FINDER_END to find the IDs images to process.
     * The code in this block is run once per stage initialization. The number of instances
     * is alaways 1.
     */

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
                    ++totalItemCount;
                    filter << imageId;
                    enqueue(nextQueue, new AutotagsPipelinePackageBase(imageId));
                }
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

bool AutotagsPipelineObject::loader()
{
    MLPIPELINE_STAGE_START(QThread::LowPriority, MLPipelineStage::Loader, MLPipelineStage::Extractor);
    AutotagsPipelinePackageBase* package = nullptr;

    /* =========================================================================================
     * Pipeline stage specific initialization code
     *
     * Use the block from here to MLPIPELINE_LOOP_START to initialize the stage.
     * The code in this block is run once per stage initialization. The number of instances
     * is at least 1. More instances are created by addMoreWorkers if needed.
     */

    MLPIPELINE_LOOP_START(MLPipelineStage::Loader, thisQueue);
    package = static_cast<AutotagsPipelinePackageBase*>(mlpackage);

    /* =========================================================================================
     * Start pipeline stage specific loop
     *
     * All code from here to MLPIPELINE_LOOP_END is in a try/catch block and loop.
     * This loop is run once per image.
     */

    {
        // check if the ID is for an image (not video or other file type)

        bool sendNotification = true;

        if (DatabaseItem::Category::Image == package->info.category())
        {
            // load image for detection

            package->image = PreviewLoadThread::loadFastSynchronously(package->info.filePath(), model->info.imageSize);

            // check for corrupted images that can't be loaded

            if (!package->image.isNull())
            {
                // create a thumbnail for the notification

                package->thumbnailIcon = QIcon(package->image.smoothScale(48, 48, Qt::KeepAspectRatio).convertToPixmap());

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
                    package->info.relativePath(),
                    0,
                    package->thumbnailIcon);

            // delete the package since it is not needed

            delete package;
        }
    }

    /* =========================================================================================
     * End pipeline stage specific loop
     */

    MLPIPELINE_LOOP_END(MLPipelineStage::Loader, "AutotagsPipelineObject::loader");

    /* =========================================================================================
     * Pipeline stage specific cleanup
     *
     * Use the block from here to MLPIPELINE_STAGE_END to clean up any resources used by the stage.
     */

    MLPIPELINE_STAGE_END(MLPipelineStage::Loader, MLPipelineStage::Extractor);
}

bool AutotagsPipelineObject::extractor()
{
    MLPIPELINE_STAGE_START(QThread::NormalPriority, MLPipelineStage::Extractor, MLPipelineStage::Classifier);
    AutotagsPipelinePackageBase* package = nullptr;

    /* =========================================================================================
     * Pipeline stage specific initialization code
     *
     * Use the block from here to MLPIPELINE_LOOP_START to initialize the stage.
     * The code in this block is run once per stage initialization. The number of instances
     * is at least 1. More instances are created by addMoreWorkers if needed.
     */

    MLPIPELINE_LOOP_START(MLPipelineStage::Extractor, thisQueue);
    package = static_cast<AutotagsPipelinePackageBase*>(mlpackage);

    /* =========================================================================================
     * Start pipeline stage specific loop
     *
     * All code from here to MLPIPELINE_LOOP_END is in a try/catch block and loop.
     * This loop is run once per image.
     */

    {
        // preprocess the image

        // copy the image to a cv::Mat

        cv::Mat cvImage         = QtOpenCVImg::image2Mat(package->image, CV_8UC3, QtOpenCVImg::MatColorOrder::MCO_RGB);

        // resize the image if needed.  Only resize if the image is larger than the input size of the detector

        cv::Size inputImageSize = cv::Size(model->info.imageSize, model->info.imageSize);

        if (std::max(cvImage.cols, cvImage.rows) > std::max(inputImageSize.width, inputImageSize.height))
        {
            // Image should be resized.

            float resizeFactor      = std::min(static_cast<float>(inputImageSize.width)  / static_cast<float>(cvImage.cols),
                                                static_cast<float>(inputImageSize.height) / static_cast<float>(cvImage.rows));

            int newWidth            = (int)(resizeFactor * cvImage.cols);
            int newHeight           = (int)(resizeFactor * cvImage.rows);
            cv::resize(cvImage, cvImage, cv::Size(newWidth, newHeight));
        }

        // pad the image if needed

        if ((model->info.imageSize != cvImage.cols) || (model->info.imageSize != cvImage.rows))
        {
            // Image needs to be padded so we add a border

            cv::Mat borderImage;
            int xPad = model->info.imageSize - cvImage.cols;
            int yPad = model->info.imageSize - cvImage.rows;

            cv::copyMakeBorder(cvImage, borderImage,
                                0, yPad,
                                0, xPad,
                                cv::BORDER_CONSTANT,
                                cv::Scalar(0, 0, 0));

            cvImage = borderImage;
        }

        // convert the image to a blob

        cv::UMat cvUBlob = cv::dnn::blobFromImage(cvImage, 1.0 / 255,
                                                cv::Size(cvImage.cols, cvImage.rows),
                                                cv::Scalar(0, 0, 0),
                                                true, false).getUMat(cv::ACCESS_READ);

        std::vector<cv::Mat> detectionResults;

        {
            // detect any objects in the image

            QMutexLocker lock(&(model->mutex));

            model->getNet().setInput(cvUBlob);

            model->getNet().forward(detectionResults, model->getNet().getUnconnectedOutLayersNames());
        }

        for (const auto& result : detectionResults)
        {
            package->featuresList << result;
        }

        // send the package to the next stage

        enqueue(nextQueue, package);
    }

    /* =========================================================================================
     * End pipeline stage specific loop
     */

    MLPIPELINE_LOOP_END(MLPipelineStage::Extractor, "AutotagsPipelineObject::extractor");

    /* =========================================================================================
     * Pipeline stage specific cleanup
     *
     * Use the block from here to MLPIPELINE_STAGE_END to clean up any resources used by the stage.
     */

    MLPIPELINE_STAGE_END(MLPipelineStage::Extractor, MLPipelineStage::Classifier);
}

bool AutotagsPipelineObject::classifier()
{
    MLPIPELINE_STAGE_START(QThread::LowPriority, MLPipelineStage::Classifier, MLPipelineStage::Writer);
    AutotagsPipelinePackageBase* package = nullptr;

    /* =========================================================================================
     * Pipeline stage specific initialization code
     *
     * Use the block from here to MLPIPELINE_LOOP_START to initialize the stage.
     * The code in this block is run once per stage initialization. The number of instances
     * is at least 1. More instances are created by addMoreWorkers if needed.
     */

    MLPIPELINE_LOOP_START(MLPipelineStage::Classifier, thisQueue);
    package = static_cast<AutotagsPipelinePackageBase*>(mlpackage);

    /* =========================================================================================
     * Start pipeline stage specific loop
     *
     * All code from here to MLPIPELINE_LOOP_END is in a try/catch block and loop.
     * This loop is run once per image.
     */

    {
        package->labelList = autotagsClassifier->predictMulti(package->featuresList);
        package->tagList   = autotagsClassifier->getClassStrings(package->labelList);

        // send the package to the next stage

        enqueue(nextQueue, package);
    }

    /* =========================================================================================
     * End pipeline stage specific loop
     */

    MLPIPELINE_LOOP_END(MLPipelineStage::Classifier, "AutotagsPipelineObject::classifier");

    /* =========================================================================================
     * Pipeline stage specific cleanup
     * 
     * Use the block from here to MLPIPELINE_STAGE_END to clean up any resources used by the stage.
     */

    MLPIPELINE_STAGE_END(MLPipelineStage::Classifier, MLPipelineStage::Writer);
}

bool AutotagsPipelineObject::writer()
{
    MLPIPELINE_STAGE_START(QThread::LowPriority, MLPipelineStage::Writer, MLPipelineStage::None);
    AutotagsPipelinePackageBase* package = nullptr;

    /* =========================================================================================
     * Pipeline stage specific initialization code
     *
     * Use the block from here to MLPIPELINE_LOOP_START to initialize the stage.
     * The code in this block is run once per stage initialization. The number of instances
     * is at least 1. More instances are created by addMoreWorkers if needed.
     */

    const QString rootTag      = QLatin1String("auto/");
    TagsCache* const tagsCache = TagsCache::instance();

    MLPIPELINE_LOOP_START(MLPipelineStage::Writer, thisQueue);
    package                    = static_cast<AutotagsPipelinePackageBase*>(mlpackage);

    /* =========================================================================================
     * Start pipeline stage specific loop
     *
     * All code from here to MLPIPELINE_LOOP_END is in a try/catch block and loop.
     * This loop is run once per image.
     */

    {
        bool tagsChanged           = false;
        QStringList tagsPath;
        QStringList displayTags;
        const int rootTagId        = tagsCache->getOrCreateTag(rootTag);

        // Clear auto-tags

        const auto ids = package->info.tagIds();

        if (!settings.bqmMode && (AutotagsScanSettings::TagMode::Replace == settings.tagMode))
        {
            for (int tid : ids)
            {
                if (tagsCache->parentTags(tid).contains(rootTagId))
                {
                    package->info.removeTag(tid);
                    tagsChanged = true;
                }
            }
        }

        tagsPath.clear();
        displayTags.clear();

        for (const auto& tag : std::as_const(package->tagList))
        {
            int tagId = -1;

            if (!settings.languages.isEmpty())
            {
                for (const QString& trLang : std::as_const(settings.languages))
                {
                    QString trOut;
                    QString error;
                    bool trRet = s_inlineTranslateString(tag, trLang, trOut, error);

                    if (trRet)
                    {
                        QString newTag = rootTag + trLang + QLatin1Char('/') + trOut;
                        tagsPath    << newTag;
                        displayTags << trOut;
                        tagId          = tagsCache->getOrCreateTag(newTag);
                    }
                    else
                    {
                        qCDebug(DIGIKAM_AUTOTAGSENGINE_LOG) << "Auto-Tags online translation error:"
                                                            << error;
                        QString newTag = rootTag + trLang + QLatin1Char('/') + tag;
                        tagsPath    << newTag;
                        displayTags << tag;
                        tagId          = tagsCache->getOrCreateTag(newTag);
                    }
                }
            }
            else
            {
                QString newTag = rootTag + tag;
                tagsPath    << newTag;
                displayTags << tag;
                tagId          = tagsCache->getOrCreateTag(newTag);
            }

            if (!settings.bqmMode && (tagId != -1) && !package->info.tagIds().contains(tagId))
            {
                package->info.setTag(tagId);
                tagsChanged = true;
            }
        }

        // Write tags to the metadata too

        if (!settings.bqmMode)
        {
            if (tagsChanged)
            {
                MetadataHub hub;
                hub.load(package->info);

                ScanController::FileMetadataWrite writeScope(package->info);
                writeScope.changed(hub.writeToMetadata(package->info, MetadataHub::WRITE_TAGS));
            }
        }
        else
        {
            if (tagsPath.size() > 0)
            {
/*
                QStringList oldTags = bqmMeta->getItemTagsPath();
                oldTags.append(tagsPath);
*/
                bqmMeta->setItemTagsPath(tagsPath);
                bqmMeta->save(bqmOutputUrl.toLocalFile());
            }
        }

        // combine the image name with the tags for the notification

        QString displayName(package->info.name() + QStringLiteral("\n"));

        if (!displayTags.isEmpty())
        {
            displayName += displayTags.join(QLatin1String(", "));
            if (displayTags.size() > 4)
            {
                qCDebug(DIGIKAM_AUTOTAGSENGINE_LOG) << "AutotagsPipelineObject::writer: adding " << displayTags.size() << " tags to the image.";
                qCDebug(DIGIKAM_AUTOTAGSENGINE_LOG) << "AutotagsPipelineObject::writer: " << package->info.relativePath() << "/" << displayName;
            }
        }

        // send a notification that the image was processed

        notify(MLPipelineNotification::notifyProcessed,
               displayName,
               package->info.relativePath(),
               1,
               package->thumbnailIcon);

        // delete the package

        delete package;
    }

    /* =========================================================================================
     * End pipeline stage specific loop
     */

    MLPIPELINE_LOOP_END(MLPipelineStage::Writer, "AutotagsPipelineObject::writer");

    /* =========================================================================================
     * Pipeline stage specific cleanup
     *
     * Use the block from here to MLPIPELINE_STAGE_END to clean up any resources used by the stage.
     */

    MLPIPELINE_STAGE_END(MLPipelineStage::Writer, MLPipelineStage::None);
}

void AutotagsPipelineObject::addMoreWorkers()
{
    /* =========================================================================================
     * Use the performanceProfile metrics to find the slowest stages
     * and add more workers to those stages.
     * 
     * For the Autotags object detection pipeline, the loader is the
     * slowest stage so add 3 more loaders and 2 more extractors.
     */

    addWorker(Loader);
    addWorker(Loader);
    addWorker(Loader);
    addWorker(Extractor);
    addWorker(Extractor);
}

} // namespace Digikam

#include "moc_autotagspipelineobject.cpp"
