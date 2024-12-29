/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-06-01
 * Description : Face recognition using deep learning.
 *               The internal DNN library interface.
 *
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2020-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dnnopenfaceextractor.h"

// Qt includes

#include <QMutex>
#include <QString>
#include <QFileInfo>
#include <QMutexLocker>
#include <QElapsedTimer>
#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"
#include "recognitionpreprocessor.h"
#include "dnnmodelmanager.h"
#include "dnnmodelnet.h"

namespace Digikam
{

class Q_DECL_HIDDEN DNNOpenFaceExtractor::Private
{
public:

    Private() = default;

    ~Private()
    {
        delete preprocessor;
    }

public:

    RecognitionPreprocessor* preprocessor       = nullptr;

    int                      ref                = 1;

    DNNModelBase*            model              = nullptr;

    // As we use OpenFace, we need to set appropriate values for image color space and image size.

    cv::Size                 imageSize          = cv::Size(96, 96);
    float                    scaleFactor        = 1.0F / 255.0F;
    cv::Scalar               meanValToSubtract  = cv::Scalar(0.0, 0.0, 0.0);
};

DNNOpenFaceExtractor::DNNOpenFaceExtractor()
    : DNNFaceExtractorBase(),
      d(new Private)
{
    // Virtual call in contructor: use dynamic binding.

    this->loadModels();
}

DNNOpenFaceExtractor::~DNNOpenFaceExtractor()
{
    --(d->ref);

    if (d->ref == 0)
    {
        delete d;
    }
}

bool DNNOpenFaceExtractor::loadModels()
{
    d->preprocessor = new RecognitionPreprocessor;
    d->preprocessor->init(PreprocessorSelection::OPENFACE);

    d->model        = DNNModelManager::instance()->getModel(QLatin1String("OpenFace"),
                                                            DNNModelUsage::DNNUsageFaceRecognition);

    if (d->model)
    {
        try
        {
            if (!d->model->modelLoaded)
            {
                cv::dnn::Net net = static_cast<DNNModelNet*>(d->model)->getNet();
                qCDebug(DIGIKAM_FACEDB_LOG) << "Extractor model:" << d->model->info.displayName;
            }
        }

        catch (cv::Exception& e)
        {
            qCWarning(DIGIKAM_FACEDB_LOG) << "cv::Exception:" << e.what();

            return false;
        }

        catch (...)
        {
            qCWarning(DIGIKAM_FACEDB_LOG) << "Default exception from OpenCV";

            return false;
        }
    }

    else
    {
        qCCritical(DIGIKAM_FACEDB_LOG) << "OpenFace cannot find faces engine DNN model";
        qCCritical(DIGIKAM_FACEDB_LOG) << "Faces recognition feature cannot be used!";

        return false;
    }

    return true;
}

float DNNOpenFaceExtractor::getThreshold(int uiThreshold) const
{
    if (d->model)
    {
        return d->model->getThreshold(uiThreshold);
    }

    return 0.0f;
}

cv::Mat DNNOpenFaceExtractor::alignFace(const cv::Mat& inputImage) const
{
    return d->preprocessor->preprocess(inputImage);
}

cv::Mat DNNOpenFaceExtractor::getFaceEmbedding(const cv::Mat& faceImage)
{
    cv::Mat face_descriptors;
    cv::Mat alignedFace;
    /*
        qCDebug(DIGIKAM_FACEDB_LOG) << "faceImage channels: " << faceImage.channels();
        qCDebug(DIGIKAM_FACEDB_LOG) << "faceImage size: (" << faceImage.rows << ", " << faceImage.cols << ")\n";
    */
    QElapsedTimer timer;

    timer.start();

    alignedFace = d->preprocessor->preprocess(faceImage);

    qCDebug(DIGIKAM_FACEDB_LOG) << "Finish aligning face in " << timer.elapsed() << " ms";
    qCDebug(DIGIKAM_FACEDB_LOG) << "Start neural network";

    timer.start();

    cv::Mat blob = cv::dnn::blobFromImage(alignedFace, d->scaleFactor, d->imageSize, cv::Scalar(), true, false);

    if (d->model && d->model->modelLoaded)
    {
        QMutexLocker lock(&(d->model->mutex));
        static_cast<DNNModelNet*>(d->model)->getNet().setInput(blob);
        face_descriptors = static_cast<DNNModelNet*>(d->model)->getNet().forward();
    }

    qCDebug(DIGIKAM_FACEDB_LOG) << "Finish computing face embedding in "
                                << timer.elapsed() << " ms";

    return face_descriptors;
}

} // namespace Digikam
