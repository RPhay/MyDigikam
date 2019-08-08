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

#ifndef DIGIKAM_FACESENGINE_DNN_FACE_DETECTOR_BASE_H
#define DIGIKAM_FACESENGINE_DNN_FACE_DETECTOR_BASE_H

// OpenCV library

#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>

// C++ includes

#include <vector>

namespace Digikam
{

class DNNFaceDetectorBase
{

public:

	explicit DNNFaceDetectorBase();
	explicit DNNFaceDetectorBase(float scale, const cv::Scalar& val, const cv::Size& inputImgSize);
	virtual ~DNNFaceDetectorBase();

	virtual void detectFaces(const cv::Mat& inputImage, const cv::Size& paddedSize,
					 		 std::vector<cv::Rect>& detectedBboxes) = 0;
	cv::Size nnInputSizeRequired() const;

protected:

    void selectBbox(const cv::Size& paddedSize, float confidence,
                    int left, int right, int top, int bottom,
                    std::vector<float>& goodConfidences, std::vector<cv::Rect>& goodBoxes,
                    std::vector<float>& doubtConfidences, std::vector<cv::Rect>& doubtBoxes);
    void correctBbox(cv::Rect& bbox, const cv::Size& paddedSize);

public:

    static float    confidenceThreshold; 	// Threshold for bbox detection. It can be init and changed in the GUI
	static float 	nmsThreshold;			// Threshold for nms suppression

protected:

    float           scaleFactor;
    cv::Scalar      meanValToSubtract;
    cv::Size        inputImageSize;

    cv::dnn::Net    net;

};

}; // namespace Digikam

#endif // DIGIKAM_FACESENGINE_DNN_FACE_DETECTOR_BASE_H