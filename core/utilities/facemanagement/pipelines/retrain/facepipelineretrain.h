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

// Local includes

#include "facepipelinebase.h"
#include "facedetector.h"
#include "faceclassifier.h"

namespace Digikam
{

class FacePipelineRetrain : public FacePipelineBase
{
public:

    explicit FacePipelineRetrain(const FaceScanSettings& _settings);
    ~FacePipelineRetrain()                                      override;

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

    FacePipelineRetrain()                                       = delete;
    FacePipelineRetrain(FacePipelineRetrain&)                   = delete;
};

} // namespace Digikam
