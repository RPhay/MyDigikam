/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 
 * Description : Image Quality Parser - Abtrait class for detector 
 *
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021-2022 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 * 
 * References  : 
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

#ifndef DIGIKAM_IMAGEQUALITY_THREAD_H
#define DIGIKAM_IMAGEQUALITY_THREAD_H

// Qt includes

#include <QThread> 

// Local includes

#include "dimg.h"
#include "digikam_opencv.h"
#include "detector.h"
#include "imagequalitycalculator.h"

namespace Digikam
{

class ImageQualityThread : public QThread
{
    Q_OBJECT
public:

    explicit ImageQualityThread(QObject* const parent ,DetectorDistortion* detector,
                                const cv::Mat& image, ImageQualityCalculator* calculator, 
                                float weight_quality);
    ~ImageQualityThread() = default;

public:

    void run() override;

private:

    DetectorDistortion*     m_detector;
    ImageQualityCalculator* m_calculator;
    cv::Mat                 m_image;
    float                   m_weight;
};



class ImageQualityThreadPool: public QObject
{
    Q_OBJECT
public:

    explicit ImageQualityThreadPool(QObject* const parent , ImageQualityCalculator* calculator);
    ~ImageQualityThreadPool();

public:

    void addDetector(const cv::Mat& image, float weight_quality, DetectorDistortion* detector);  

    void start();
    void end();

private:

    ImageQualityCalculator*     m_calculator;
    QList<ImageQualityThread*>  m_threads;
};

} // namespace Digikam

#endif // DIGIKAM_IMAGEQUALITY_THREAD_H
