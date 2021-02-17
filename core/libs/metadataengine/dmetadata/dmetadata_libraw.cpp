/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-02-23
 * Description : item metadata interface - libraw helpers.
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2011      by Leif Huhn <leif at dkstat dot com>
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

#include "dmetadata.h"

// Qt includes

#include <QString>
#include <QScopedPointer>

// Local includes

#include "digikam_config.h"
#include "digikam_debug.h"
#include "drawinfo.h"
#include "drawdecoder.h"
#include "iccprofile.h"

namespace Digikam
{

bool DMetadata::loadUsingRawEngine(const QString& filePath)
{
    QScopedPointer<DRawInfo> identify(new DRawInfo);

    if (DRawDecoder::rawFileIdentify(*identify, filePath))
    {
        long int num = 1;
        long int den = 1;

        if (!identify->model.isNull())
        {
            setExifTagString("Exif.Image.Model", identify->model);
        }

        if (!identify->make.isNull())
        {
            setExifTagString("Exif.Image.Make", identify->make);
        }

        if (!identify->owner.isNull())
        {
            setExifTagString("Exif.Image.Artist", identify->owner);
        }

        if (!identify->software.isNull())
        {
            setExifTagString("Exif.Image.ProcessingSoftware", identify->software);
        }

        if (!identify->firmware.isNull())
        {
            setExifTagString("Exif.Image.Software", identify->firmware);
        }

        if (!identify->DNGVersion.isNull())
        {
            QByteArray ba = identify->DNGVersion.toLatin1();
            ba.truncate(4);
            setExifTagData("Exif.Image.DNGVersion", ba);
        }

        if (!identify->uniqueCameraModel.isNull())
        {
            setExifTagString("Exif.Image.UniqueCameraModel", identify->uniqueCameraModel);
        }

        if (!identify->localizedCameraModel.isNull())
        {
            setExifTagData("Exif.Image.LocalizedCameraModel", identify->localizedCameraModel.toLatin1());
        }

        if (!identify->imageID.isNull())
        {
            setExifTagString("Exif.Image.ImageID", identify->imageID);
        }

        if (!identify->rawDataUniqueID.isNull())
        {
            setExifTagData("Exif.Image.RawDataUniqueID", identify->rawDataUniqueID.toLatin1());
        }

        if (!identify->originalRawFileName.isNull())
        {
            setExifTagData("Exif.Image.OriginalRawFileName", identify->originalRawFileName.toLatin1());
        }

        if (!identify->lensModel.isNull())
        {
            setExifTagString("Exif.Photo.LensModel", identify->lensModel);
        }

        if (!identify->lensMake.isNull())
        {
            setExifTagString("Exif.Photo.LensMake", identify->lensMake);
        }

        if (!identify->lensSerial.isNull())
        {
            setExifTagString("Exif.Photo.LensSerialNumber", identify->lensSerial);
        }

        if (identify->focalLengthIn35mmFilm != -1)
        {
            setExifTagLong("Exif.Photo.FocalLengthIn35mmFilm", identify->focalLengthIn35mmFilm);
        }

        if (identify->maxAperture != -1.0F)
        {
            convertToRational(identify->maxAperture, &num, &den, 8);
            setExifTagRational("Exif.Image.MaxApertureValue", num, den);
        }

        if (identify->serialNumber != 0)
        {
            setExifTagLong("Exif.Image.ImageNumber", identify->serialNumber);
        }

        if (identify->flashUsed != -1)
        {
            setExifTagLong("Exif.Photo.Flash", identify->flashUsed);
        }

        if (identify->meteringMode != -1)
        {
            setExifTagLong("Exif.Image.MeteringMode", identify->meteringMode);
        }

        if (identify->exposureProgram != -1)
        {
            setExifTagLong("Exif.Photo.ExposureProgram", identify->exposureProgram);
        }

        if (identify->sensitivity != -1)
        {
            setExifTagLong("Exif.Photo.ISOSpeedRatings", lroundf(identify->sensitivity));
        }

        if (identify->baselineExposure != -999.0F)
        {
            convertToRational(identify->baselineExposure, &num, &den, 8);
            setExifTagRational("Exif.Image.BaselineExposure", num, den);
        }

        if (identify->ambientTemperature != -1000.0F)
        {
            convertToRational(identify->ambientTemperature, &num, &den, 8);
            setExifTagRational("Exif.Photo.AmbientTemperature", num, den);
        }

        if (identify->ambientHumidity != -1000.0F)
        {
            convertToRational(identify->ambientHumidity, &num, &den, 8);
            setExifTagRational("Exif.Photo.0x9401", num, den);
        }

        if (identify->ambientPressure != -1000.0F)
        {
            convertToRational(identify->ambientPressure, &num, &den, 8);
            setExifTagRational("Exif.Photo.0x9402", num, den);
        }

        if (identify->ambientWaterDepth != 1000.0F)
        {
            convertToRational(identify->ambientWaterDepth, &num, &den, 8);
            setExifTagRational("Exif.Photo.0x9403", num, den);
        }

        if (identify->ambientAcceleration != -1000.0F)
        {
            convertToRational(identify->ambientAcceleration, &num, &den, 8);
            setExifTagRational("Exif.Photo.0x9404", num, den);
        }

        if (identify->ambientElevationAngle != -1000.0F)
        {
            convertToRational(identify->ambientElevationAngle, &num, &den, 8);
            setExifTagRational("Exif.Photo.0x9405", num, den);
        }

        if (identify->exposureIndex != -1.0F)
        {
            convertToRational(identify->exposureIndex, &num, &den, 8);
            setExifTagRational("Exif.Photo.ExposureIndex", num, den);
        }

        if (identify->dateTime.isValid())
        {
            setImageDateTime(identify->dateTime, false);
        }

        if (identify->exposureTime != -1.0F)
        {
            convertToRationalSmallDenominator(identify->exposureTime, &num, &den);
            setExifTagRational("Exif.Photo.ExposureTime", num, den);
        }

        if (identify->aperture != -1.0F)
        {
            convertToRational(identify->aperture, &num, &den, 8);
            setExifTagRational("Exif.Photo.ApertureValue", num, den);
        }

        if (identify->focalLength != -1.0F)
        {
            convertToRational(identify->focalLength, &num, &den, 8);
            setExifTagRational("Exif.Photo.FocalLength", num, den);
        }

        if (identify->orientation != DRawInfo::ORIENTATION_NONE)
        {
            ImageOrientation orientation;

            switch (identify->orientation)
            {
                case DRawInfo::ORIENTATION_180:

                    orientation = ORIENTATION_ROT_180;
                    break;

                case DRawInfo::ORIENTATION_Mirror90CCW:

                    orientation = ORIENTATION_ROT_90_HFLIP;
                    break;

                case DRawInfo::ORIENTATION_90CCW:

                    orientation = ORIENTATION_ROT_270;
                    break;

                case DRawInfo::ORIENTATION_90CW:

                    orientation = ORIENTATION_ROT_90;
                    break;

                default:

                    orientation = ORIENTATION_NORMAL;
                    break;
            }

            setExifTagLong("Exif.Image.Orientation", orientation);
        }

        if (identify->imageSize.isValid())
        {
            setItemDimensions(identify->imageSize);
        }

        // A RAW image is always uncalibrated. */

        setItemColorWorkSpace(WORKSPACE_UNCALIBRATED);

        // Handle ICC color profile byte-array

        if (!identify->iccData.isEmpty())
        {
            setIccProfile(IccProfile(identify->iccData));
        }

        // Handle GPS information

        if (identify->hasGpsInfo)
        {
            setGPSInfo(identify->altitude, identify->latitude, identify->longitude);
        }

        // Handle image description

        if (!identify->description.isEmpty())
        {
            setExifComment(identify->description);
        }

        // Handle thumbnail image

        if (!identify->thumbnail.isNull())
        {
            QImage thumb = QImage::fromData(identify->thumbnail);

            if (!thumb.isNull())
            {
                setExifThumbnail(thumb.scaled(160, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
        }

        // Handle XMP metadata byte-array

        if (!identify->xmpData.isEmpty())
        {
            setXmp(identify->xmpData);
        }

        return true;
    }

    return false;
}

} // namespace Digikam
