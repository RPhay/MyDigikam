/* ============================================================
 *
 * This file is a part of digiKam
 * https://www.digikam.org
 *
 * Date        : 2023-09-02
 * Description : Low-level DNN base class for object detection and classification.
 *
 * SPDX-FileCopyrightText: 2023 by Quoc Hung TRAN <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dnnbasedetectormodel.h"

// Qt includes

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QElapsedTimer>
#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"
#include "dnnmodelnet.h"
#include "dnnmodelconfig.h"


namespace Digikam
{

/**
 * @note Setting 1000 will use the value from dnnmodels.conf if passed in.
 */
int DNNBaseDetectorModel::uiConfidenceThreshold = DNN_MODEL_THRESHOLD_NOT_SET;
float DNNBaseDetectorModel::nmsThreshold        = 0.4F;
float DNNBaseDetectorModel::scoreThreshold      = 0.45F;

// --------------------------------------------------------------------------------------------------------

DNNBaseDetectorModel::DNNBaseDetectorModel()
    : scaleFactor(1.0F)
{
}

DNNBaseDetectorModel::DNNBaseDetectorModel(float scale,
                                           const cv::Scalar& val,
                                           const cv::Size& inputImageSize)
    : scaleFactor      (scale),
      meanValToSubtract(val),
      inputImageSize   (inputImageSize)
{
}

QList<QString> DNNBaseDetectorModel::generateObjects(const cv::Mat& inputImage)
{
    QHash<QString, QVector<QRect> > results = detectObjects(inputImage);
    QList<QString> objectNames;

    for (QHash<QString, QVector<QRect> >::const_iterator it = results.constBegin() ;
         it != results.constEnd() ; ++it)
    {
        objectNames.append(it.key());
    }

    return objectNames;
}

QList<QList<QString> > DNNBaseDetectorModel::generateObjects(const std::vector<cv::Mat>& inputBatchImages)
{
    QList<QHash<QString, QVector<QRect> > > results = detectObjects(inputBatchImages);
    QList<QList<QString> > objectNamesList;

    for (const auto& detectedBoxes : std::as_const(results))
    {
        QList<QString> objectNames;

        for (QHash<QString, QVector<QRect> >::const_iterator it = detectedBoxes.constBegin() ;
             it != detectedBoxes.constEnd() ; ++it)
        {
            objectNames.append(it.key());
        }

        objectNamesList.append(objectNames);
    }

    return objectNamesList;
}

cv::Size DNNBaseDetectorModel::getinputImageSize() const
{
    return inputImageSize;
}

double DNNBaseDetectorModel::showInferenceTime()
{
    // Put efficiency information.
    // The function getPerfProfile returns the overall time for
    // inference(t) and the timings for each of the layers(in layersTimes).

    std::vector<double> layersTimes;
    double freq = cv::getTickFrequency() / 1000.0;

    return (static_cast<DNNModelNet*>(model)->getNet().getPerfProfile(layersTimes) / freq);
}

QList<QString> DNNBaseDetectorModel::loadDetectionClasses()
{
    QList<QString> classList;

    // NOTE: storing all model definitions at the same application path as face engine.

    if (model && model->modelLoaded)
    {
        const DNNModelConfig* configModel = static_cast<DNNModelConfig*>(DNNModelManager::instance()->getModel(model->info.classList,
                                                                                                               DNNModelUsage::DNNUsageObjectDetection));

        if (configModel)
        {
            QFile classFile(configModel->getModelPath());

            if (classFile.exists() && classFile.open(QIODevice::ReadOnly))
            {
                QTextStream stream(&classFile);

                while (!stream.atEnd())
                {
                    classList.append(stream.readLine());
                }
            }
        }
    }

    return classList;
}

QList<QString> DNNBaseDetectorModel::getPredefinedClasses() const
{
    return predefinedClasses;
}

QHash<QString, QVector<QRect> > DNNBaseDetectorModel::detectObjects(const::cv::Mat& inputImage)
{
    if (inputImage.empty())
    {
        qCDebug(DIGIKAM_AUTOTAGSENGINE_LOG) << "Invalid image given, not detecting objects";

        return {};
    }

    std::vector<cv::Mat> outs = preprocess(inputImage);

    return postprocess(inputImage, outs[0]);
}

QList<QHash<QString, QVector<QRect> > > DNNBaseDetectorModel::detectObjects(const std::vector<cv::Mat>& inputBatchImages)
{
    if (inputBatchImages.empty())
    {
        qCDebug(DIGIKAM_AUTOTAGSENGINE_LOG) << "Invalid image list given, not detecting objects";

        return {};
    }

    std::vector<cv::Mat> outs = preprocess(inputBatchImages);

    // NOTE: outs = [1 x [rows x 85]]

    return postprocess(inputBatchImages, outs);
}

std::vector<cv::Mat> DNNBaseDetectorModel::preprocess(const cv::Mat& inputImage)
{
    std::vector<cv::Mat> outs;

    try
    {
        cv::Mat inputBlob = cv::dnn::blobFromImage(inputImage,
                                                   scaleFactor,
                                                   inputImageSize,
                                                   meanValToSubtract,
                                                   true,
                                                   false);

        if (model && model->modelLoaded)
        {
            QMutexLocker lock(&model->mutex);
            static_cast<DNNModelNet*>(model)->getNet().setInput(inputBlob);
            static_cast<DNNModelNet*>(model)->getNet().forward(outs, getOutputsNames());
        }
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "Default exception from OpenCV";
    }

    return outs;
}

std::vector<cv::Mat> DNNBaseDetectorModel::preprocess(const std::vector<cv::Mat>& inputBatchImages)
{
    std::vector<cv::Mat> outs;

    try
    {
        cv::Mat inputBlob = cv::dnn::blobFromImages(inputBatchImages,
                                                    scaleFactor,
                                                    inputImageSize,
                                                    meanValToSubtract,
                                                    true,
                                                    false);

        if (model && model->modelLoaded)
        {
            QMutexLocker lock(&model->mutex);
            QElapsedTimer timer;
            timer.start();

            static_cast<DNNModelNet*>(model)->getNet().setInput(inputBlob);
            static_cast<DNNModelNet*>(model)->getNet().forward(outs, getOutputsNames());

            int elapsed = timer.elapsed();

            qCDebug(DIGIKAM_AUTOTAGSENGINE_LOG) << "Batch forward (Inference) takes: " << elapsed << " ms";
        }
    }
    catch (cv::Exception& e)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "Default exception from OpenCV";
    }

    return outs;
}

QList<QHash<QString, QVector<QRect> > > DNNBaseDetectorModel::postprocess(const std::vector<cv::Mat>& inputBatchImages,
                                                                          const std::vector<cv::Mat>& outs) const
{
    QList<QHash<QString, QVector<QRect> > > detectedBoxesList;

    // NOTE: outs = [batch_size x [rows x 85]]

    if (!outs.empty())
    {
        for (unsigned int i = 0 ; i < inputBatchImages.size() ; i++)
        {
            detectedBoxesList.append(postprocess(inputBatchImages[i], outs[0].row(i)));
        }
    }

    return detectedBoxesList;
}

std::vector<cv::String> DNNBaseDetectorModel::getOutputsNames() const
{
    static std::vector<cv::String> names;

    if (model && model->modelLoaded && names.empty())
    {
        // Get the indices of the output layers, i.e. the layers with unconnected outputs.

        std::vector<int> outLayers          = static_cast<DNNModelNet*>(model)->getNet().getUnconnectedOutLayers();

        // Get the names of all the layers in the network.

        std::vector<cv::String> layersNames = static_cast<DNNModelNet*>(model)->getNet().getLayerNames();

        // Get the names of the output layers in names.

        names.resize(outLayers.size());

        for (size_t i = 0 ; i < outLayers.size() ; ++i)
        {
            names[i] = layersNames[outLayers[i] - 1];
        }
    }

    return names;
}

} // namespace Digikam
