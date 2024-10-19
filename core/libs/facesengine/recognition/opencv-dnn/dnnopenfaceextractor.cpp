/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-06-01
 * Description : Face recognition using deep learning
 *               The internal DNN library interface
 *
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2020-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024      by Michael Miller <michael underscore miller at msn dot com>
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

    cv::dnn::Net             net;
    QMutex                   mutex;

    // As we use OpenFace, we need to set appropriate values for image color space and image size.

    cv::Size                 imageSize          = cv::Size(96, 96);
    float                    scaleFactor        = 1.0F / 255.0F;
    cv::Scalar               meanValToSubtract  = cv::Scalar(0.0, 0.0, 0.0);
};

DNNOpenFaceExtractor::DNNOpenFaceExtractor()
    : DNNFaceExtractorBase(),
      d                   (new Private)
{
    // Virtual call in contructor: use dynamic binding.

    this->loadModels();
}

DNNOpenFaceExtractor::DNNOpenFaceExtractor(const DNNOpenFaceExtractor& other)
    : DNNFaceExtractorBase(),
      d                   (other.d)
{
    ++(d->ref);
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
    QString appPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                             QLatin1String("digikam/facesengine"),
                                             QStandardPaths::LocateDirectory);

    d->preprocessor = new RecognitionPreprocessor;
    d->preprocessor->init(PreprocessorSelection::OPENFACE);

    QString model   = QLatin1String("openface_nn4.small2.v1.t7");
    QString nnmodel = appPath + QLatin1Char('/') + model;

    if (QFileInfo::exists(nnmodel))
    {
        try
        {
            qCDebug(DIGIKAM_FACEDB_LOG) << "Extractor model:" << nnmodel;

            d->net = cv::dnn::readNetFromTorch(nnmodel.toStdString());

#if (OPENCV_VERSION == QT_VERSION_CHECK(4, 7, 0))

            d->net.enableWinograd(false);

#endif

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
        qCCritical(DIGIKAM_FACEDB_LOG) << "Cannot found faces engine DNN model" << model;
        qCCritical(DIGIKAM_FACEDB_LOG) << "Faces recognition feature cannot be used!";

        return false;
    }

    return true;
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

    if (!d->net.empty())
    {
        QMutexLocker lock(&d->mutex);
        d->net.setInput(blob);
        face_descriptors = d->net.forward();
    }

    qCDebug(DIGIKAM_FACEDB_LOG) << "Finish computing face embedding in "
                                << timer.elapsed() << " ms";

    return face_descriptors;
}

} // namespace Digikam
