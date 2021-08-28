/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 25/08/2013
 * Description : Image Quality Parser
 *
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013-2014 by Gowtham Ashok <gwty93 at gmail dot com>
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

#include "imagequalityparser_p.h"

// C++ includes

#include <thread>
#include <memory>

// Qt includes

#include <QScopedPointer>
#include <QThread>
#include <QThreadPool>
#include <QFuture>
#include <QtConcurrent>

// Local includes

#include "noise_detector.h"
#include "exposure_detector.h"
#include "compression_detector.h"
#include "blur_detector.h"
#include "imagequalitythread.h"

namespace Digikam
{

ImageQualityParser::ImageQualityParser(const DImg& image,
                                       const ImageQualityContainer& settings,
                                       PickLabel* const label)
    : d(new Private)
{
    d->imq     = settings;
    d->image   = image;
    d->label   = label;
}

ImageQualityParser::~ImageQualityParser()
{
    delete d->calculator;
    delete d;
}

void ImageQualityParser::startAnalyse()
{

    float finalQuality = -1.0;

    // const DetectorDistortion detector(d->image);

    cv::Mat cvImage    = DetectorDistortion::prepareForDetection(d->image);

    cv::Mat grayImage;

    cv::cvtColor(cvImage, grayImage, cv::COLOR_BGR2GRAY);
    //-----------------------------------------------------------------------------

    std::unique_ptr<BlurDetector> blurDetector;
    std::unique_ptr<NoiseDetector> noiseDetector;
    std::unique_ptr<CompressionDetector> compressionDetector;
    std::unique_ptr<ExposureDetector> exposureDetector;

    ImageQualityThreadPool pool(this, d->calculator);

    if (d->running && d->imq.detectBlur)
    {
        blurDetector = std::unique_ptr<BlurDetector>(new BlurDetector(d->image));

        pool.addDetector(cvImage, d->imq.blurWeight, blurDetector.get());
    }

    if (d->running && d->imq.detectNoise)
    {
        noiseDetector = std::unique_ptr<NoiseDetector>(new NoiseDetector());

        pool.addDetector(grayImage, d->imq.noiseWeight, noiseDetector.get());
    }

    if (d->running && d->imq.detectCompression)
    {
        compressionDetector = std::unique_ptr<CompressionDetector>(new CompressionDetector());

        pool.addDetector(cvImage, d->imq.compressionWeight, compressionDetector.get());
    }

    if (d->running && d->imq.detectExposure)
    {
        exposureDetector = std::unique_ptr<ExposureDetector>(new ExposureDetector());

        pool.addDetector(grayImage, d->imq.exposureWeight, exposureDetector.get());
    }

    pool.start();
    pool.end();

#ifdef TRACE

    // QFile filems("imgqsortresult.txt");

    // if (filems.open(QIODevice::Append | QIODevice::Text))
    // {
    //     QTextStream oms(&filems);
    //     oms << "File:" << d->image.originalFilePath() << endl;

    //     if (d->imq.detectBlur)
    //     {
    //         oms << "Blur Present:" << blur << endl;
    //         oms << "Blur Present(using LoG filter):"<< blur2 << endl;
    //     }

    //     if (d->imq.detectNoise)
    //     {
    //         oms << "Noise Present:" << noise << endl;
    //     }

    //     if (d->imq.detectCompression)
    //     {
    //         oms << "Compression Present:" << compressionLevel << endl;
    //     }

    //     if (d->imq.detectExposure)
    //     {
    //         oms << "Under-exposure Percents:" << underLevel << endl;
    //         oms << "Over-exposure Percents:"  << overLevel << endl;
    //     }

    //     filems.close();
    // }

#endif // TRACE

    // Calculating finalquality

    if (d->running)
    {
        finalQuality            =  d->calculator->calculateQuality();

        qCDebug(DIGIKAM_DIMG_LOG) << "Final Quality estimated: " << finalQuality;

        // Assigning PickLabels

        if      (finalQuality == -1.0)
        {
            *d->label = NoPickLabel;
        }
        else if ((int)finalQuality <= d->imq.rejectedThreshold)
        {
            *d->label = RejectedLabel;
        }
        else if (((int)finalQuality >  d->imq.rejectedThreshold) &&
                 ((int)finalQuality <= d->imq.acceptedThreshold))
        {
            *d->label = PendingLabel;
        }
        else
        {
            *d->label = AcceptedLabel;
        }
    }
    else
    {
        *d->label = NoPickLabel;
    }
}

void ImageQualityParser::cancelAnalyse()
{
    d->running = false;
}

} // namespace Digikam
