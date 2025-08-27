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

    int                   radius         = 3;
    int                   transition     = 0;
    QRect                 selection;
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
                                           QObject* const parent)
    : DImgThreadedFilter(orgImage, parent, QLatin1String("BackgroundBlur")),
      d                 (new Private)
{
    d->selection  = selection;
    d->radius     = radius;
    d->transition = transition;
    initFilter();
}

BackgroundBlurFilter::BackgroundBlurFilter(DImgThreadedFilter* const parentFilter,
                       const DImg& orgImage,
                       const DImg& destImage,
                       const QRect& selection,
                       int radius,
                       int transition,
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

    try
    {
        // Convert image to CV_8UC3 (BGR) if necessary.

        cv::Mat inputBGR;

        if (input.type() != CV_8UC3)
        {
            if      (input.channels() == 1)
            {
                cv::cvtColor(input, inputBGR, cv::COLOR_GRAY2BGR); // Gray scale -> BGR
            }
            else if (input.channels() == 4)
            {
                cv::cvtColor(input, inputBGR, cv::COLOR_BGRA2BGR); // RGBA -> BGR
            }
            else
            {
                // Not supported case.

                output = input.clone();
                return;
            }
        }
        else
        {
            inputBGR = input; // Already the good format.
        }

        postProgress(10);

        // Init the mask for Grabcut.

        cv::Rect roi(d->selection.x(), d->selection.y(), d->selection.width(), d->selection.height());
        cv::Mat mask(input.rows, input.cols, CV_8UC1, cv::GC_PR_BGD);
        mask(roi) = cv::GC_PR_FGD;

        postProgress(20);

        // Apply GrabCut to inputBGR.

        cv::Mat bgModel;
        cv::Mat fgModel;
        cv::grabCut(inputBGR, mask, roi, bgModel, fgModel, 5, cv::GC_INIT_WITH_RECT);
        cv::compare(mask, cv::GC_PR_FGD, mask, cv::CMP_EQ);

        postProgress(30);

        // Blur the background.

        cv::Mat blurred;
        cv::GaussianBlur(inputBGR, blurred, cv::Size(0, 0), d->radius);

        postProgress(40);

        float transition = (d->transition * 3.0F) / 100.0F;

        // Progressive blur.
        // First compute the map for the progressive blur.

        cv::Mat distanceMap;
        cv::distanceTransform(~mask, distanceMap, cv::DIST_L2, cv::DIST_MASK_5);

        // Normalize the distance for the progressive effect (0 = near the subject, 1 = far the subject).

        cv::normalize(distanceMap, distanceMap, transition, 0, cv::NORM_MINMAX);

        // Create the result with the progresive blur.

        output = inputBGR.clone();

        postProgress(60);

        for (int y = 0 ; y < input.rows ; y++)
        {
            for (int x = 0 ; x < input.cols ; x++)
            {
                float alpha = distanceMap.at<float>(y, x);

                // NOTE: if alpha is near of 1, the blur effect is intensive.

                output.at<cv::Vec3b>(y, x) = alpha       * blurred.at<cv::Vec3b>(y, x) +
                                             (1 - alpha) * inputBGR.at<cv::Vec3b>(y, x);
            }
        }

        postProgress(80);

        // Convert back to the original format if necessary.

        if (input.type() != CV_8UC3)
        {
            if      (input.channels() == 1)
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
        qCWarning(DIGIKAM_DIMG_LOG) << "BackgroundBlurFilter::applyBackgroundBlur: cv::Exception:" << e.what();
    }
    catch (...)
    {
        qCWarning(DIGIKAM_DIMG_LOG) << "BackgroundBlurFilter::applyBackgroundBlur: Default exception from OpenCV";
    }

    // FIXME: 16 bits color depth is not yet supported from QImage to DImg.
    m_destImage   = DImg(QtOpenCVImg::mat2Image(output));

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
