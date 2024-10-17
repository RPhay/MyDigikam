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

#include "dnnsfaceextractor.h"

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
#include "dnnmodelsface.h"
#include "dnnmodelyunet.h"

namespace Digikam
{

class Q_DECL_HIDDEN DNNSFaceExtractor::Private
{
public:

    Private() = default;

    ~Private()
    {
    }

public:

    int                             ref             = 1;
    DNNModelBase*                   model           = nullptr;
    DNNModelYuNet*                  detectorModel   = nullptr;

    // cv::Ptr<cv::FaceRecognizerSF>   net         = nullptr;
    // cv::Ptr<cv::FaceDetectorYN>     cv_model    = nullptr;
    // QMutex                          mutex;
};

DNNSFaceExtractor::DNNSFaceExtractor()
    : DNNFaceExtractorBase(),
      d                   (new Private)
{
    // Virtual call in contructor: use dynamic binding.

    this->loadModels();
}

DNNSFaceExtractor::DNNSFaceExtractor(const DNNSFaceExtractor& other)
    : DNNFaceExtractorBase(),
      d                   (other.d)
{
    ++(d->ref);
}

DNNSFaceExtractor::~DNNSFaceExtractor()
{
    --(d->ref);

    if (d->ref == 0)
    {
        delete d;
    }
}

bool DNNSFaceExtractor::loadModels()
{
    d->model = DNNModelManager::instance()->getModel(QLatin1String("SFace"), DNNModelUsage::DNNUsageFaceRecognition);

    if (d->model && !(d->model->modelLoaded))
    {
        try
        {
            cv::Ptr<cv::FaceRecognizerSF> net = static_cast<DNNModelSFace*>(d->model)->getNet();
            qCDebug(DIGIKAM_FACEDB_LOG) << "Extractor model:" << d->model->info.displayName;

            d->detectorModel = static_cast<DNNModelYuNet*>(DNNModelManager::instance()->getModel(QLatin1String("YuNet"), DNNModelUsage::DNNUsageFaceDetection));
            cv::Ptr<cv::FaceDetectorYN> detNet = static_cast<DNNModelYuNet*>(d->detectorModel)->getNet();
            qCDebug(DIGIKAM_FACEDB_LOG) << "Recognition model:" << d->detectorModel->info.displayName;

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
        if (d->model)
        {
            qCCritical(DIGIKAM_FACEDB_LOG) << "Cannot find faces engine DNN model"
                                           << d->model->info.displayName;
        }

        qCCritical(DIGIKAM_FACEDB_LOG) << "Faces recognition feature cannot be used!";

        return false;
    }

    return true;
}

cv::Mat DNNSFaceExtractor::alignFace(const cv::Mat& inputImage) const
{
    cv::Mat alignedFace;
    static_cast<DNNModelSFace*>(d->model)->getNet()->alignCrop(inputImage, inputImage, alignedFace);

    return alignedFace;
}

cv::Mat DNNSFaceExtractor::getFaceEmbedding(const cv::Mat& faceImage)
{
    cv::Mat face_descriptors;
    cv::Mat paddedFace;
    cv::Mat alignedFace;

    QElapsedTimer timer;

    // start the timer for profiling

    timer.start();

    // resize the thumbnail if necessary to match SFace detection
    // SFace wants 112x112px images.  Resize so 112 is the smallest dimension

    if (std::min(faceImage.cols, faceImage.rows) > 112)
    {
        // Image should be resized.  YuNet image sizes are much more flexible than SSD and YOLO
        // so we just need to make sure no one bound exceeds the max. No padding needed

        float resizeFactor      = std::min(static_cast<float>(112) / static_cast<float>(faceImage.cols),
                                           static_cast<float>(112) / static_cast<float>(faceImage.rows));

        int newWidth            = (int)(resizeFactor * faceImage.cols);
        int newHeight           = (int)(resizeFactor * faceImage.rows);
        cv::resize(faceImage, paddedFace, cv::Size(newWidth, newHeight));
    }
    else
    {
        paddedFace = faceImage.clone();
    }

    // add a border so there is room to rotate the image during alignment

    cv::copyMakeBorder(paddedFace, paddedFace,
                       60, 60,
                       60, 60,
                       cv::BORDER_CONSTANT,
                       cv::Scalar(0, 0, 0));

    try 
    {
        if (
            d->model                                              &&
            d->detectorModel                                      &&
            !d->detectorModel->getNet().empty()                   &&
            !static_cast<DNNModelSFace*>(d->model)->getNet().empty()
           )
        {
            QMutexLocker detectorLock(&d->detectorModel->mutex);

            // redetect face using YuNet to get landmarks

            cv::Mat faceLandmark;

            d->detectorModel->getNet()->setInputSize(paddedFace.size());
            d->detectorModel->getNet()->detect(paddedFace, faceLandmark);

            detectorLock.unlock();

            if (0 < faceLandmark.rows)
            {
                QMutexLocker lock(&d->model->mutex);

                // align and crop the face to standard size

                static_cast<DNNModelSFace*>(d->model)->getNet()->alignCrop(paddedFace, faceLandmark, alignedFace);

                qCDebug(DIGIKAM_FACEDB_LOG) << "Finish aligning face in " << timer.elapsed() << " ms";
                qCDebug(DIGIKAM_FACEDB_LOG) << "Start neural network";

                timer.start();

                static_cast<DNNModelSFace*>(d->model)->getNet()->feature(alignedFace, face_descriptors);
                normalize(face_descriptors, face_descriptors);
            }
        }
    }
    catch (cv::Exception& e)
    {
        qCritical(DIGIKAM_FACEDB_LOG) << "cv::Exception:" << e.what();
    }
    catch (...)
    {
        // qCDebug(DIGIKAM_FACEDB_LOG) << e.what();
    }

    qCDebug(DIGIKAM_FACEDB_LOG) << "Finish computing face embedding in "
                                << timer.elapsed() << " ms";


    return face_descriptors;
}

} // namespace Digikam
