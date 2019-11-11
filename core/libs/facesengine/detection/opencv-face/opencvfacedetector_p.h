/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-01-03
 * Description : Class to perform faces detection.
 *               Modesto Castrillón, Oscar Déniz, Daniel Hernández, Javier Lorenzo
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

#ifndef DIGIKAM_FACESENGINE_OPENCV_FACE_DETECTOR_P_H
#define DIGIKAM_FACESENGINE_OPENCV_FACE_DETECTOR_P_H

#include "opencvfacedetector.h"

// Qt includes

#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <qmath.h>

// Local includes

#include "digikam_debug.h"

using namespace std;

namespace Digikam
{

class Q_DECL_HIDDEN DetectObjectParameters
{
public:

    DetectObjectParameters();

public:

    double   searchIncrement;
    int      grouping;
    int      flags;
    cv::Size minSize;
};

// --------------------------------------------------------------

QString  s_opencvFaceDetectorFindFileInDirs(const QStringList& dirs, const QString& fileName);
int      s_opencvFaceDetectorDistanceOfCenters(const QRect& r1, const QRect& r2);
QRect    s_opencvFaceDetectorToQRect(const cv::Rect& rect);
cv::Rect s_opencvFaceDetectorFromQRect(const QRect& rect);

// --------------------------------------------------------------

class Q_DECL_HIDDEN Cascade : public cv::CascadeClassifier
{
public:

    Cascade(const QStringList& dirs, const QString& fileName);

    cv::Size getOriginalWindowSize() const;

    /**
     * Assumptions on the relation of the size of a facial feature to the whole face.
     * Basically, we say the size is between 1/10 and 1/4, approx 1/6
     */
    static double faceToFeatureRelationMin()      { return 10; }
    static double faceToFeatureRelationMax()      { return 4;  }
    static double faceToFeatureRelationPresumed() { return 6;  }

    /**
     * A primary cascade does the initial scan on the whole image area
     * A verifying cascade scans the area reported by the primary cascade
     */
    void setPrimaryCascade(bool isPrimary = true);

    void setROI(double x, double y, double width, double height);

    bool isFacialFeature() const;

    /**
     * given the full face rect (relative to whole image), returns the rectangle
     * of the region of interest of this cascade (still relative to whole image).
     * For frontal face cascades, returns the given parameter unchanged.
     */
    cv::Rect faceROI(const CvRect& faceRect) const;

    /**
     * Verifying cascades: Returns the minSize parameter for cvHaarDetectObjects.
     * For frontal faces, starts the scan in the same order of magnitude as the presumed face,
     * slightly smaller.
     * For facial features, which are smaller than a face, uses the faceToFeatureRelation
     * assumptions made above. Often may end using the minimum.
     */
    cv::Size minSizeForFace(const cv::Size& faceSize) const;

    /**
     * For facial features:
     * For the case that a feature ROI is small and shall be scaled up.
     * Give the real face size.
     * Returns a scale factor (>1) by which the face, or rather only the ROI,
     * should be scaled up to fit with the windowSize of this cascade.
     */
    double requestedInputScaleFactor(const cv::Size& faceSize) const;

    bool lessThanWindowSize(const cv::Size& size) const;

public:

    bool   primaryCascade;
    bool   verifyingCascade;

    /**
     * Facial features have a region of interest, e.g., the left eye is typically
     * located in the left upper region of the presumed face.
     * For frontal face cascades, this is 0,0 - 1x1.
     */
    QRectF roi;
};

// ---------------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN OpenCVFaceDetector::Private
{

public:

    explicit Private();

public:

    QList<Cascade>         cascades;

    int                    maxDistance;    // Maximum distance between two faces to call them unique
    int                    minDuplicates;  // Minimum number of duplicates required to qualify as a genuine face

    // Tunable values, for accuracy
    DetectObjectParameters primaryParams;
    DetectObjectParameters verifyingParams;

    double                 speedVsAccuracy;
    double                 sensitivityVsSpecificity;

    QMutex                 mutex;
};

} // namespace Digikam

#endif // DIGIKAM_FACESENGINE_OPENCV_FACE_DETECTOR_P_H
