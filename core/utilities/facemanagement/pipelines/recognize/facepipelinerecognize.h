/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-11-10
 * Description : Performs face detection and recognition
 *
 * SPDX-FileCopyrightText: 2024      by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024      by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

// #include <QThread>

// local includes

#include "facepipelinebase.h"
#include "facedetector.h"
#include "faceclassifier.h"

namespace Digikam
{

class FacePipelineRecognize : public FacePipelineBase
{
public:
    explicit FacePipelineRecognize(const FaceScanSettings& _settings);
    ~FacePipelineRecognize()                                    override;

    bool start()                                                override;
    void cancel()                                               override;

protected:
    bool finder()                                               override;
    bool loader()                                               override;
    bool extractor()                                            override;
    bool classifier()                                           override;
    bool trainer()                                              override
    {
        return false;
    }
    bool writer()                                               override;

    void addMoreWorkers()                                       override;


private:

    FaceDetector    detector;
    // float           threshold   = 0.5;

    FacePipelineRecognize()                                     = delete;
    FacePipelineRecognize(FacePipelineRecognize&)               = delete;
};

}