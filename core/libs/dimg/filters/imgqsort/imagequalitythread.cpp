/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Image Quality Parser - Abtrait class for detector
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

#include "imagequalitythread.h"

namespace Digikam
{

ImageQualityThread::ImageQualityThread(QObject* const parent,
                                       DetectorDistortion* const detector,
                                       const cv::Mat& image,
                                       ImageQualityCalculator* const calculator,
                                       float weight_quality)
    : QThread     (parent),
      m_detector  (detector),
      m_calculator(calculator),
      m_image     (image),
      m_weight    (weight_quality)
{
}

void ImageQualityThread::run()
{
    float damageLevel = m_detector->detect(m_image);
    m_calculator->addDetectionResult(QString(), damageLevel, m_weight);
}

//--------------------------------------------------------------------------

ImageQualityThreadPool::ImageQualityThreadPool(QObject* const parent,
                                               ImageQualityCalculator* const calculator)
    : QObject     (parent),
      m_calculator(calculator)
{
}

ImageQualityThreadPool::~ImageQualityThreadPool()
{
    end();
/*
    for (auto& thread : m_threads)
    {
        delete thread;
    }
*/
}


void ImageQualityThreadPool::addDetector(const cv::Mat& image,
                                         float weight_quality,
                                         DetectorDistortion* const detector)
{
    ImageQualityThread* const thread = new ImageQualityThread(this,
                                                              detector,
                                                              image,
                                                              m_calculator,
                                                              weight_quality);
/*
    connect(thread, &QThread::finished,
            thread, &QObject::deleteLater);
*/
    m_threads.push_back(thread);
}

void ImageQualityThreadPool::start()
{
    for (const auto& thread : m_threads)
    {
        thread->start();
    }
}

void ImageQualityThreadPool::end()
{
    for (auto& thread : m_threads)
    {
        thread->quit();
        thread->wait();
    }
}

} // namespace Digikam
