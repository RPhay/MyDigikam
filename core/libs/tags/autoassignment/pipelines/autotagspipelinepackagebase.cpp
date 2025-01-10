/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-11-10
 * Description : Performs object detection and recognition
 *
 * SPDX-FileCopyrightText: 2024      by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024      by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "autotagspipelinepackagebase.h"

namespace Digikam
{

AutotagsPipelinePackageBase::AutotagsPipelinePackageBase(qlonglong _imageId) :
                                                     info(_imageId)
{
}

// AutotagsPipelinePackageBase::AutotagsPipelinePackageBase(qlonglong _imageId, const FaceTagsIface& _face) :
//                                                      info(_imageId),
//                                                      face(_face)
// {
// }

// AutotagsPipelinePackageBase::AutotagsPipelinePackageBase(const ItemInfo& _info, const FaceTagsIface& _face, int _tagId, const TagRegion& _region, const DImg& _image, EditPipelineAction _action, bool _retrain) :
//                                                      info(_info),
//                                                      face(_face),
//                                                      image(_image),
//                                                      tagId(_tagId),
//                                                      region(_region),
//                                                      action(_action),
//                                                      retrain(_retrain)
// {
// }

}