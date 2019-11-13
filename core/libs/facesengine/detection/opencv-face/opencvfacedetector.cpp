/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-01-03
 * Description : Class to perform faces detection.
 *               Based on work from Modesto Castrillón, Oscar Déniz, Daniel Hernández, and Javier Lorenzo
 *               A comparison of face and facial feature detectors based on the
 *               https://en.wikipedia.org/wiki/Viola-Jones_object_detection_framework
 *               Machine Vision and Applications, 01/2008
 *               DOI 10.1007/s00138-010-0250-7
 *
 * Copyright (C) 2010-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2010 by Aditya Bhatt <adityabhatt at gmail dot com>
 * Copyright (C)      2010 by Alex Jironkin <alexjironkin at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "opencvfacedetector_p.h"

namespace Digikam
{

OpenCVFaceDetector::OpenCVFaceDetector(const QStringList& cascadeDirs)
    : d(new Private)
{
    if (cascadeDirs.isEmpty())
    {
        qCCritical(DIGIKAM_FACESENGINE_LOG) << "OpenCV Haar Cascade directory cannot be found. "
                                               "Did you install OpenCV XML data files?";
        return;
    }

    d->cascades << Cascade(cascadeDirs, QLatin1String("haarcascade_frontalface_alt.xml"));
    d->cascades << Cascade(cascadeDirs, QLatin1String("haarcascade_frontalface_default.xml"));
    d->cascades << Cascade(cascadeDirs, QLatin1String("haarcascade_frontalface_alt2.xml"));
    d->cascades << Cascade(cascadeDirs, QLatin1String("haarcascade_frontalface_alt_tree.xml"));

    d->cascades << Cascade(cascadeDirs, QLatin1String("haarcascade_profileface.xml"));

    d->cascades << Cascade(cascadeDirs, QLatin1String("haarcascade_mcs_lefteye.xml"));
    d->cascades << Cascade(cascadeDirs, QLatin1String("haarcascade_mcs_righteye.xml"));
    d->cascades << Cascade(cascadeDirs, QLatin1String("haarcascade_mcs_nose.xml"));
    d->cascades << Cascade(cascadeDirs, QLatin1String("haarcascade_mcs_mouth.xml"));

    d->cascades[2].setPrimaryCascade();

    d->cascades[5].setROI(0,   0,    0.6, 0.6);
    d->cascades[6].setROI(0.4, 0,    0.6, 0.6);
    d->cascades[7].setROI(0.2, 0.25, 0.6, 0.6);
    d->cascades[8].setROI(0.1, 0.4,  0.8, 0.6);
}

OpenCVFaceDetector::~OpenCVFaceDetector()
{
    delete d;
}

double OpenCVFaceDetector::accuracy() const
{
    return d->speedVsAccuracy;
}

double OpenCVFaceDetector::specificity() const
{
    return d->sensitivityVsSpecificity;
}

void OpenCVFaceDetector::setAccuracy(double speedVsAccuracy)
{
    d->speedVsAccuracy = qBound(0.0, speedVsAccuracy, 1.0);
}

void OpenCVFaceDetector::setSpecificity(double sensitivityVsSpecificity)
{
    d->sensitivityVsSpecificity = qBound(0.0, sensitivityVsSpecificity, 1.0);
}

void OpenCVFaceDetector::updateParameters(const cv::Size& /*scaledSize*/,
                                          const cv::Size& originalSize)
{
    double origSize = double(cv::max(originalSize.width, originalSize.height)) / 1000;

    /*
     * Search increment will determine the number of passes over the image.
     * But with fewer passes, we will miss some faces.
     */

    if (d->speedVsAccuracy <= 0.159)
    {
        d->primaryParams.searchIncrement = 1.5;
    }
    else if (d->speedVsAccuracy >= 0.8)
    {
        d->primaryParams.searchIncrement = 1.1;
    }
    else
    {
        d->primaryParams.searchIncrement = round(100 * (1.1 - 0.5*log10(d->speedVsAccuracy))) / 100;
    }

    /*
     * This is a clear tradeoff. With 1, we'll get many faces,
     * but more false positives than faces.
     * 3 is the best parameter for normal use.
     */

    if (d->sensitivityVsSpecificity < 0.25)
    {
        d->primaryParams.grouping = 1;
    }
    else if (d->sensitivityVsSpecificity < 0.5)
    {
        d->primaryParams.grouping = 2;
    }
    else
    {
        d->primaryParams.grouping = 3;
    }

    /*
     * Flag speeds up (very much faster) and potentially lowers sensitivity: We mostly use it,
     * unless in we want very high sensitivity at low speed
     */

    if ((d->sensitivityVsSpecificity > 0.1) || (d->speedVsAccuracy < 0.9))
    {
        d->primaryParams.flags = cv::CASCADE_DO_CANNY_PRUNING;
    }
    else
    {
        d->primaryParams.flags = 0;
    }

    /*
     * Greater min size will filter small images, lowering sensitivity, enhancing specificity,
     * with false positives often small
     */

    double minSize = 32 * d->sensitivityVsSpecificity;

    /*
     * Original small images deserve a smaller minimum size
     */

    minSize       -= 10 * (1.0 - cv::min(1.0, origSize));

    /*
     * A small min size means small starting size, together with search increment, determining
     * the number of operations and thus speed
     */

    if (d->speedVsAccuracy < 0.75)
    {
        minSize += 100 * (0.75 - d->speedVsAccuracy);
    }

    /*
     * Cascade minimum is 20 for most of our cascades (one is 24).
     * Passing 0 will use the cascade minimum.
     */

    if (minSize < 20)
    {
        minSize = 0;
    }

    d->primaryParams.minSize           = cv::Size(lround(minSize), lround(minSize));

    d->maxDistance                     = 15;    // Maximum distance between two faces to call them unique
    d->minDuplicates                   = 0;

    d->verifyingParams.searchIncrement = 1.1;
    d->verifyingParams.flags           = 0;

    // NOTE: min size is adjusted each time

/*
    qCDebug(DIGIKAM_FACESENGINE_LOG) << "updateParameters: accuracy " << d->speedVsAccuracy
                                     << " sensitivity " << d->sensitivityVsSpecificity
                                     << " - searchIncrement " << d->primaryParams.searchIncrement
                                     << " grouping " << d->primaryParams.grouping
                                     << " flags " << d->primaryParams.flags
                                     << " min size " << d->primaryParams.minSize.width << endl
                                     << " primary cascades: ";

    for (unsigned int i = 0 ; i < d->cascadeProperties.size() ; ++i)
    {
        if (d->cascadeProperties[i].primaryCascade)
        {
            qCDebug(DIGIKAM_FACESENGINE_LOG) << d->cascadeSet->getCascade(i).name << " ";
        }
    }

    qCDebug(DIGIKAM_FACESENGINE_LOG) << " maxDistance " << d->maxDistance << " minDuplicates " << d->minDuplicates;
*/

/*
    if (d->speedVsAccuracy < 0.5)
    {
        d->primaryCascades[0] = true;
        d->minDuplicates = 0;
    }
    else
    {
        d->primaryCascades[1] = true;
        d->primaryCascades[2] = true;

        if (d->sensitivityVsSpecificity > 0.5)
        {
            d->minDuplicates = 1;
        }
    }
*/
}

QList<QRect> OpenCVFaceDetector::cascadeResult(const cv::Mat& inputImage,
                                               Cascade& cascade,
                                               const DetectObjectParameters& params) const
{
    // Check whether the cascade has loaded successfully. Else report and error and quit

    if (cascade.empty())
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Cascade XML data are not loaded.";
        return QList<QRect>();
    }

    QMutexLocker locker(&d->mutex);

    // There can be more than one face in an image. So create a growable sequence of faces.
    // Detect the objects and store them in the sequence

    qCDebug(DIGIKAM_FACESENGINE_LOG) << "detectMultiScale: image size (" << inputImage.cols << "," << inputImage.rows
                                     << ") searchIncrement" << params.searchIncrement
                                     << "grouping" << params.grouping
                                     << "flags" << params.flags
                                     << "min size (" << params.minSize.width << "," << params.minSize.height << ")";

    std::vector<cv::Rect> faces;
    cascade.detectMultiScale(inputImage, faces,
                             params.searchIncrement,                // Increase search scale by this factor every time.
                             params.grouping,                       // Drop groups of less than n detections.
                             params.flags,                          // Optionally, pre-test regions by edge detection.
                             params.minSize                         // Minimum face size to look for.
                            );

    QList<QRect> results;

    for (std::vector<cv::Rect>::const_iterator it = faces.begin() ; it != faces.end() ; ++it)
    {
        results << s_opencvFaceDetectorToQRect(*it);
    }

    qCDebug(DIGIKAM_FACESENGINE_LOG) << "detectMultiScale gave" << results;

    return results;
}

bool OpenCVFaceDetector::verifyFace(const cv::Mat& inputImage, const QRect& face) const
{
    // check if we need to verify

    bool hasVerifyingCascade = false;

    for (int i = 0 ; i < d->cascades.size() ; ++i)
    {
        if (d->cascades[i].verifyingCascade)
        {
            hasVerifyingCascade = true;
            break;
        }
    }

    if (!hasVerifyingCascade)
    {
        return true;
    }

    // Face coordinates. Add a certain margin for the other frontal cascades.

    const cv::Rect faceRect = s_opencvFaceDetectorFromQRect(face);
    const cv::Size faceSize = cv::Size(face.width(), face.height());
    const int margin        = cv::min(40, cv::max(faceRect.width, faceRect.height));

    // Clip to bounds of image, after adding the margin

    cv::Rect extendedRect   = cv::Rect(cv::max(0, faceRect.x - margin),
                                       cv::max(0, faceRect.y - margin),
                                               faceRect.width  + 2*margin,
                                               faceRect.height + 2*margin);

    extendedRect.width      = cv::min(inputImage.cols - extendedRect.x, extendedRect.width);
    extendedRect.height     = cv::min(inputImage.rows - extendedRect.y, extendedRect.height);


    // shallow copy by ROI

    cv::Mat extendedFaceImg = inputImage(extendedRect);

    QList<QRect> foundFaces;
    int frontalFaceVotes    = 0;
    int facialFeatureVotes  = 0;

    for (int i = 0 ; i < d->cascades.size() ; ++i)
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Verifying face" << face << "using cascade" << i;

        if (d->cascades[i].verifyingCascade)
        {
            d->verifyingParams.minSize = d->cascades[i].minSizeForFace(faceSize);

            if (d->cascades[i].isFacialFeature())
            {
                d->verifyingParams.grouping = 2;

                cv::Rect roi      = d->cascades[i].faceROI(faceRect);
                cv::Mat  feature  = inputImage(roi);
                qCDebug(DIGIKAM_FACESENGINE_LOG) << "feature" << d->cascades[i].roi
                                                 << s_opencvFaceDetectorToQRect(faceRect)
                                                 << s_opencvFaceDetectorToQRect(roi);
                foundFaces        = cascadeResult(feature, d->cascades[i], d->verifyingParams);

                if (!foundFaces.isEmpty())
                {
                    facialFeatureVotes++;
                }
/*
                // This is pretty much working code that scales up the face if it's too small
                // for the  facial feature cascade. It did not bring me benefit with false positives though.

                double factor     = cascade.requestedInputScaleFactor(faceSize);
                IplImage* feature = LibFaceUtils::scaledSection(inputImage, roi, factor);

                // qCDebug(DIGIKAM_FACESENGINE_LOG) << "Facial feature in roi " << cascade.roi << "scaled up to" << feature->width << feature->height;

                foundFaces = cascadeResult(feature, cascade.cascade, d->verifyingParams);

                for (vector<Face>::iterator it = foundFaces.begin() ; it != foundFaces.end() ; ++it)
                {
                    qCDebug(DIGIKAM_FACESENGINE_LOG) << "Feature face " << it->getX1() << " " << it->getY1() << " " << it->getWidth() << "x" << it->getHeight();

                    double widthScaled  = it->getWidth()  / factor;
                    double heightScaled = it->getHeight() / factor;

                    // qCDebug(DIGIKAM_FACESENGINE_LOG) << "Hit feature size " << widthScaled << " " << heightScaled << " "
                    //                                  << (faceSize.width / CascadeProperties::faceToFeatureRelationMin()) << " "
                    //                                  << (faceSize.width / CascadeProperties::faceToFeatureRelationMax());

                    if (
                        (widthScaled  > faceSize.width  / Cascade::faceToFeatureRelationMin() &&
                         widthScaled  < faceSize.width  / Cascade::faceToFeatureRelationMax())
                        ||
                        (heightScaled > faceSize.height / Cascade::faceToFeatureRelationMin() &&
                         heightScaled < faceSize.height / Cascade::faceToFeatureRelationMax())
                        )
                    {
                        facialFeatureVotes++;
                        qCDebug(DIGIKAM_FACESENGINE_LOG) << "voting";
                        break;
                    }
                }
*/
            }
            else
            {
                d->verifyingParams.grouping = 3;

                foundFaces = cascadeResult(extendedFaceImg, d->cascades[i], d->verifyingParams);

                // We don't need to check the size of found regions, the minSize in verifyingParams is large enough

                if (!foundFaces.empty())
                {
                    frontalFaceVotes++;
                }
            }
/*
            qCDebug(DIGIKAM_FACESENGINE_LOG) << "Verifying cascade " << cascade.name << " gives " << foundFaces.size();
*/
        }
    }

    bool verified;

    // Heuristic: Discard a sufficiently large face that shows no facial features

    if (faceSize.width <= 50 && facialFeatureVotes == 0)
    {
        verified = false;
    }
    else
    {
        if      (frontalFaceVotes && facialFeatureVotes)
        {
            verified = true;
        }
        else if (frontalFaceVotes >= 2)
        {
            verified = true;
        }
        else if (facialFeatureVotes >= 2)
        {
            verified = true;
        }
        else
        {
            verified = false;
        }
    }

/*
    qCDebug(DIGIKAM_FACESENGINE_LOG) << "Verification finished. Votes: Frontal " << frontalFaceVotes << " Features "
                                     << facialFeatureVotes << ". Face verified: " << verified;
*/

    return verified;
}

QList<QRect> OpenCVFaceDetector::mergeFaces(const cv::Mat& inputImage, const QList<QList<QRect> >& combo) const
{
    Q_UNUSED(inputImage);

    QList<QRect> results;

    // Make one long vector of all faces

    foreach (const QList<QRect>& list, combo)
    {
        results += list;
    }

    // used only one cascade? No need to merge then

    int primaryCascades = 0;

    foreach (const Cascade& cascade, d->cascades)
    {
        if (cascade.primaryCascade)
        {
            primaryCascades++;
        }
    }

    if (primaryCascades <= 1)
    {
        return results;
    }

    /*
     * Now, starting from the left, take a face and compare with rest. If distance is less than a threshold,
     * consider them to be "overlapping" face frames and delete the "duplicate" from the vector.
     * Remember that only faces to the RIGHT of the reference face will be deleted.
     */

    QList<int> genuineness;
    int ctr = 0;

    QList<QRect>::iterator first;
    QList<QRect>::iterator second;

    for (first = results.begin() ; first != results.end() ; )
    {
        int duplicates = 0;

        for (second = first + 1 ; second != results.end() ; )  // Compare with the faces to the right
        {
            ++ctr;

            if (s_opencvFaceDetectorDistanceOfCenters(*first, *second) < d->maxDistance)
            {
                second = results.erase(second);
                ++duplicates;
            }
            else
            {
                ++second;
            }
        }

        if (duplicates < d->minDuplicates)    // Less duplicates, probably not genuine, kick it out
        {
            first = results.erase(first);
        }
        else
        {
            // Face passed both tests, will be in final results
            ++first;
        }
    }

    qCDebug(DIGIKAM_FACESENGINE_LOG) << "Faces parsed:" << ctr << "number of final faces:" << results.size();

    return results;
}

int OpenCVFaceDetector::recommendedImageSizeForDetection()
{
    return 800;
}

cv::Mat OpenCVFaceDetector::prepareForDetection(const QImage& inputImage) const
{
    if (inputImage.isNull() || !inputImage.size().isValid())
    {
        return cv::Mat();
    }

    QImage image(inputImage);
    int inputArea                    = image.width() * image.height();
    const int maxAcceptableInputArea = 1024*768;

    if (inputArea > maxAcceptableInputArea)
    {
        // Resize to 1024 * 768 (or comparable area for different aspect ratio)
        // Looking for scale factor z where A = w*z * h*z => z = sqrt(A/(w*h))

        qreal z          = qSqrt(qreal(maxAcceptableInputArea) / image.width() / image.height());
        QSize scaledSize = image.size() * z;
        image            = image.scaled(scaledSize, Qt::KeepAspectRatio);
    }

    // TODO: move to common utils, opentldrecognition

    cv::Mat cvImageWrapper, cvImage;

    switch (image.format())
    {
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32:
        case QImage::Format_ARGB32_Premultiplied:
            // I think we can ignore premultiplication when converting to grayscale
            cvImageWrapper = cv::Mat(image.height(), image.width(), CV_8UC4, image.scanLine(0), image.bytesPerLine());
            cvtColor(cvImageWrapper, cvImage, CV_RGBA2GRAY);
            break;

        default:
            image          = image.convertToFormat(QImage::Format_RGB888);
            cvImageWrapper = cv::Mat(image.height(), image.width(), CV_8UC3, image.scanLine(0), image.bytesPerLine());
            cvtColor(cvImageWrapper, cvImage, CV_RGB2GRAY);
            break;
    }

    equalizeHist(cvImage, cvImage);

    return cvImage;
}

cv::Mat OpenCVFaceDetector::prepareForDetection(const DImg& inputImage) const
{
    if (inputImage.isNull() || !inputImage.size().isValid())
    {
        return cv::Mat();
    }

    DImg image(inputImage);
    int inputArea                    = image.width() * image.height();
    const int maxAcceptableInputArea = 1024*768;

    if (inputArea > maxAcceptableInputArea)
    {
        // Resize to 1024 * 768 (or comparable area for different aspect ratio)
        // Looking for scale factor z where A = w*z * h*z => z = sqrt(A/(w*h))

        qreal z          = qSqrt(qreal(maxAcceptableInputArea) / image.width() / image.height());
        QSize scaledSize = image.size() * z;
        image            = image.smoothScale(scaledSize, Qt::KeepAspectRatio);
    }

    // TODO: move to common utils, opentldrecognition

    cv::Mat cvImageWrapper;
    cv::Mat cvImage;
    int type = image.sixteenBit() ? CV_16UC3 : CV_8UC3;
    type     = image.hasAlpha()   ? type     : type + 8;

    switch (type)
    {
        case CV_8UC4:
        case CV_16UC4:
            cvImageWrapper = cv::Mat(image.height(), image.width(), type, image.bits());
            cvtColor(cvImageWrapper, cvImage, CV_RGBA2GRAY);
            break;

        case CV_8UC3:
        case CV_16UC3:
            cvImageWrapper = cv::Mat(image.height(), image.width(), type, image.bits());
            cvtColor(cvImageWrapper, cvImage, CV_RGB2GRAY);
            break;
    }

    if ((type == CV_16UC4) || (type == CV_16UC3))
    {
        cvImage.convertTo(cvImage, CV_8UC1, 1/255.0);
    }

    equalizeHist(cvImage, cvImage);

    return cvImage;
}

QList<QRect> OpenCVFaceDetector::detectFaces(const cv::Mat& inputImage, const cv::Size& originalSize)
{
    if (inputImage.empty())
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Invalid image given, not detecting faces.";
        return QList<QRect>();
    }

    updateParameters(inputImage.size(), originalSize);

    // Now loop through each cascade, apply it, and get back a vector of detected faces

    QList<QList<QRect> > primaryResults;
    QList<QRect> results;

    for (int i = 0 ; i < d->cascades.size() ; ++i)
    {
        if (d->cascades[i].primaryCascade)
        {
            primaryResults << cascadeResult(inputImage, d->cascades[i], d->primaryParams);
        }
    }

    // Merge overlaps of face regions by different cascades.

    results = mergeFaces(inputImage, primaryResults);

    // Verify faces using other cascades

    for (QList<QRect>::iterator it = results.begin() ; it != results.end() ; )
    {
        if (!verifyFace(inputImage, *it))
        {
            it = results.erase(it);
        }
        else
        {
            ++it;
        }
    }

    return results;
}

} // namespace Digikam
