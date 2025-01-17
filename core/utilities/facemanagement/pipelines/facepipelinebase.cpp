/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-11-10
 * Description : Integrated, multithread face detection / recognition
 *
 * SPDX-FileCopyrightText: 2024-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "facepipelinebase.h"

// Qt includes

#include <QtConcurrent>
#include <QThread>
#include <QList>

// Local includes

#include "digikam_debug.h"
#include "digikam_opencv.h"
#include "sharedqueue.h"
#include "facepipelinepackagebase.h"
#include "thumbnailloadthread.h"
#include "dnnsfaceextractor.h"
#include "dnnmodelmanager.h"

namespace Digikam
{

FacePipelineBase::FacePipelineBase(const FaceScanSettings& _settings)
    : MLPipelineFoundation(),
      settings            (_settings)
{
}

FacePipelineBase::~FacePipelineBase()
{
}

double FacePipelineBase::detectNoise(const cv::Mat& cvGrayImage) const
{
    // Use a Gaussian filter to detect noise

    // apply Gaussian blur to the image

    cv::Mat blurred;
    cv::GaussianBlur(cvGrayImage, blurred, cv::Size(5, 5), 0);

    // calculate the difference between the original and blurred image

    cv::Mat noise = cvGrayImage - blurred;

    // calculate the standard deviation of the noise

    cv::Mat mean, stddev;
    cv::meanStdDev(noise, mean, stddev);

    double noiseLevel = stddev.at<double>(0, 0);

    return noiseLevel;
}

#define BLOCK 20

double FacePipelineBase::detectBlur(const cv::Mat& cvGrayImage) const
{
    // Use a Fast Fourier Transform to detect blurriness

    int cx = cvGrayImage.cols/2;
    int cy = cvGrayImage.rows/2;

    // Convert the image to a flat float

    cv::Mat fImage;
    cvGrayImage.convertTo(fImage, CV_32F);

    // FFT

    cv::Mat fourierTransform;
    cv::dft(fImage, fourierTransform, cv::DFT_SCALE | cv::DFT_COMPLEX_OUTPUT);

    // center low frequencies in the middle
    // by shuffling the quadrants.

    cv::Mat q0(fourierTransform, cv::Rect(0, 0, cx, cy));       // Top-Left - Create a ROI per quadrant
    cv::Mat q1(fourierTransform, cv::Rect(cx, 0, cx, cy));      // Top-Right
    cv::Mat q2(fourierTransform, cv::Rect(0, cy, cx, cy));      // Bottom-Left
    cv::Mat q3(fourierTransform, cv::Rect(cx, cy, cx, cy));     // Bottom-Right

    // swap quadrants (Top-Left with Bottom-Right)

    cv::Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    // swap quadrant (Top-Right with Bottom-Left)

    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);

    // Block the low frequencies
    // #define BLOCK could also be a argument on the command line of course

    fourierTransform(cv::Rect(cx - BLOCK, cy - BLOCK, 2 * BLOCK, 2 * BLOCK)).setTo(0);

    //shuffle the quadrants to their original position

    cv::Mat orgFFT;
    fourierTransform.copyTo(orgFFT);
    cv::Mat p0(orgFFT, cv::Rect(0, 0, cx, cy));       // Top-Left - Create a ROI per quadrant
    cv::Mat p1(orgFFT, cv::Rect(cx, 0, cx, cy));      // Top-Right
    cv::Mat p2(orgFFT, cv::Rect(0, cy, cx, cy));      // Bottom-Left
    cv::Mat p3(orgFFT, cv::Rect(cx, cy, cx, cy));     // Bottom-Right

    // swap quadrant (Top-Left with Bottom-Right)

    p0.copyTo(tmp);
    p3.copyTo(p0);
    tmp.copyTo(p3);

    // swap quadrant (Top-Right with Bottom-Left)

    p1.copyTo(tmp);
    p2.copyTo(p1);
    tmp.copyTo(p2);

    // IFFT

    cv::Mat invFFT;
    cv::Mat logFFT;
    double minVal,maxVal;

    cv::dft(orgFFT, invFFT, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);

    invFFT = cv::abs(invFFT);
    cv::minMaxLoc(invFFT, &minVal, &maxVal, NULL, NULL);
    
    //check for impossible values

    if(maxVal <= 0.0)
    {
        return 1;
    }

    cv::log(invFFT,logFFT);
    logFFT *= 20;

    cv::Scalar result = cv::mean(logFFT);

    return result.val[0];
    
}

bool FacePipelineBase::useForTraining(const cv::Rect origSize, const cv::Mat& cvImage)
{
    if (!detectorModel)
    {
        detectorModel = DNNModelManager::instance()->getModel(QStringLiteral("yunet"), DNNModelUsage::DNNUsageFaceDetection);        
    }

    // thumbnail must be at least minThumbnailSize of the size the detector expects

    if (detectorModel->info.imageSize * minThumbnailSize > origSize.width ||
        detectorModel->info.imageSize * minThumbnailSize > origSize.height)
    {
        return false;
    }

    // convert to grayscale for use in noise and blur detection
    
    cv::Mat cvGrayImage;
    cv::cvtColor(cvImage, cvGrayImage, cv::COLOR_RGB2GRAY);

    // use a Gaussian filter to check for noisy images

    double noise = detectNoise(cvGrayImage);

    if (noise > noiseThreshold)
    {
        return false;
    }

    // use a FFT filter to check for blurred images

    double blur = detectBlur(cvGrayImage);

    // If the variance is less than the threshold, the image is considered blurred

    if (blur < blurThreshold)
    {
        return false;
    }

    return true;
}

bool FacePipelineBase::commonFaceThumbnailLoader(const QString& pipelineName, MLPipelineStage thisStage, MLPipelineStage nextStage)
{
    // All threads start with the same basic functions

    MLPipelineQueue* thisQueue = nullptr, *nextQueue = nullptr;
    stageStart(QThread::LowPriority, thisStage, nextStage, thisQueue, nextQueue);
    FacePipelinePackageBase* package = nullptr;
    QElapsedTimer timer;

    //--------------------------------------------------------------------------------

    ThumbnailLoadThread* const thumbnailLoadThread = new ThumbnailLoadThread;
    // ThumbnailLoadThread* thumbnailLoadThread = ThumbnailLoadThread::defaultThread();

    thumbnailLoadThread->setPixmapRequested(false);
    thumbnailLoadThread->setThumbnailSize(ThumbnailLoadThread::maximumThumbnailSize());
    thumbnailLoadThread->setPriority(QThread::NormalPriority);

    ThumbnailImageCatcher* const catcher           = new ThumbnailImageCatcher(thumbnailLoadThread);

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

            pipelinePerformanceStart(thisStage, timer);

            //////////////////////////////////////////////////////////////////////////////////////////////
            // start pipeline stage specific code

            catcher->thread()->find(ItemInfo::thumbnailIdentifier(package->face.imageId()), package->face.region().toRect());
            catcher->enqueue();
            QList<QImage> images = catcher->waitForThumbnails();

            if (images.size() && !images[0].isNull())
            {
                package->thumbnail     = images[0];
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

            pipelinePerformanceEnd(thisStage, timer);
        }

        catch (const std::exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << pipelineName << "::loader(): unknown error. "
                                                << e.what() << "    Restarting...";

            if (package)
            {
                delete package;
            }
        }

        catch (...)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << pipelineName << "::loader(): unknown error.  Restarting...";

            if (package)
            {
                delete package;
            }
        }
    }

    catcher->setActive(false);

    catcher->thread()->stopAllTasks();
    catcher->cancel();

    delete catcher->thread();
    delete catcher;

    //--------------------------------------------------------------------------------
    // all threads end with the same basic functions

    stageEnd(thisStage, nextStage);

    return true;
}

bool FacePipelineBase::commonFaceThumbnailExtractor(const QString& pipelineName,
                                                    MLPipelineStage thisStage,
                                                    MLPipelineStage nextStage,
                                                    bool trainingQualityCheck)
{
    // All threads start with the same basic functions

    MLPipelineQueue* thisQueue = nullptr, *nextQueue = nullptr;
    stageStart(QThread::LowPriority, thisStage, nextStage, thisQueue, nextQueue);
    FacePipelinePackageBase* package = nullptr;
    QElapsedTimer timer;

    //--------------------------------------------------------------------------------

    DNNSFaceExtractor extractor;

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
            
            cv::UMat cvUImage;

            QImage inputImage(package->thumbnail.copy());

            // preprocess image to be in the correct format

            if (inputImage.format() != QImage::Format_RGB888)
            {
                inputImage = inputImage.convertToFormat(QImage::Format_RGB888);
            }

            // create a cv::Mat image from the QImage and move it to the GPU with a cv::UMat

            cvUImage = cv::Mat(inputImage.height(), inputImage.width(), CV_8UC3, inputImage.scanLine(0), inputImage.bytesPerLine()).getUMat(cv::ACCESS_FAST);

            // extract the face features

            package->features = extractor.getFaceEmbedding(cvUImage);
            cv::Rect origSize(0, 0, package->face.region().toRect().width(), package->face.region().toRect().height());

            if (trainingQualityCheck)
            {
                package->useForTraining = useForTraining(origSize, cvUImage.getMat(cv::ACCESS_FAST));
            }

            enqueue(nextQueue, package);

            // end pipeline stage specific code
            //////////////////////////////////////////////////////////////////////////////////////////////

            pipelinePerformanceEnd(MLPipelineStage::Extractor, timer);
        }

        catch (const std::exception& e)
        {
            qCCritical(DIGIKAM_FACESENGINE_LOG) << pipelineName << "::extractor(): unknown error. "
                                                << e.what() << "    Restarting...";

            if (package)
            {
                delete package;
            }
        }

        catch (...)
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
    if (nullptr != package)
    {
        // calculate the package size.  Only big items need to be checked

        package->size = static_cast<FacePipelinePackageBase*>(package)->image.size().width() * static_cast<FacePipelinePackageBase*>(package)->image.size().height() * 4;
    }

    return MLPipelineFoundation::enqueue(thisQueue, package);
}

} // namespace Digikam

#include "moc_facepipelinebase.cpp"
