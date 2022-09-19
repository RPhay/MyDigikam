/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        :
 * Description : Image Quality Parser - Abtrait class for detector
 *
 * SPDX-FileCopyrightText: 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2021-2022 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DETECTOR_H
#define DIGIKAM_DETECTOR_H

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

#endif // DIGIKAM_DETECTOR_H
