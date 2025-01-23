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

#include "facepipelinepackagebase.h"

namespace Digikam
{

FacePipelinePackageBase::FacePipelinePackageBase(qlonglong _imageId)
    : info(_imageId)
{
}

FacePipelinePackageBase::FacePipelinePackageBase(qlonglong _imageId, const FaceTagsIface& _face)
    : info(_imageId),
      face(_face)
{
}

FacePipelinePackageBase::FacePipelinePackageBase(const ItemInfo& _info,
                                                 const FaceTagsIface& _face,
                                                 int _tagId,
                                                 const TagRegion& _region,
                                                 const DImg& _image,
                                                 EditPipelineAction _action,
                                                 bool _retrain)
    : info   (_info),
      face   (_face),
      image  (_image),
      tagId  (_tagId),
      region (_region),
      action (_action),
      retrain(_retrain)
{
}

} // namespace Digikam
