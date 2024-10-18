/* ============================================================
 *
 * This file is a part of digiKam
 * https://www.digikam.org
 *
 * Date        : 2023-09-02
 * Description : Derived class to perform YOLO neural network inference
 *               for object detection (here yolo version 5).
 *
 * SPDX-FileCopyrightText: 2023 by Quoc Hung TRAN <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dnnresnetdetector.h"

// C++ includes

#include <fstream>

// Qt includes

#include <QDir>
#include <QStandardPaths>
#include <QFileInfo>
#include <QElapsedTimer>
#include <QMutexLocker>

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"
#include "dnnmodelmanager.h"
#include "dnnmodelnet.h"
#include "dnnmodelconfig.h"

namespace Digikam
{

DNNResnetDetector::DNNResnetDetector()
    : DNNBaseDetectorModel(1.0F / 255.0F,
                           cv::Scalar(0.0 ,0.0 ,0.0),
                           cv::Size(224, 224))
{
    if (loadModels())
    {
        predefinedClasses = loadImageNetClass();
    }
}

DNNResnetDetector::~DNNResnetDetector()
{
}

QList<QString> DNNResnetDetector::loadImageNetClass()
{
    QList<QString> classList;
    QString classFile;

    // NOTE: storing all model definitions at the same application path as face engine
    
    if (model && model->modelLoaded)
    {
        const DNNModelConfig* configModel = static_cast<DNNModelConfig*>(DNNModelManager::instance()->getModel(model->info.classList, DNNModelUsage::DNNUsageObjectDetection));
        if (configModel)
        {
            classFile = configModel->getModelPath();
        }
    }

    if (QFileInfo::exists(classFile))
    {
        std::ifstream ifs(classFile.toStdString());
        std::string line;

        while (getline(ifs, line))
        {
            classList.append(QString::fromStdString(line));
        }
    }

    return classList;
}

QList<QString> DNNResnetDetector::getPredefinedClasses() const
{
    return predefinedClasses;
}

bool DNNResnetDetector::loadModels()
{

    model = DNNModelManager::instance()->getModel(QLatin1String("ResNet50"), DNNModelUsage::DNNUsageObjectDetection);

    if (model && !model->modelLoaded)
    {
        try
        {
            cv::dnn::Net net = static_cast<DNNModelNet*>(model)->getNet();  // check we can load the DNN net
        }
        catch (cv::Exception& e)
        {
            qCWarning(DIGIKAM_AUTOTAGSENGINE_LOG) << "cv::Exception:" << e.what();

            return false;
        }
        catch (...)
        {
           qCWarning(DIGIKAM_AUTOTAGSENGINE_LOG) << "Default exception from OpenCV";

           return false;
        }
    }

    if (model && !model->modelLoaded)
    {
        qCCritical(DIGIKAM_AUTOTAGSENGINE_LOG) << "Cannot find object classification DNN model";

        return false;
    }

    return true;
}

QHash<QString, QVector<QRect> > DNNResnetDetector::detectObjects(const::cv::Mat& inputImage)
{
    if (inputImage.empty())
    {
        qDebug() << "Invalid image given, not detecting objects";

        return {};
    }

    std::vector<cv::Mat> outs = preprocess(inputImage);

    return postprocess(inputImage, outs[0]);
}

QList<QHash<QString, QVector<QRect> > > DNNResnetDetector::detectObjects(const std::vector<cv::Mat>& inputBatchImages)
{
    if (inputBatchImages.empty())
    {
        qCDebug(DIGIKAM_AUTOTAGSENGINE_LOG) << "Invalid image list given, not detecting objects";

        return {};
    }

    std::vector<cv::Mat> outs = preprocess(inputBatchImages);

    // outs = [1 x [rows x 85]]

    return postprocess(inputBatchImages, outs);
}

std::vector<cv::Mat> DNNResnetDetector::preprocess(const cv::Mat& inputImage)
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
        qCWarning(DIGIKAM_AUTOTAGSENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCWarning(DIGIKAM_AUTOTAGSENGINE_LOG) << "Default exception from OpenCV";
    }

    return outs;
}

std::vector<cv::Mat> DNNResnetDetector::preprocess(const std::vector<cv::Mat>& inputBatchImages)
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
        qCWarning(DIGIKAM_AUTOTAGSENGINE_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCWarning(DIGIKAM_AUTOTAGSENGINE_LOG) << "Default exception from OpenCV";
    }

    return outs;
}

QList<QHash<QString, QVector<QRect> > > DNNResnetDetector::postprocess(const std::vector<cv::Mat>& inputBatchImages,
                                                                       const std::vector<cv::Mat>& outs) const
{
    QList<QHash<QString, QVector<QRect> > > detectedBoxesList;

    // outs = [batch_size x [rows x 85]]

    if (!outs.empty())
    {
        for (unsigned int i = 0 ; i < inputBatchImages.size() ; i++)
        {
            detectedBoxesList.append(postprocess(inputBatchImages[i], outs[0].row(i)));
        }
    }

    return detectedBoxesList;
}

QHash<QString, QVector<QRect> > DNNResnetDetector::postprocess(const cv::Mat& /*inputImage*/,
                                                               const cv::Mat& out) const
{
    QHash<QString, QVector<QRect> > detectedBoxes;

    if (predefinedClasses.isEmpty())
    {
        return detectedBoxes;
    }

    cv::Point classIdPoint;
    double final_prob = 0.0;

    minMaxLoc(out.reshape(1, 1), 0, &final_prob, 0, &classIdPoint);
    int label_id      = classIdPoint.x;

    QString label     = predefinedClasses[label_id];
    detectedBoxes[label].push_back( { } );

    return detectedBoxes;
}

std::vector<cv::String> DNNResnetDetector::getOutputsNames() const
{
    static std::vector<cv::String> names;

    if (model && model->modelLoaded && names.empty())
    {
        // Get the indices of the output layers, i.e. the layers with unconnected outputs

        std::vector<int> outLayers          = static_cast<DNNModelNet*>(model)->getNet().getUnconnectedOutLayers();

        // Get the names of all the layers in the network

        std::vector<cv::String> layersNames = static_cast<DNNModelNet*>(model)->getNet().getLayerNames();

        // Get the names of the output layers in names

        names.resize(outLayers.size());

        for (size_t i = 0 ; i < outLayers.size() ; ++i)
        {
            names[i] = layersNames[outLayers[i] - 1];
        }
    }

    return names;
}

} // namespace Digikam
