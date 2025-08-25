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

#include <QtConcurrentRun>
#include <QtMath>
#include <QMutex>

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

    void applyBackgroundBlur(const cv::Mat& input, cv::Mat& output, const QRect& selection, int blurIntensity)
    {
        cv::Rect roi(selection.x(), selection.y(), selection.width(), selection.height());
        cv::Mat mask(input.rows, input.cols, CV_8UC1, cv::GC_PR_BGD);
        mask(roi) = cv::GC_PR_FGD;

        cv::Mat bgModel, fgModel;
        cv::grabCut(input, mask, roi, bgModel, fgModel, 5, cv::GC_INIT_WITH_RECT);
        cv::compare(mask, cv::GC_PR_FGD, mask, cv::CMP_EQ);

        cv::Mat blurred;
        cv::GaussianBlur(input, blurred, cv::Size(0, 0), blurIntensity);
        input.copyTo(output, mask);
        blurred.copyTo(output, ~mask);
    }

public:

    int    radius         = 3;
    QRect  selection;
    int    globalProgress = 0;

    QMutex lock;
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
                                           QObject* const parent)
    : DImgThreadedFilter(orgImage, parent, QLatin1String("BackgroundBlur")),
      d                 (new Private)
{
    d->selection = selection;
    d->radius    = radius;
    initFilter();
}

BackgroundBlurFilter::BackgroundBlurFilter(DImgThreadedFilter* const parentFilter,
                       const DImg& orgImage,
                       const DImg& destImage,
                       const QRect& selection,
                       int radius,
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
    d->selection = selection;
    d->radius    = radius;
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
    d->applyBackgroundBlur(input, output, d->selection, d->radius);

    // FIXME: 16 bits color depth is not yet supported from QImage to DImg.
    m_destImage   = DImg(QtOpenCVImg::mat2Image(output));
}

FilterAction BackgroundBlurFilter::filterAction()
{
    FilterAction action(FilterIdentifier(), CurrentVersion());
    action.setDisplayableName(DisplayableName());

    action.addParameter(QLatin1String("radius"), d->radius);

    return action;
}

void BackgroundBlurFilter::readParameters(const FilterAction& action)
{
    d->radius = action.parameter(QLatin1String("radius")).toInt();
}

} // namespace DigikamEditorBackgroundBlurToolPlugin

#include "moc_backgroundblurfilter.cpp"
