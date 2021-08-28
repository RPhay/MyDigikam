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

#ifndef DIGIKAM_DETECTOR_DISTORTION_H
#define DIGIKAM_DETECTOR_DISTORTION_H

// Qt includes

#include <QThread>

// Local includes

#include "dimg.h"
#include "digikam_opencv.h"

namespace Digikam
{

class DetectorDistortion : public QObject
{
    Q_OBJECT

public:

    explicit DetectorDistortion(QObject* const parent = nullptr);
    virtual ~DetectorDistortion() = default;

    virtual float detect(const cv::Mat& image) const = 0;

public:

    static cv::Mat prepareForDetection(const DImg& inputImage);

};

} // namespace Digikam

#endif // DIGIKAM_DETECTOR_DISTORTION_H