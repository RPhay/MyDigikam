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
#include <QElapsedTimer>

// Local includes

#include "facepipelinebase.h"
#include "facetagsiface.h"

namespace Digikam
{

class FacePipelineEdit : public FacePipelineBase
{
    Q_OBJECT

public:

    explicit FacePipelineEdit();
    ~FacePipelineEdit()                                         override = default;

    static FacePipelineEdit* instance();

public:

    bool start()                                                override;

    FaceTagsIface confirmFace(const ItemInfo& info,
                              const FaceTagsIface& face,
                              const TagRegion& region,
                              int tagId,
                              bool retrain = true);

    void removeFace(const ItemInfo& info,
                    const FaceTagsIface& face);

    void removeAllFaces(const ItemInfo& info);

    FaceTagsIface editTag(const ItemInfo& info,
                          const FaceTagsIface& face,
                          int newTagId);

    FaceTagsIface editRegion(const ItemInfo& info,
                             const FaceTagsIface& face,
                             const TagRegion& region,
                             const DImg& image,
                             bool retrain = true);

    FaceTagsIface addManually(const ItemInfo& info,
                              const DImg& image,
                              const TagRegion& region,
                              bool retrain = true);

protected:

    bool finder()                                               override
    {
        return false;
    }

    bool loader()                                               override;
    bool extractor()                                            override;

    bool classifier()                                           override
    {
        return false;
    }

    bool trainer()                                              override
    {
        return false;
    }

    bool writer()                                               override;

    void addMoreWorkers()                                       override;

private:

    bool isStarted = false;

    // TODO: Remove DEBUG timer
    QElapsedTimer   debugConfirmTimer;

private:

    // Disable
    FacePipelineEdit(QObject* const)                            = delete;
    FacePipelineEdit(const FacePipelineEdit&)                   = delete;
    FacePipelineEdit& operator=(const FacePipelineEdit&)        = delete;
};

} // namespace Digikam
