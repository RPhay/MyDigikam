/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-08-08
 * Description : Base class to perform low-level neural network inference
 *				 for face detection
 *
 * Copyright (C) 2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
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

#include "dnnfacedetectorbase.h"

// Qt includes

#include <QtGlobal>
#include <QRect>

// Digikam includes

#include "digikam_debug.h"

namespace Digikam
{

float DNNFaceDetectorBase::confidenceThreshold 	= 0.6;
float DNNFaceDetectorBase::nmsThreshold 		= 0.4;

// --------------------------------------------------------------------------------------------------------

DNNFaceDetectorBase::DNNFaceDetectorBase()
{
}

DNNFaceDetectorBase::DNNFaceDetectorBase(float scale, const cv::Scalar& val, const cv::Size& inputImgSize)
  : scaleFactor(scale),
    meanValToSubtract(val),
    inputImageSize(inputImgSize)
{
}

DNNFaceDetectorBase::~DNNFaceDetectorBase()
{
}

cv::Size DNNFaceDetectorBase::nnInputSizeRequired() const
{
	return inputImageSize;
}

void DNNFaceDetectorBase::selectBbox(const cv::Size& paddedSize, float confidence,
                                     int left, int right, int top, int bottom,
                                     std::vector<float>& goodConfidences, std::vector<cv::Rect>& goodBoxes,
                                     std::vector<float>& doubtConfidences, std::vector<cv::Rect>& doubtBoxes)
{

    int width = right - left;
    int height = bottom - top;

    cv::Rect bbox(left, top, width, height);

    int borderLeft = paddedSize.width;
    int borderRight = inputImageSize.width - paddedSize.width;
    int borderTop = paddedSize.height;
    int borderBottom = inputImageSize.height - paddedSize.height;

    /** Classify bounding boxes detected.
     * Good bounding boxes are defined as boxes that reside within the non-padded zone or 
     * those that are out only for min of (10% of padded range, 10% of bbox dim)
     * 
     * Bad bounding boxes are defined as boxes that have at maximum 25% of each dimension
     * out of non-padded zone
     */
    if(left >= (int)qMin(borderLeft*0.9, borderLeft - 0.1*width)
    && right <= (int)qMax(borderRight + 0.1*paddedSize.width, borderRight + 0.1*width)
    && top >= (int)qMin(borderTop*0.9, borderTop - 0.1*height)
    && bottom <= (int)qMax(borderBottom + 0.1*paddedSize.height, borderBottom + 0.1*height))
    {
        goodBoxes.push_back(bbox);
        goodConfidences.push_back(confidence);
        qDebug() << "Good rect = " << QRect(bbox.x, bbox.y, bbox.width, bbox.height) << ", conf = " << confidence;
    }
    else if(right > left && right >= borderLeft + width*0.75 && left <= borderRight - 0.75*width
         && bottom > top && bottom >= borderRight + height*0.75 && top <= borderBottom - 0.75*height)
    {
        doubtBoxes.push_back(bbox);
        doubtConfidences.push_back(confidence);
        qDebug() << "Doubt rect = " << QRect(bbox.x, bbox.y, bbox.width, bbox.height) << ", conf = " << confidence;
    }
}

void DNNFaceDetectorBase::correctBbox(cv::Rect& bbox, const cv::Size& paddedSize)
{
    //TODO: Should the box be cropped to square or not???

    int left = qMax(bbox.x - paddedSize.width, 0);
    int right = qMin(left + bbox.width, inputImageSize.width - 2*paddedSize.width);
    int top = qMax(bbox.y - paddedSize.height, 0);
    int bottom = qMin(top + bbox.height, inputImageSize.height - 2*paddedSize.height);

    bbox.x = left ;
    bbox.y = top;
    bbox.width = right - left;
    bbox.height = bottom - top;
}

}; // namespace Digikam