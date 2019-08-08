/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-08-08
 * Description : Derived class to perform YOLO neural network inference
 *				 for face detection. Credit: Ayoosh Kathuria (for Yolov3 blog post),
 *				 sthanhng (for example of face detection with Yolov3).				 
 *				 More information with Yolov3:
 *				 https://towardsdatascience.com/yolo-v3-object-detection-53fb7d3bfe6b
 *				 sthanhng github on face detection with Yolov3:
 *				 https://github.com/sthanhng/yoloface
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

#ifndef DIGIKAM_FACESENGINE_DNN_FACE_DETECTOR_YOLO_H
#define DIGIKAM_FACESENGINE_DNN_FACE_DETECTOR_YOLO_H

// Digikam includes

#include "dnnfacedetectorbase.h"

namespace Digikam
{

class DNNFaceDetectorYOLO: public DNNFaceDetectorBase
{

public:

	DNNFaceDetectorYOLO();
	~DNNFaceDetectorYOLO();

	virtual void detectFaces(const cv::Mat& inputImage, const cv::Size& paddedSize,
					 		 std::vector<cv::Rect>& detectedBboxes);

private:

	std::vector<cv::String> getOutputsNames();
	void postprocess(const std::vector<cv::Mat>& outs, const cv::Size& paddedSize, 
					 std::vector<cv::Rect>& detectedBboxes);

};

}; // namespace Digikam

#endif // DIGIKAM_FACESENGINE_DNN_FACE_DETECTOR_YOLO_H