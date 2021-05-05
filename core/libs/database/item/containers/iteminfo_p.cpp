/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-04-21
 * Description : Handling access to one item and associated data - private
 *
 * Copyright (C) 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2007-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013      by Michael G. Hansen <mike at mghansen dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "iteminfo_p.h"

namespace Digikam
{

MetadataInfo::Field DatabaseVideoMetadataFieldsToMetadataInfoField(const DatabaseFields::VideoMetadata videoMetadataField)
{
    switch (videoMetadataField)
    {
        case DatabaseFields::AspectRatio:
            return MetadataInfo::AspectRatio;

        case DatabaseFields::AudioBitRate:
            return MetadataInfo::AudioBitRate;

        case DatabaseFields::AudioChannelType:
            return MetadataInfo::AudioChannelType;

        case DatabaseFields::AudioCodec:
            return MetadataInfo::AudioCodec;

        case DatabaseFields::Duration:
            return MetadataInfo::Duration;

        case DatabaseFields::FrameRate:
            return MetadataInfo::FrameRate;

        case DatabaseFields::VideoCodec:
            return MetadataInfo::VideoCodec;

        default:
            break;
    }

    /// @todo Invalid request...
    return MetadataInfo::Field();
}

MetadataInfo::Field DatabaseImageMetadataFieldsToMetadataInfoField(const DatabaseFields::ImageMetadata imageMetadataField)
{
    switch (imageMetadataField)
    {
        case DatabaseFields::Make:
            return MetadataInfo::Make;

        case DatabaseFields::Model:
            return MetadataInfo::Model;

        case DatabaseFields::Lens:
            return MetadataInfo::Lens;

        case DatabaseFields::Aperture:
            return MetadataInfo::Aperture;

        case DatabaseFields::FocalLength:
            return MetadataInfo::FocalLength;

        case DatabaseFields::FocalLength35:
            return MetadataInfo::FocalLengthIn35mm;

        case DatabaseFields::ExposureTime:
            return MetadataInfo::ExposureTime;

        case DatabaseFields::ExposureProgram:
            return MetadataInfo::ExposureProgram;

        case DatabaseFields::ExposureMode:
            return MetadataInfo::ExposureMode;

        case DatabaseFields::Sensitivity:
            return MetadataInfo::Sensitivity;

        case DatabaseFields::FlashMode:
            return MetadataInfo::FlashMode;

        case DatabaseFields::WhiteBalance:
            return MetadataInfo::WhiteBalance;

        case DatabaseFields::WhiteBalanceColorTemperature:
            return MetadataInfo::WhiteBalanceColorTemperature;

        case DatabaseFields::MeteringMode:
            return MetadataInfo::MeteringMode;

        case DatabaseFields::SubjectDistance:
            return MetadataInfo::SubjectDistance;

        case DatabaseFields::SubjectDistanceCategory:
            return MetadataInfo::SubjectDistanceCategory;

        default:
            break;
    }

    /// @todo Invalid request...
    return MetadataInfo::Field();
}

} // namespace Digikam
