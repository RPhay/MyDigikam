/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Image Quality Parser - Blur detection
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
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

#include "blur_detector.h"

// Qt includes

#include <QtMath>

// Local includes

#include "digikam_debug.h"
#include "focuspoints_extractor.h"

namespace Digikam
{

class Q_DECL_HIDDEN BlurDetector::Private
{

public:

    explicit Private()
      : min_abs                 (1),
        ordre_log_filtrer       (30),
        sigma_smooth_image      (5),
        filtrer_defocus         (115),
        part_size_motion_blur   (40),
        edges_filtrer           (8),
        theta_resolution        (CV_PI/900),
        min_line_length         (30),
        threshold_hough         (15),
        min_nb_lines            (3),
        max_stddev              (0.65),
        part_size_mono_color    (40),
        mono_color_threshold    (10),
        have_focus_region       (false),
        ratio_expand_af_point   (2)
    {
    }

    float                               min_abs;
    float                               ordre_log_filtrer;
    float                               sigma_smooth_image;
    float                               filtrer_defocus;

    int                                 part_size_motion_blur;
    float                               edges_filtrer;
    double                              theta_resolution;
    double                              min_line_length;
    float                               threshold_hough;
    int                                 min_nb_lines;
    float                               max_stddev;

    int                                 part_size_mono_color;
    float                               mono_color_threshold;

    bool                                have_focus_region;
    int                                 ratio_expand_af_point;
    FocusPointsExtractor::ListAFPoints  af_points;
};

BlurDetector::BlurDetector(const DImg& image)
    : DetectorDistortion(),
      d                 (new Private)
{
    QScopedPointer<FocusPointsExtractor> const extractor (new FocusPointsExtractor(nullptr, image.originalFilePath()));

    d->af_points         = extractor->get_af_points(FocusPoint::TypePoint::Selected);
    d->have_focus_region = !d->af_points.isEmpty();
}

BlurDetector::~BlurDetector()
{
    delete d;
}

float BlurDetector::detect(const cv::Mat& image) const
{
    cv::Mat edgesMap      = edgeDetection(image);

    cv::Mat defocusMap    = detectDefocusMap(edgesMap);
    defocusMap.convertTo(defocusMap, CV_8U);

    cv::Mat motionBlurMap = detectMotionBlurMap(edgesMap);
    motionBlurMap.convertTo(motionBlurMap, CV_8U);

    cv::Mat weightsMat    = getWeightMap(image);

    cv::Mat blurMap       = defocusMap + motionBlurMap;

    cv::Mat res           = weightsMat.mul(blurMap);

    int totalPixels       = cv::countNonZero(weightsMat);

    int blurPixel         = cv::countNonZero(res);

    float percentBlur     = float(blurPixel) / float(totalPixels);

    qCDebug(DIGIKAM_DIMG_LOG) << "percentage of blur" << percentBlur;

    return percentBlur;
}

cv::Mat BlurDetector::edgeDetection(const cv::Mat& image) const
{
    // Convert the image to grayscale

    cv::Mat image_gray;
    cvtColor(image, image_gray, cv::COLOR_BGR2GRAY);

    // Use laplacian to detect edge map

    cv::Mat dst;
    cv::Laplacian( image_gray, dst, CV_32F);

    return dst;
}

cv::Mat BlurDetector::detectDefocusMap(const cv::Mat& edgesMap) const
{
    cv::Mat abs_map = cv::abs(edgesMap);

    abs_map.setTo(d->min_abs, (abs_map < d->min_abs));

    // Log filter

    cv::log(abs_map,abs_map);

    abs_map /= log(d->ordre_log_filtrer);

    // Smooth image to get blur map

    cv::blur(abs_map, abs_map, cv::Size(d->sigma_smooth_image, d->sigma_smooth_image));
    cv::medianBlur(abs_map, abs_map, d->sigma_smooth_image);

    // Mask blurred pixel as 1 and sharp pixel 0

    cv::Mat res = abs_map * 255;

    cv::threshold(res, res, d->filtrer_defocus, 1, cv::THRESH_BINARY_INV);

    return res;
}

cv::Mat BlurDetector::detectMotionBlurMap(const cv::Mat& edgesMap) const
{
    // Divide image

    qCDebug(DIGIKAM_DIMG_LOG) << "Divide image to small parts";

    int nb_parts_row = static_cast<int>(edgesMap.size().height / d->part_size_motion_blur);
    int nb_parts_col = static_cast<int>(edgesMap.size().width  / d->part_size_motion_blur);

    cv::Mat res      = cv::Mat::zeros(edgesMap.size(), CV_8U);

    for (int i = 0 ; i < nb_parts_row ; ++i)
    {
        for (int j = 0 ; j < nb_parts_col ; ++j)
        {
            cv::Rect rect{
                            j*d->part_size_motion_blur,
                            i*d->part_size_motion_blur,
                            d->part_size_motion_blur,
                            d->part_size_motion_blur
                         };

            cv::Mat subImg = edgesMap(rect);

            qCDebug(DIGIKAM_DIMG_LOG) << "Detect if each part is motion blur";

            if (isMotionBlur(subImg))
            {
                res(rect).setTo(1);
            }
        }
    }

    return res;
}

bool BlurDetector::isMotionBlur(const cv::Mat& frag) const
{
    // Convert to 8u

    cv::Mat tmp;

    cv::threshold(frag, tmp, d->edges_filtrer, 255, cv::THRESH_BINARY);
    tmp.convertTo(tmp, CV_8U);

    std::vector<cv::Vec4i> lines;
    HoughLinesP(tmp, lines, 1, d->theta_resolution, d->threshold_hough, d->min_line_length, 10);

    // Detect if region is motion blurred by number of paralle lines

    if (static_cast<int>(lines.size()) > d->min_nb_lines)
    {
        std::vector<float> list_theta;

        for (const auto line : lines)
        {
            float theta = (line[2] == line[0]) ? 0              : qAtan((line[3] - line[1]) / (line[2] - line[0]));
            theta       = (theta < 0)          ? theta + CV_PI  : theta;
            theta       = (theta < CV_PI/20)   ?  CV_PI - theta : theta;

            list_theta.push_back(theta);
        }

        // Calculate Standard Deviation

        cv::Scalar mean, stddev;
        cv::meanStdDev(list_theta, mean, stddev);

        qCDebug(DIGIKAM_DIMG_LOG) << "Standard Deviation for group of lines " << stddev[0];

        return (stddev[0] < d->max_stddev);
    }

    return false;
}

bool BlurDetector::haveFocusRegion(const DImg& image) const
{
    QScopedPointer<FocusPointsExtractor> const extractor(new FocusPointsExtractor(nullptr, image.originalFilePath()));

    d->af_points = extractor->get_af_points(FocusPoint::TypePoint::Selected);

    return (!d->af_points.isEmpty());
}

cv::Mat BlurDetector::getWeightMap(const cv::Mat& image) const
{
    cv::Mat res = cv::Mat::zeros(image.size(), CV_8UC1);

    if (d->have_focus_region)
    {
        for (const auto point : d->af_points)
        {
            QPointF pos           = point.getPosition();
            QSizeF size           = point.getSize();

            int x_position_corner = std::max(static_cast<int>((pos.x() - size.width() * 0.5  *d->ratio_expand_af_point) * image.size().width), 0);
            int y_position_corner = std::max(static_cast<int>((pos.y() - size.height() * 0.5 *d->ratio_expand_af_point) * image.size().height), 0);

            int width             = std::min(image.size().width - x_position_corner, static_cast<int>(size.width() * image.size().width * d->ratio_expand_af_point) );
            int height            = std::min(image.size().height - y_position_corner, static_cast<int>(size.height() * image.size().height * d->ratio_expand_af_point) );

            cv::Rect rect{
                            x_position_corner,
                            y_position_corner,
                            width,
                            height
                         };

            res(rect).setTo(1);
        }
    }
    else
    {
        res = detectBackgroundRegion(image);

        cv::threshold(res, res, 0.5, 1, cv::THRESH_BINARY_INV);
    }

    return res;
}

cv::Mat BlurDetector::detectBackgroundRegion(const cv::Mat& image)    const
{
    qCDebug(DIGIKAM_DIMG_LOG) << "Divide image to small parts";

    int nb_parts_row = static_cast<int>(image.size().height / d->part_size_mono_color);
    int nb_parts_col = static_cast<int>(image.size().width  / d->part_size_mono_color);

    cv::Mat res = cv::Mat::zeros(image.size(), CV_8U);

    for (int i = 0 ; i < nb_parts_row ; ++i)
    {
        for (int j = 0 ; j < nb_parts_col ; ++j)
        {
            cv::Rect rect{j * d->part_size_mono_color,
                          i * d->part_size_mono_color,
                          d->part_size_mono_color,
                          d->part_size_mono_color};

            cv::Mat subImg = image(rect);

            cv::Scalar mean, stddev;

            cv::meanStdDev(subImg, mean, stddev);

            if (stddev[0] < d->mono_color_threshold)
            {
                res(rect).setTo(1);
            }
        }
    }

    return res;
}

} // namespace Digikam
