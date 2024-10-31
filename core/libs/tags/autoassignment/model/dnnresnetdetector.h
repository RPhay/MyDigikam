/* ============================================================
 *
 * This file is a part of digiKam
 * https://www.digikam.org
 *
 * Date        : 2023-09-02
 * Description : Derived class to perform Resnet neural network inference
 *               for object detection (including yolo versions to benchmark).
 *
 * SPDX-FileCopyrightText: 2023 by Quoc Hung TRAN <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

#include "dnnbasedetectormodel.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT DNNResnetDetector: public DNNBaseDetectorModel
{

public:

    explicit DNNResnetDetector();
    ~DNNResnetDetector()                                                                    override;

protected:

    bool loadModels()                                                                       override;

private:

    QHash<QString, QVector<QRect> >        postprocess(const cv::Mat& inputImage,
                                                       const cv::Mat& out)                const override;

private:

    // Disable
    DNNResnetDetector(const DNNResnetDetector&)            = delete;
    DNNResnetDetector& operator=(const DNNResnetDetector&) = delete;

};

} // namespace Digikam
