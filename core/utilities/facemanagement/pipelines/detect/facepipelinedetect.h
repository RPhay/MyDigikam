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

// local includes

#include "facepipelinebase.h"
#include "facedetector.h"

namespace Digikam
{

class FacePipelineDetect : public FacePipelineBase
{
public:
    explicit FacePipelineDetect(const FaceScanSettings& _settings);
    ~FacePipelineDetect()                                       override;

    bool start()                                                override;
    void cancel()                                               override;

protected:
    bool finder()                                               override;
    bool loader()                                               override;
    bool extractor()                                            override;
    bool classifier()                                           override    { return false; }
    bool trainer()                                              override    { return false; }
    bool writer()                                               override;

    void addMoreWorkers()                                       override;


private:

    FaceDetector*           detector                            = nullptr;

    FacePipelineDetect()                                        = delete;
    FacePipelineDetect(FacePipelineDetect&)                     = delete;

};

}