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
 * Copyright (C) 2010-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

DetectObjectParameters::DetectObjectParameters()
    : searchIncrement(0),
      grouping(0),
      flags(0),
      minSize(cv::Size(0, 0))
{
}

// --------------------------------------------------------------------------------

QString s_opencvFaceDetectorFindFileInDirs(const QStringList& dirs, const QString& fileName)
{
    foreach (const QString& dir, dirs)
    {
        const QString file = dir + (dir.endsWith(QLatin1Char('/')) ? QLatin1String("")
                                                                   : QLatin1String("/"))
                                 + fileName;

        if (QFile::exists(file))
        {
            return file;
        }
    }

    return QString();
}

int s_opencvFaceDetectorDistanceOfCenters(const QRect& r1, const QRect& r2)
{
    QPointF diff = r1.center() - r2.center();

    return (lround(sqrt(diff.x() * diff.x() + diff.y() * diff.y())));    // Euclidean distance
}

QRect s_opencvFaceDetectorToQRect(const cv::Rect& rect)
{
    return (QRect(rect.x, rect.y, rect.width, rect.height));
}

cv::Rect s_opencvFaceDetectorFromQRect(const QRect& rect)
{
    return (cv::Rect(rect.x(), rect.y(), rect.width(), rect.height()));
}

// --------------------------------------------------------------------------------

Cascade::Cascade(const QStringList& dirs, const QString& fileName)
    : primaryCascade(false),
      verifyingCascade(true)
{
    const QString file = s_opencvFaceDetectorFindFileInDirs(dirs, fileName);

    if (file.isEmpty())
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Failed to locate cascade" << fileName << "in" << dirs;
        return;
    }

    qCDebug(DIGIKAM_FACESENGINE_LOG) << "Loading cascade" << file;

    if (!load(file.toStdString()))
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Failed to load cascade" << file;
        return;
    }
}

cv::Size Cascade::getOriginalWindowSize() const
{
    return cv::Size(0, 0);
}

void Cascade::setPrimaryCascade(bool isPrimary)
{
    primaryCascade   = isPrimary;
    verifyingCascade = !isPrimary;
}

void Cascade::setROI(double x, double y, double width, double height)
{
    roi = QRectF(x, y, width, height);
}

bool Cascade::isFacialFeature() const
{
    return roi.isValid();
}

cv::Rect Cascade::faceROI(const CvRect& faceRect) const
{
    return (cv::Rect(lround(faceRect.x + roi.x()      * faceRect.width),
                     lround(faceRect.y + roi.y()      * faceRect.height),
                     lround(             roi.width()  * faceRect.width),
                     lround(             roi.height() * faceRect.height)));
}

cv::Size Cascade::minSizeForFace(const cv::Size& faceSize) const
{
    cv::Size minSize;

    if (!isFacialFeature())
    {
        // Start with a size slightly smaller than the presumed face

        minSize = cv::Size(lround(double(faceSize.width)  * 0.6),
                           lround(double(faceSize.height) * 0.6));
    }
    else
    {
        // for a feature, which is smaller than the face, start with a significantly smaller min size

        minSize = cv::Size(lround(double(faceSize.width)  / faceToFeatureRelationMin()),
                           lround(double(faceSize.height) / faceToFeatureRelationMin()));
    }

    if (lessThanWindowSize(minSize))
    {
        return cv::Size(0, 0);
    }

    return minSize;
}

double Cascade::requestedInputScaleFactor(const cv::Size& faceSize) const
{
    if (!isFacialFeature())
    {
        return 1.0;
    }

    // getOriginalWindowSize is the size on which the cascade was trained, read from the XML file

    if ((faceSize.width  / faceToFeatureRelationMin() >= getOriginalWindowSize().width)  &&
        (faceSize.height / faceToFeatureRelationMin() >= getOriginalWindowSize().height))
    {
        return 1.0;
    }

    return (cv::max(double(getOriginalWindowSize().width)  * faceToFeatureRelationPresumed() / faceSize.width,
                    double(getOriginalWindowSize().height) * faceToFeatureRelationPresumed() / faceSize.height));
}

bool Cascade::lessThanWindowSize(const cv::Size& size) const
{
    return ((size.width  < getOriginalWindowSize().width)   ||
            (size.height < getOriginalWindowSize().height));
}

// ---------------------------------------------------------------------------------------------------

OpenCVFaceDetector::Private::Private()
{
    maxDistance              = 0;
    minDuplicates            = 0;
    speedVsAccuracy          = 0.8;
    sensitivityVsSpecificity = 0.8;
}

} // namespace Digikam
