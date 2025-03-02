/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-11-10
 * Description : Performs face detection and recognition
 *
 * SPDX-FileCopyrightText: 2024-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QImage>
#include <QRectF>

// Local includes

#include "digikam_opencv.h"
#include "mlpipelinepackagefoundation.h"
#include "faceutils.h"
#include "dimg.h"
#include "iteminfo.h"

namespace Digikam
{

class FacePipelinePackageBase : public MLPipelinePackageFoundation
{
public:

    enum EditPipelineAction
    {
        Confirm,
        Remove,
        EditTag,
        EditRegion,
        AddManually
    };

public:

    FacePipelinePackageBase()                                               = default;
    explicit FacePipelinePackageBase(qlonglong _imageId,
                                     int _serialNumber = -1);

    explicit FacePipelinePackageBase(qlonglong _imageId,
                                     const FaceTagsIface& _face,
                                     int _serialNumber = -1);

    explicit FacePipelinePackageBase(const ItemInfo& _info,
                                     const FaceTagsIface& _face,
                                     int _tagId,
                                     const TagRegion& _region,
                                     const DImg& _image,
                                     EditPipelineAction _action,
                                     bool _retrain,
                                     int _serialNumber = -1);


    virtual ~FacePipelinePackageBase() override;

public:

    ItemInfo                info;
    FaceTagsIface           face;
    QImage                  thumbnail;
    DImg                    image;
    QList<QRectF>           faceRects;
    cv::Mat                 features;
    int                     label           = -1;
    int                     tagId           = -1;
    TagRegion               region;
    EditPipelineAction      action          = EditPipelineAction::Confirm;
    bool                    useForTraining  = false;
    bool                    retrain         = false;
    QList<cv::Mat>          featuresList;
    QList<int>              labelList;
    QList<FaceTagsIface>    faceList;
    int                     serialNumber    = -1;

private:

    // Disable
    FacePipelinePackageBase(const FacePipelinePackageBase&)                 = delete;
    FacePipelinePackageBase& operator=(const FacePipelinePackageBase&)      = delete;
};

} // namespace Digikam
