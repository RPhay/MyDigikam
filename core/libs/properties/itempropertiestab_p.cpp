/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-04-19
 * Description : A tab to display general item information
 *
 * SPDX-FileCopyrightText: 2006-2023 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2013      by Michael G. Hansen <mike at mghansen dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "itempropertiestab_p.h"

namespace Digikam
{

ItemPropertiesTab::Private::Private()
    : caption                   (nullptr),
      title                     (nullptr),
      pickLabel                 (nullptr),
      colorLabel                (nullptr),
      rating                    (nullptr),
      tags                      (nullptr),
      peoples                   (nullptr),
      versionned                (nullptr),
      grouped                   (nullptr),
      labelFile                 (nullptr),
      labelFolder               (nullptr),
      labelSymlink              (nullptr),
      labelFileModifiedDate     (nullptr),
      labelFileSize             (nullptr),
      labelFileOwner            (nullptr),
      labelFilePermissions      (nullptr),
      labelImageMime            (nullptr),
      labelImageDimensions      (nullptr),
      labelImageRatio           (nullptr),
      labelImageBitDepth        (nullptr),
      labelImageColorMode       (nullptr),
      labelHasSidecar           (nullptr),
      labelHasGPSInfo           (nullptr),
      labelVersionnedInfo       (nullptr),
      labelGroupedInfo          (nullptr),
      labelPhotoMake            (nullptr),
      labelPhotoModel           (nullptr),
      labelPhotoDateTime        (nullptr),
      labelPhotoLens            (nullptr),
      labelPhotoAperture        (nullptr),
      labelPhotoFocalLength     (nullptr),
      labelPhotoExposureTime    (nullptr),
      labelPhotoSensitivity     (nullptr),
      labelPhotoExposureMode    (nullptr),
      labelPhotoFlash           (nullptr),
      labelPhotoWhiteBalance    (nullptr),
      labelCaption              (nullptr),
      labelTitle                (nullptr),
      labelTags                 (nullptr),
      labelPeoples              (nullptr),
      labelLocation             (nullptr),
      labelRights               (nullptr),
      labelPickLabel            (nullptr),
      labelColorLabel           (nullptr),
      labelRating               (nullptr),
      labelVideoAspectRatio     (nullptr),
      labelVideoDuration        (nullptr),
      labelVideoFrameRate       (nullptr),
      labelVideoVideoCodec      (nullptr),
      labelVideoAudioBitRate    (nullptr),
      labelVideoAudioChannelType(nullptr),
      labelVideoAudioCodec      (nullptr)
{
}

ItemPropertiesTab::Private::~Private()
{
}

} // namespace Digikam
