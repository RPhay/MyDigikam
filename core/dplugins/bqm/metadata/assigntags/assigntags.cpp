/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-10-03
 * Description : auto assign tags batch tool.
 *
 * SPDX-FileCopyrightText: 2022-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2023      by Quoc Hung Tran <quochungtran1999 at gmail dot com>
 * SPDX-FileCopyrightText: 2025      by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "assigntags.h"

// Qt includes

#include <QComboBox>
#include <QWidget>
#include <QLabel>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "dimg.h"
#include "dnuminput.h"
#include "dmetadata.h"
#include "dpluginbqm.h"
#include "dlayoutbox.h"
#include "dfileoperations.h"
#include "localizeselector.h"
#include "previewloadthread.h"
#include "autotagsscansettings.h"
#include "autotagsscanwidget.h"
#include "autotagspipelineobject.h"

namespace DigikamBqmAssignTagsPlugin
{

class Q_DECL_HIDDEN AssignTags::Private
{
public:

    Private() = default;
    ~Private();

    bool                    changeSettings      = true;
    AutotagsPipelineObject* pipeline            = nullptr;
    AutotagsScanWidget*     autotagsScanWidget  = nullptr;
};

AssignTags::Private::~Private()
{
    if (pipeline)
    {
        pipeline->cancel();
        delete pipeline;
        pipeline = nullptr;
    }
}

AssignTags::AssignTags(QObject* const parent)
    : BatchTool(QLatin1String("AssignTags"), MetadataTool, parent),
      d        (new Private)
{
}

AssignTags::~AssignTags()
{
    delete d;
}

BatchTool* AssignTags::clone(QObject* const parent) const
{
    return new AssignTags(parent);
}

void AssignTags::registerSettingsWidget()
{
    DVBox* const vbox  = new DVBox;
    d->autotagsScanWidget = new AutotagsScanWidget(AutotagsScanWidget::SettingsDisplayMode::BQM, vbox);

    m_settingsWidget   = vbox;

    connect(d->autotagsScanWidget, SIGNAL(signalSettingsChanged()),
            this, SLOT(slotSettingsChanged()));

    BatchTool::registerSettingsWidget();
}

BatchToolSettings AssignTags::defaultSettings()
{
    BatchToolSettings settings;
    AutotagsScanSettings autotagsSettings;

    // Not needed in BQM

    // settings.insert(QLatin1String("AutotagsAlbums"), autotagsSettings.albums);
    // settings.insert(QLatin1String("AutotagsAlbumMode"), autotagsSettings.wholeAlbums);
    // settings.insert(QLatin1String("AutotagsScanMode"), autotagsSettings.scanMode);

    settings.insert(QLatin1String("AutotagsTagMode"), autotagsSettings.tagMode);
    settings.insert(QLatin1String("AutotagsObjectDetectModel"), autotagsSettings.objectDetectModel);
    settings.insert(QLatin1String("AutotagsObjectDetectAccuracy"), autotagsSettings.uiConfidenceThreshold);
    settings.insert(QLatin1String("TrAutotagsLangs"), autotagsSettings.languages);
    
    return settings;
}

void AssignTags::slotAssignSettings2Widget()
{
    AutotagsScanSettings autotagsSettings;
    autotagsSettings.objectDetectModel      = settings().value(QLatin1String("AutotagsObjectDetectModel")).toString();
    autotagsSettings.tagMode                = (AutotagsScanSettings::TagMode)settings().value(QLatin1String("AutotagsTagMode")).toInt();
    autotagsSettings.uiConfidenceThreshold  = settings().value(QLatin1String("AutotagsObjectDetectAccuracy")).toInt();
    autotagsSettings.languages              = settings().value(QLatin1String("TrAutotagsLangs")).toStringList();
    autotagsSettings.bqmMode                = true;

    d->autotagsScanWidget->settings(autotagsSettings);
}

void AssignTags::slotSettingsChanged()
{
    if (d->changeSettings)
    {
        BatchToolSettings settings;

        AutotagsScanSettings autotagsSettings = d->autotagsScanWidget->settings();

        // Not needed in BQM
        
        // settings.insert(QLatin1String("AutotagsAlbums"), autotagsSettings.albums);
        // settings.insert(QLatin1String("AutotagsAlbumMode"), autotagsSettings.wholeAlbums);
        // settings.insert(QLatin1String("AutotagsScanMode"), autotagsSettings.scanMode);

        settings.insert(QLatin1String("AutotagsTagMode"), autotagsSettings.tagMode);
        settings.insert(QLatin1String("AutotagsObjectDetectModel"), autotagsSettings.objectDetectModel);
        settings.insert(QLatin1String("AutotagsObjectDetectAccuracy"), autotagsSettings.uiConfidenceThreshold);
        settings.insert(QLatin1String("TrAutotagsLangs"), autotagsSettings.languages);

        BatchTool::slotSettingsChanged(settings);
    }
}

bool AssignTags::toolOperations()
{

    if (nullptr == d->pipeline)
    {
        AutotagsScanSettings pipelineSettings;
        pipelineSettings.objectDetectModel      = settings().value(QLatin1String("AutotagsObjectDetectModel")).toString();
        pipelineSettings.tagMode                = (AutotagsScanSettings::TagMode)settings().value(QLatin1String("AutotagsTagMode")).toInt();
        pipelineSettings.uiConfidenceThreshold  = settings().value(QLatin1String("AutotagsObjectDetectAccuracy")).toInt();
        pipelineSettings.languages              = settings().value(QLatin1String("TrAutotagsLangs")).toStringList();
        pipelineSettings.bqmMode                = true;

        d->pipeline = new AutotagsPipelineObject(pipelineSettings);
        d->pipeline->start();
    }

    bool ret = true;
    QScopedPointer<DMetadata> meta(new DMetadata);

    if (image().isNull())
    {
        QFile::remove(outputUrl().toLocalFile());
        ret = DFileOperations::copyFile(inputUrl().toLocalFile(), outputUrl().toLocalFile());

        if (!ret || !meta->load(outputUrl().toLocalFile()))
        {
            return ret;
        }
    }
    else
    {
        ret = savefromDImg();
    }

    if (AutotagsScanSettings::TagMode::Update == (AutotagsScanSettings::TagMode)settings().value(QLatin1String("AutotagsTagMode")).toInt())
    {
        if (image().isNull())
        {
            meta->load(inputUrl().toLocalFile());
        }
        else
        {
            meta->setData(image().getMetadata());
        }
    }

    d->pipeline->bqmSendOne(meta, imageInfo(), outputUrl(), image());
    
    // DImg img          = image();
    // AutotagsScanSettings::ObjectDetectionModel detectModel         = (AutotagsScanSettings::ObjectDetectionModel)settings().value(QLatin1String("AutoTagModel")).toInt();
    // QStringList langs = settings().value(QLatin1String("TrAutoTagsLangs")).toStringList();

    // int uiDetectThreshold = settings().value(QLatin1String("AutoTagAccuracy")).toInt();

    // DNNModelNet* model = nullptr;
    // AutotagsClassifierBase* autotagsClassifier = nullptr;

    // try
    // {
    //     switch (detectModel)
    //     {
    //         case AutotagsScanSettings::ObjectDetectionModel::YOLOV11NANO:
    //         {
    //             model = static_cast<DNNModelNet*>(DNNModelManager::instance()->getModel(QStringLiteral("YOLOv11-nano"), DNNModelUsage::DNNUsageObjectDetection));
    //             break;
    //         }
    //         case AutotagsScanSettings::ObjectDetectionModel::YOLOV11XLARGE:
    //         {
    //             model = static_cast<DNNModelNet*>(DNNModelManager::instance()->getModel(QStringLiteral("YOLOv11-xl"), DNNModelUsage::DNNUsageObjectDetection));
    //             break;
    //         }
    //         case AutotagsScanSettings::ObjectDetectionModel::RESNET152:
    //         {
    //             model = static_cast<DNNModelNet*>(DNNModelManager::instance()->getModel(QStringLiteral("ResNet152_v2"), DNNModelUsage::DNNUsageImageClassification));
    //             break;
    //         }
    //         default:
    //         {
    //             qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "AutotagsPipelineObject::start(): Unknown object detection model. ";
    //             return false;
    //         }
    //     }

    //     model->getNet();

    //     const DNNModelConfig* configModel = static_cast<DNNModelConfig*>(DNNModelManager::instance()->getModel(model->info.classList,
    //                                                                                                            DNNModelUsage::DNNUsageObjectDetection));

    //     if (configModel)
    //     {
    //         if (AutotagsScanSettings::ObjectDetectionModel::YOLOV11NANO == detectModel || AutotagsScanSettings::ObjectDetectionModel::YOLOV11XLARGE == detectModel)
    //         {
    //             autotagsClassifier = new AutotagsClassifierYolo(model->getThreshold(uiDetectThreshold), configModel->getModelPath());
    //             static_cast<AutotagsClassifierYolo*>(autotagsClassifier)->setParams(AutotagsClassifierYolo::YoloVersion::YOLOv11, QSize(model->info.imageSize, model->info.imageSize));
    //         }
    //         else
    //         {
    //             autotagsClassifier = new AutotagsClassifierSoftmax(model->getThreshold(uiDetectThreshold), configModel->getModelPath());
    //         }
    //     }
    // }
    // catch(const std::exception& e)
    // {
    //     qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "AutotagsPipelineObject::start(): Unable to load model. " << e.what();
    //     if (model)
    //     {
    //         model = nullptr;
    //     }
    // }
    // catch(...)
    // {
    //     qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "AutotagsPipelineObject::start(): Unknown error. Unable to load model. ";
    //     if (model)
    //     {
    //         model = nullptr;
    //     }
    // }

    // // check if the model and classifier were created

    // if (nullptr == model || nullptr == autotagsClassifier)
    // {
    //     return false;
    // }

    // if (ret && img.isNull())
    // {
    //     img = PreviewLoadThread::loadFastSynchronously(outputUrl().toLocalFile(), model->info.imageSize);
    // }

    // cv::Mat cvImage = QtOpenCVImg::image2Mat(img, CV_8UC3, QtOpenCVImg::MatColorOrder::MCO_RGB);

    // // resize the image if needed.  Only resize if the image is larger than the input size of the detector

    // cv::Size inputImageSize = cv::Size(model->info.imageSize, model->info.imageSize);

    // if (std::max(cvImage.cols, cvImage.rows) > std::max(inputImageSize.width, inputImageSize.height))
    // {
    //     // Image should be resized. 

    //     float resizeFactor      = std::min(static_cast<float>(inputImageSize.width)  / static_cast<float>(cvImage.cols),
    //                                         static_cast<float>(inputImageSize.height) / static_cast<float>(cvImage.rows));

    //     int newWidth            = (int)(resizeFactor * cvImage.cols);
    //     int newHeight           = (int)(resizeFactor * cvImage.rows);
    //     cv::resize(cvImage, cvImage, cv::Size(newWidth, newHeight));
    // }

    // // pad the image if needed

    // if (model->info.imageSize != cvImage.cols || model->info.imageSize != cvImage.rows)
    // {
    //     // Image needs to be padded so we add a border
    //     cv::Mat borderImage;
    //     int xPad = model->info.imageSize - cvImage.cols;
    //     int yPad = model->info.imageSize - cvImage.rows;
        
    //     cv::copyMakeBorder(cvImage, borderImage,
    //                     0, yPad,
    //                     0, xPad,
    //                     cv::BORDER_CONSTANT,
    //                     cv::Scalar(0, 0, 0));
    //     cvImage = borderImage;
    // }

    // // convert the image to a blob 
    // cv::Mat cvBlob = cv::dnn::blobFromImage(cvImage, 1.0/255, cv::Size(cvImage.cols, cvImage.rows), cv::Scalar(0, 0, 0), true, false);

    // std::vector<cv::Mat> detectionResults;

    // {
    //     // detect any objects in the image

    //     QMutexLocker lock(&(model->mutex));

    //     model->getNet().setInput(cvBlob);

    //     model->getNet().forward(detectionResults, model->getNet().getUnconnectedOutLayersNames());
    // }


    // QList<int> labelList = autotagsClassifier->predictMulti(QList<cv::Mat>() << detectionResults.at(0));
    // QList<QString> tagsList = autotagsClassifier->getClassStrings(labelList);

    // if (ret && !img.isNull())
    // {
    //     if (!tagsList.isEmpty())
    //     {
    //         QString path = outputUrl().toLocalFile();
    //         qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Path to process with Auto-Tags:" << path;

    //         QStringList tagsPath;
    //         const QString rootTag = QLatin1String("auto/");

    //         for (const auto& tag : tagsList)
    //         {
    //             if (!langs.isEmpty())
    //             {
    //                 for (const QString& trLang : std::as_const(langs))
    //                 {
    //                     QString trOut;
    //                     QString error;
    //                     bool trRet = s_inlineTranslateString(tag, trLang, trOut, error);

    //                     if (trRet)
    //                     {
    //                         tagsPath << (rootTag + trLang +
    //                                      QLatin1Char('/') +  trOut);
    //                     }
    //                     else
    //                     {
    //                         qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Auto-Tags online translation error:"
    //                                                          << error;
    //                         tagsPath << (rootTag + trLang +
    //                                      QLatin1Char('/') +  tag);
    //                     }
    //                 }
    //             }
    //             else
    //             {
    //                 tagsPath << (rootTag + tag);
    //             }
    //         }

    //         if (!tagsPath.isEmpty())
    //         {
    //             meta->setItemTagsPath(tagsPath);
    //             ret = meta->save(outputUrl().toLocalFile());
    //         }
    //     }
    // }

    return ret;
}

} // namespace DigikamBqmAssignTagsPlugin

#include "moc_assigntags.cpp"
