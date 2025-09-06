/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2025-08-24
 * Description : image editor plugin to blur the background of an image
 *
 * SPDX-FileCopyrightText: 2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "backgroundblurfilter.h"

// Qt includes

#include <QtMath>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals_p.h"      // For KF6::Ki18n deprecated
#include "digikam_opencv.h"
#include "qtopencvimg.h"

namespace DigikamEditorBackgroundBlurToolPlugin
{

class Q_DECL_HIDDEN BackgroundBlurFilter::Private
{
public:

    Private() = default;

public:

    int                   radius     = 3;   ///< Blur effect radius.
    int                   transition = 0;   ///< Number of blur transitions.
    int                   iterations = 10;  ///< GrabCut iterations to isolate more and less the subject.
    QRect                 selection;        ///< suject area previously selected by the user.
};

BackgroundBlurFilter::BackgroundBlurFilter(QObject* const parent)
    : DImgThreadedFilter(parent),
      d                 (new Private)
{
    initFilter();
}

BackgroundBlurFilter::BackgroundBlurFilter(DImg* const orgImage,
                                           const QRect& selection,
                                           int radius,
                                           int transition,
                                           int iterations,
                                           QObject* const parent)
    : DImgThreadedFilter(orgImage, parent, QLatin1String("BackgroundBlur")),
      d                 (new Private)
{
    d->selection  = selection;
    d->radius     = radius;
    d->transition = transition;
    d->iterations = iterations;

    initFilter();
}

BackgroundBlurFilter::BackgroundBlurFilter(DImgThreadedFilter* const parentFilter,
                                           const DImg& orgImage,
                                           const DImg& destImage,
                                           const QRect& selection,
                                           int radius,
                                           int transition,
                                           int iterations,
                                           int progressBegin,
                                           int progressEnd)
    : DImgThreadedFilter(parentFilter,
                         orgImage,
                         destImage,
                         progressBegin,
                         progressEnd,
                         parentFilter->filterName() + QLatin1String(": BackgroundBlur")),
      d(new Private)
{
    d->selection  = selection;
    d->radius     = radius;
    d->transition = transition;
    d->iterations = iterations;

    this->filterImage();
}

BackgroundBlurFilter::~BackgroundBlurFilter()
{
    cancelFilter();

    delete d;
}

QString BackgroundBlurFilter::DisplayableName()
{
    return QString::fromUtf8(I18N_NOOP("Background Blur Filter"));
}

void BackgroundBlurFilter::filterImage()
{
    if (d->radius < 1)
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "Radius out of range...";
        m_destImage = m_orgImage;

        return;
    }

    if (d->selection.isEmpty())
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "Selection is empty...";
        m_destImage = m_orgImage;

        return;
    }

    cv::Mat input = QtOpenCVImg::image2Mat(m_orgImage);
    cv::Mat output;

    postProgress(10);

    try
    {
        // Convert image to CV_8UC3 (BGR) if necessary.

        cv::Mat inputBGR;

        if (input.type() != CV_8UC3)
        {
            if      (input.channels() == 1)
            {
                cv::cvtColor(input, inputBGR, cv::COLOR_GRAY2BGR);
            }
            else if (input.channels() == 4)
            {
                cv::cvtColor(input, inputBGR, cv::COLOR_BGRA2BGR);
            }
            else
            {
                output = input.clone();

                return;
            }
        }
        else
        {
            inputBGR = input;
        }

        postProgress(20);

        // Init the mask for GrabCut.

        cv::Rect roi(d->selection.x(), d->selection.y(), d->selection.width(), d->selection.height());
        cv::Mat mask(input.rows, input.cols, CV_8UC1, cv::GC_PR_BGD);
        mask(roi) = cv::GC_PR_FGD;

        postProgress(30);

        // Apply GrabCut with more iterations for better accuracy.

        cv::Mat bgModel, fgModel;
        cv::grabCut(inputBGR, mask, roi, bgModel, fgModel, d->iterations, cv::GC_INIT_WITH_RECT);

        // Refine the mask: GC_PR_FGD and GC_FGD are considered foreground.

        cv::compare(mask, cv::GC_PR_FGD, mask, cv::CMP_GE);

        postProgress(40);

        // Smooth the mask edges.

        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
        cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel); // Close small holes

        // Expand a little bit the mask to include more pixels near the subject.

        cv::Mat kernelDilate = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2, 2));
        cv::dilate(mask, mask, kernel);

        // Convert the mask to BGR to be suitable for the preview.

        cv::Mat maskDisplay;
        mask.convertTo(maskDisplay, CV_8U, 255);  // Convert to 0-255

        // Create a copy of original.

        cv::Mat overlay = inputBGR.clone();

        // Create an image from the mask using semi-transparent green.

        cv::Mat coloredMask;
        cv::cvtColor(maskDisplay, coloredMask, cv::COLOR_GRAY2BGR);
        coloredMask.setTo(cv::Scalar(0, 255, 0), maskDisplay);  // Colorized the mask in green.

        // Apply a transparency to the mask.

        cv::Mat alphaMask;
        maskDisplay.convertTo(alphaMask, CV_32F, 1.0 / 255.0);  // Convert to  0.0-1.0.

        // Merge the semi-transparent mask over original image.

        for (int y = 0 ; y < inputBGR.rows ; y++)
        {
            for (int x = 0 ; x < inputBGR.cols ; x++)
            {
                float alpha                = alphaMask.at<float>(y, x);
                cv::Vec3b& pixel           = overlay.at<cv::Vec3b>(y, x);
                const cv::Vec3b& maskPixel = coloredMask.at<cv::Vec3b>(y, x);

                // Mix pixels using alpha channel.

                pixel[0] = static_cast<uchar>(alpha * maskPixel[0] + (1.0f - alpha) * pixel[0]);
                pixel[1] = static_cast<uchar>(alpha * maskPixel[1] + (1.0f - alpha) * pixel[1]);
                pixel[2] = static_cast<uchar>(alpha * maskPixel[2] + (1.0f - alpha) * pixel[2]);
            }
        }

        // Send the signal to render the mask preview in the GUI.

        QImage maskQImage = QtOpenCVImg::mat2Image(overlay);
        QImage rgbmask    = maskQImage.convertToFormat(QImage::Format_ARGB32);

        Q_EMIT signalSegmentedMask(rgbmask);

        postProgress(50);

        // Blur the background.

        cv::Mat blurred;
        cv::GaussianBlur(inputBGR, blurred, cv::Size(0, 0), d->radius);

        postProgress(60);

        if (d->transition == 0)
        {
            // Uniform blur.

            inputBGR.copyTo(output, mask);
            blurred.copyTo(output, ~mask);
        }
        else
        {
            // Normalize transition parameter between 0.1 and 2.0 for better control.

            float transition = 0.1F + (d->transition / 100.0F) * 1.9F;

            // Progressive blur.

            cv::Mat distanceMap;
            cv::distanceTransform(~mask, distanceMap, cv::DIST_L2, cv::DIST_MASK_5);

            // Normalize the distance for the progressive effect (0 = near the subject, 1 = far the subject).

            cv::normalize(distanceMap, distanceMap, 0, 1, cv::NORM_MINMAX);

            // Apply a non-linear transformation to the distance map based on transition parameter.

            for (int y = 0 ; y < distanceMap.rows ; y++)
            {
                for (int x = 0 ; x < distanceMap.cols ; x++)
                {
                    float dist = distanceMap.at<float>(y, x);

                    // Use transition parameter to control the falloff.

                    distanceMap.at<float>(y, x) = std::pow(dist, 1.0F / transition);
                }
            }

            // Create the result with the progressive blur.

            output = inputBGR.clone();

            postProgress(70);

            for (int y = 0; y < input.rows; y++)
            {
                for (int x = 0; x < input.cols; x++)
                {
                    float alpha = distanceMap.at<float>(y, x);
                    output.at<cv::Vec3b>(y, x) = alpha * blurred.at<cv::Vec3b>(y, x) +
                                                 (1.0F - alpha) * inputBGR.at<cv::Vec3b>(y, x);
                }
            }
        }

        postProgress(80);

        // Convert back to the original format if necessary.

        if (input.type() != CV_8UC3)
        {
            if (input.channels() == 1)
            {
                cv::cvtColor(output, output, cv::COLOR_BGR2GRAY);
            }
            else if (input.channels() == 4)
            {
                cv::cvtColor(output, output, cv::COLOR_BGR2BGRA);
            }
        }

        postProgress(90);
    }
    catch (cv::Exception& e)
    {
        qCWarning(DIGIKAM_DIMG_LOG) << "BackgroundBlurFilter::filterImage: cv::Exception:" << e.what();

        Q_EMIT signalSegmentedMask(QImage());
    }
    catch (...)
    {
        qCWarning(DIGIKAM_DIMG_LOG) << "BackgroundBlurFilter::filterImage: Default exception from OpenCV";

        Q_EMIT signalSegmentedMask(QImage());
    }

    m_destImage = DImg(QtOpenCVImg::mat2Image(output));

    if (!m_orgImage.hasAlpha())
    {
        m_destImage.removeAlphaChannel();
    }
}

FilterAction BackgroundBlurFilter::filterAction()
{
    FilterAction action(FilterIdentifier(), CurrentVersion());
    action.setDisplayableName(DisplayableName());

    action.addParameter(QLatin1String("radius"),     d->radius);
    action.addParameter(QLatin1String("transition"), d->transition);

    return action;
}

void BackgroundBlurFilter::readParameters(const FilterAction& action)
{
    d->radius     = action.parameter(QLatin1String("radius")).toInt();
    d->transition = action.parameter(QLatin1String("transition")).toInt();
}

} // namespace DigikamEditorBackgroundBlurToolPlugin

#include "moc_backgroundblurfilter.cpp"
