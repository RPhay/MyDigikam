/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-25
 * Description : a tool to convert RAW file to DNG - Metadata storage.
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010-2011 by Jens Mueller <tschenser at gmx dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "dngwriter_p.h"

// Local includes

#include "dngwriterhost.h"

namespace Digikam
{

int DNGWriter::Private::storeMetadata(DNGWriterHost& host,
                                      AutoPtr<dng_negative>& negative,
                                      DRawInfo* const identify,
                                      DRawInfo* const identifyMake)
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Updating metadata to DNG Negative" ;

    dng_exif* const exif = negative->GetExif();
    exif->fModel.Set_ASCII(identify->model.toLatin1().constData());
    exif->fMake.Set_ASCII(identify->make.toLatin1().constData());

    QString   str;
    QScopedPointer<DMetadata> meta(new DMetadata);

    if (meta->load(parent->inputFile()))
    {
        long int   num, den;
        long       val;

        fileDate                 = meta->getItemDateTime();

        // Time from original shot

        orgDateTimeInfo.SetDateTime(dngDateTime(meta->getItemDateTime()));
        exif->fDateTimeOriginal  = orgDateTimeInfo;

        dng_date_time_info digiDateTimeInfo;
        digiDateTimeInfo.SetDateTime(dngDateTime(meta->getDigitizationDateTime(true)));
        exif->fDateTimeDigitized = digiDateTimeInfo;

        negative->UpdateDateTime(orgDateTimeInfo);

        // String Tags

        str = meta->getExifTagString("Exif.Image.Make");
        if (!str.isEmpty()) exif->fMake.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.Image.Model");
        if (!str.isEmpty()) exif->fModel.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.Image.Software");
        if (!str.isEmpty()) exif->fSoftware.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.Image.ImageDescription");
        if (!str.isEmpty()) exif->fImageDescription.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.Image.Artist");
        if (!str.isEmpty()) exif->fArtist.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.Image.Copyright");
        if (!str.isEmpty()) exif->fCopyright.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.Photo.UserComment");
        if (!str.isEmpty()) exif->fUserComment.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.Image.CameraSerialNumber");
        if (!str.isEmpty()) exif->fCameraSerialNumber.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.GPSInfo.GPSLatitudeRef");
        if (!str.isEmpty()) exif->fGPSLatitudeRef.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.GPSInfo.GPSLongitudeRef");
        if (!str.isEmpty()) exif->fGPSLongitudeRef.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.GPSInfo.GPSSatellites");
        if (!str.isEmpty()) exif->fGPSSatellites.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.GPSInfo.GPSStatus");
        if (!str.isEmpty()) exif->fGPSStatus.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.GPSInfo.GPSMeasureMode");
        if (!str.isEmpty()) exif->fGPSMeasureMode.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.GPSInfo.GPSSpeedRef");
        if (!str.isEmpty()) exif->fGPSSpeedRef.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.GPSInfo.GPSTrackRef");
        if (!str.isEmpty()) exif->fGPSTrackRef.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.GPSInfo.GPSSpeedRef");
        if (!str.isEmpty()) exif->fGPSSpeedRef.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.GPSInfo.GPSImgDirectionRef");
        if (!str.isEmpty()) exif->fGPSSpeedRef.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.GPSInfo.GPSMapDatum");
        if (!str.isEmpty()) exif->fGPSMapDatum.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.GPSInfo.GPSDestLatitudeRef");
        if (!str.isEmpty()) exif->fGPSDestLatitudeRef.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.GPSInfo.GPSDestLongitudeRef");
        if (!str.isEmpty()) exif->fGPSDestLongitudeRef.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.GPSInfo.GPSDestBearingRef");
        if (!str.isEmpty()) exif->fGPSDestBearingRef.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.GPSInfo.GPSDestDistanceRef");
        if (!str.isEmpty()) exif->fGPSDestDistanceRef.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.GPSInfo.GPSProcessingMethod");
        if (!str.isEmpty()) exif->fGPSProcessingMethod.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.GPSInfo.GPSAreaInformation");
        if (!str.isEmpty()) exif->fGPSAreaInformation.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.GPSInfo.GPSDateStamp");
        if (!str.isEmpty()) exif->fGPSDateStamp.Set_ASCII(str.trimmed().toLatin1().constData());

        // Rational Tags

        if (meta->getExifTagRational("Exif.Photo.ExposureTime", num, den))          exif->fExposureTime             = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.Photo.FNumber", num, den))               exif->fFNumber                  = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.Photo.ShutterSpeedValue", num, den))     exif->fShutterSpeedValue        = dng_srational(num, den);
        if (meta->getExifTagRational("Exif.Photo.ApertureValue", num, den))         exif->fApertureValue            = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.Photo.BrightnessValue", num, den))       exif->fBrightnessValue          = dng_srational(num, den);
        if (meta->getExifTagRational("Exif.Photo.ExposureBiasValue", num, den))     exif->fExposureBiasValue        = dng_srational(num, den);
        if (meta->getExifTagRational("Exif.Photo.MaxApertureValue", num, den))      exif->fMaxApertureValue         = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.Photo.FocalLength", num, den))           exif->fFocalLength              = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.Photo.DigitalZoomRatio", num, den))      exif->fDigitalZoomRatio         = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.Photo.SubjectDistance", num, den))       exif->fSubjectDistance          = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.Image.BatteryLevel", num, den))          exif->fBatteryLevelR            = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.Photo.FocalPlaneXResolution", num, den)) exif->fFocalPlaneXResolution    = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.Photo.FocalPlaneYResolution", num, den)) exif->fFocalPlaneYResolution    = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.GPSInfo.GPSAltitude", num, den))         exif->fGPSAltitude              = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.GPSInfo.GPSDOP", num, den))              exif->fGPSDOP                   = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.GPSInfo.GPSSpeed", num, den))            exif->fGPSSpeed                 = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.GPSInfo.GPSTrack", num, den))            exif->fGPSTrack                 = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.GPSInfo.GPSImgDirection", num, den))     exif->fGPSImgDirection          = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.GPSInfo.GPSDestBearing", num, den))      exif->fGPSDestBearing           = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.GPSInfo.GPSDestDistance", num, den))     exif->fGPSDestDistance          = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.GPSInfo.GPSLatitude", num, den, 0))      exif->fGPSLatitude[0]           = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.GPSInfo.GPSLatitude", num, den, 1))      exif->fGPSLatitude[1]           = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.GPSInfo.GPSLatitude", num, den, 2))      exif->fGPSLatitude[2]           = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.GPSInfo.GPSLongitude", num, den, 0))     exif->fGPSLongitude[0]          = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.GPSInfo.GPSLongitude", num, den, 1))     exif->fGPSLongitude[1]          = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.GPSInfo.GPSLongitude", num, den, 2))     exif->fGPSLongitude[2]          = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.GPSInfo.GPSTimeStamp", num, den, 0))     exif->fGPSTimeStamp[0]          = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.GPSInfo.GPSTimeStamp", num, den, 1))     exif->fGPSTimeStamp[1]          = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.GPSInfo.GPSTimeStamp", num, den, 2))     exif->fGPSTimeStamp[2]          = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.GPSInfo.GPSDestLatitude", num, den, 0))  exif->fGPSDestLatitude[0]       = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.GPSInfo.GPSDestLatitude", num, den, 1))  exif->fGPSDestLatitude[1]       = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.GPSInfo.GPSDestLatitude", num, den, 2))  exif->fGPSDestLatitude[2]       = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.GPSInfo.GPSDestLongitude", num, den, 0)) exif->fGPSDestLongitude[0]      = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.GPSInfo.GPSDestLongitude", num, den, 1)) exif->fGPSDestLongitude[1]      = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.GPSInfo.GPSDestLongitude", num, den, 2)) exif->fGPSDestLongitude[2]      = dng_urational(num, den);

        // Integer Tags

        if (meta->getExifTagLong("Exif.Photo.ExposureProgram", val))                exif->fExposureProgram          = (uint32)val;
        if (meta->getExifTagLong("Exif.Photo.ISOSpeedRatings", val))                exif->fISOSpeedRatings[0]       = (uint32)val;
        if (meta->getExifTagLong("Exif.Photo.MeteringMode", val))                   exif->fMeteringMode             = (uint32)val;
        if (meta->getExifTagLong("Exif.Photo.LightSource", val))                    exif->fLightSource              = (uint32)val;
        if (meta->getExifTagLong("Exif.Photo.Flash", val))                          exif->fFlash                    = (uint32)val;
        if (meta->getExifTagLong("Exif.Photo.SensingMethod", val))                  exif->fSensingMethod            = (uint32)val;
        if (meta->getExifTagLong("Exif.Photo.FileSource", val))                     exif->fFileSource               = (uint32)val;
        if (meta->getExifTagLong("Exif.Photo.SceneType", val))                      exif->fSceneType                = (uint32)val;
        if (meta->getExifTagLong("Exif.Photo.CustomRendered", val))                 exif->fCustomRendered           = (uint32)val;
        if (meta->getExifTagLong("Exif.Photo.ExposureMode", val))                   exif->fExposureMode             = (uint32)val;
        if (meta->getExifTagLong("Exif.Photo.WhiteBalance", val))                   exif->fWhiteBalance             = (uint32)val;
        if (meta->getExifTagLong("Exif.Photo.SceneCaptureType", val))               exif->fSceneCaptureType         = (uint32)val;
        if (meta->getExifTagLong("Exif.Photo.GainControl", val))                    exif->fGainControl              = (uint32)val;
        if (meta->getExifTagLong("Exif.Photo.Contrast", val))                       exif->fContrast                 = (uint32)val;
        if (meta->getExifTagLong("Exif.Photo.Saturation", val))                     exif->fSaturation               = (uint32)val;
        if (meta->getExifTagLong("Exif.Photo.Sharpness", val))                      exif->fSharpness                = (uint32)val;
        if (meta->getExifTagLong("Exif.Photo.SubjectDistanceRange", val))           exif->fSubjectDistanceRange     = (uint32)val;
        if (meta->getExifTagLong("Exif.Photo.FocalLengthIn35mmFilm", val))          exif->fFocalLengthIn35mmFilm    = (uint32)val;
        if (meta->getExifTagLong("Exif.Photo.ComponentsConfiguration", val))        exif->fComponentsConfiguration  = (uint32)val;
        if (meta->getExifTagLong("Exif.Photo.PixelXDimension", val))                exif->fPixelXDimension          = (uint32)val;
        if (meta->getExifTagLong("Exif.Photo.PixelYDimension", val))                exif->fPixelYDimension          = (uint32)val;
        if (meta->getExifTagLong("Exif.Photo.FocalPlaneResolutionUnit", val))       exif->fFocalPlaneResolutionUnit = (uint32)val;
        if (meta->getExifTagLong("Exif.GPSInfo.GPSAltitudeRef", val))               exif->fGPSAltitudeRef           = (uint32)val;
        if (meta->getExifTagLong("Exif.GPSInfo.GPSDifferential", val))              exif->fGPSDifferential          = (uint32)val;

        long gpsVer1        = 0;
        long gpsVer2        = 0;
        long gpsVer3        = 0;
        long gpsVer4        = 0;
        meta->getExifTagLong("Exif.GPSInfo.GPSVersionID", gpsVer1, 0);
        meta->getExifTagLong("Exif.GPSInfo.GPSVersionID", gpsVer2, 1);
        meta->getExifTagLong("Exif.GPSInfo.GPSVersionID", gpsVer3, 2);
        meta->getExifTagLong("Exif.GPSInfo.GPSVersionID", gpsVer4, 3);

        long gpsVer         = 0;
        gpsVer             += gpsVer1 << 24;
        gpsVer             += gpsVer2 << 16;
        gpsVer             += gpsVer3 <<  8;
        gpsVer             += gpsVer4;
        exif->fGPSVersionID = (uint32)gpsVer;

        // Nikon Markernotes

        if (meta->getExifTagRational("Exif.Nikon3.Lens", num, den, 0))              exif->fLensInfo[0]              = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.Nikon3.Lens", num, den, 1))              exif->fLensInfo[1]              = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.Nikon3.Lens", num, den, 2))              exif->fLensInfo[2]              = dng_urational(num, den);
        if (meta->getExifTagRational("Exif.Nikon3.Lens", num, den, 3))              exif->fLensInfo[3]              = dng_urational(num, den);
        if (meta->getExifTagLong("Exif.Nikon3.ISOSpeed", val, 1))                   exif->fISOSpeedRatings[1]       = (uint32)val;

        str = meta->getExifTagString("Exif.Nikon3.SerialNO");
        if (!str.isEmpty()) str = str.replace(QLatin1String("NO="), QLatin1String(""));
        if (!str.isEmpty()) exif->fCameraSerialNumber.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.Nikon3.SerialNumber");
        if (!str.isEmpty()) exif->fCameraSerialNumber.Set_ASCII(str.toLatin1().constData());

        if (meta->getExifTagLong("Exif.Nikon3.ShutterCount", val))                  exif->fImageNumber              = (uint32)val;
        if (meta->getExifTagLong("Exif.NikonLd1.LensIDNumber", val))                exif->fLensID.Set_ASCII((QString::fromUtf8("%1").arg(val)).toLatin1().constData());
        if (meta->getExifTagLong("Exif.NikonLd2.LensIDNumber", val))                exif->fLensID.Set_ASCII((QString::fromUtf8("%1").arg(val)).toLatin1().constData());
        if (meta->getExifTagLong("Exif.NikonLd3.LensIDNumber", val))                exif->fLensID.Set_ASCII((QString::fromUtf8("%1").arg(val)).toLatin1().constData());
        if (meta->getExifTagLong("Exif.NikonLd2.FocusDistance", val))               exif->fSubjectDistance          = dng_urational((uint32)pow(10.0, val/40.0), 100);
        if (meta->getExifTagLong("Exif.NikonLd3.FocusDistance", val))               exif->fSubjectDistance          = dng_urational((uint32)pow(10.0, val/40.0), 100);
        str = meta->getExifTagString("Exif.NikonLd1.LensIDNumber");
        if (!str.isEmpty()) exif->fLensName.Set_ASCII(str.trimmed().toLatin1().constData());
        str = meta->getExifTagString("Exif.NikonLd2.LensIDNumber");
        if (!str.isEmpty()) exif->fLensName.Set_ASCII(str.trimmed().toLatin1().constData());
        str = meta->getExifTagString("Exif.NikonLd3.LensIDNumber");
        if (!str.isEmpty()) exif->fLensName.Set_ASCII(str.trimmed().toLatin1().constData());

        // Canon Markernotes

        if (meta->getExifTagLong("Exif.Canon.SerialNumber", val))                   exif->fCameraSerialNumber.Set_ASCII((QString::fromUtf8("%1").arg(val)).toLatin1().constData());
/*
        if (meta->getExifTagLong("Exif.CanonCs.LensType", val))                     exif->fLensID.Set_ASCII((QString::fromUtf8("%1").arg(val)).toLatin1().constData());
        if (meta->getExifTagLong("Exif.CanonCs.FlashActivity", val))                exif->fFlash                    = (uint32)val;
        if (meta->getExifTagLong("Exif.CanonFi.FileNumber", val))                   exif->fImageNumber              = (uint32)val;
        if (meta->getExifTagLong("Exif.CanonCs.MaxAperture", val))                  exif->fMaxApertureValue         = dng_urational(val, 100000);
*/
        if (meta->getExifTagLong("Exif.CanonCs.ExposureProgram", val))
        {
            switch (val)
            {
                case 1:
                {
                    exif->fExposureProgram = 2;
                    break;
                }

                case 2:
                {
                    exif->fExposureProgram = 4;
                    break;
                }

                case 3:
                {
                    exif->fExposureProgram = 3;
                    break;
                }

                case 4:
                {
                    exif->fExposureProgram = 1;
                    break;
                }

                default:
                {
                    break;
                }
            }
        }

        if (meta->getExifTagLong("Exif.CanonCs.MeteringMode", val))
        {
            switch (val)
            {
                case 1:
                {
                    exif->fMeteringMode = 3;
                    break;
                }

                case 2:
                {
                    exif->fMeteringMode = 1;
                    break;
                }

                case 3:
                {
                    exif->fMeteringMode = 5;
                    break;
                }

                case 4:
                {
                    exif->fMeteringMode = 6;
                    break;
                }

                case 5:
                {
                    exif->fMeteringMode = 2;
                    break;
                }

                default:
                {
                    break;
                }
            }
        }

        long canonLensUnits = 1;
        if (meta->getExifTagRational("Exif.CanonCs.Lens", num, den, 2))             canonLensUnits                  = num;
        if (meta->getExifTagRational("Exif.CanonCs.Lens", num, den, 0))             exif->fLensInfo[1]              = dng_urational(num, canonLensUnits);
        if (meta->getExifTagRational("Exif.CanonCs.Lens", num, den, 1))             exif->fLensInfo[0]              = dng_urational(num, canonLensUnits);
        if (meta->getExifTagRational("Exif.Canon.FocalLength", num, den, 1))        exif->fFocalLength              = dng_urational(num, canonLensUnits);
        long canonLensType = 65535;

        if (meta->getExifTagLong("Exif.CanonCs.LensType", canonLensType))           exif->fLensID.Set_ASCII((QString::fromUtf8("%1").arg(canonLensType)).toLatin1().constData());
        str = meta->getExifTagString("Exif.Canon.LensModel");

        if      (!str.isEmpty())
        {
            exif->fLensName.Set_ASCII(str.trimmed().toLatin1().constData());
        }
        else if (canonLensType != 65535)
        {
            str = meta->getExifTagString("Exif.CanonCs.LensType");
            if (!str.isEmpty()) exif->fLensName.Set_ASCII(str.trimmed().toLatin1().constData());
        }

        str = meta->getExifTagString("Exif.Canon.OwnerName");
        if (!str.isEmpty()) exif->fOwnerName.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.Canon.FirmwareVersion");
        if (!str.isEmpty()) str = str.replace(QLatin1String("Firmware"), QLatin1String(""));
        if (!str.isEmpty()) str = str.replace(QLatin1String("Version"), QLatin1String(""));
        if (!str.isEmpty()) exif->fFirmware.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.CanonSi.ISOSpeed");
        if (!str.isEmpty()) exif->fISOSpeedRatings[1] = str.toInt();

        // Pentax Markernotes

        str = meta->getExifTagString("Exif.Pentax.LensType");

        if (!str.isEmpty())
        {
            exif->fLensName.Set_ASCII(str.trimmed().toLatin1().constData());
            exif->fLensName.TrimLeadingBlanks();
            exif->fLensName.TrimTrailingBlanks();
        }

        long pentaxLensId1 = 0;
        long pentaxLensId2 = 0;

        if ((meta->getExifTagLong("Exif.Pentax.LensType", pentaxLensId1, 0)) &&
            (meta->getExifTagLong("Exif.Pentax.LensType", pentaxLensId2, 1)))
        {
            exif->fLensID.Set_ASCII(QString::fromUtf8("%1").arg(pentaxLensId1, pentaxLensId2).toLatin1().constData());
        }

        // Olympus Makernotes

        str = meta->getExifTagString("Exif.OlympusEq.SerialNumber");
        if (!str.isEmpty()) exif->fCameraSerialNumber.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.OlympusEq.LensSerialNumber");
        if (!str.isEmpty()) exif->fLensSerialNumber.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.OlympusEq.LensModel");
        if (!str.isEmpty()) exif->fLensName.Set_ASCII(str.trimmed().toLatin1().constData());

        if (meta->getExifTagLong("Exif.OlympusEq.MinFocalLength", val))             exif->fLensInfo[0]              = dng_urational(val, 1);
        if (meta->getExifTagLong("Exif.OlympusEq.MaxFocalLength", val))             exif->fLensInfo[1]              = dng_urational(val, 1);

        // Panasonic Makernotes

        str = meta->getExifTagString("Exif.Panasonic.LensType");
        if (!str.isEmpty()) exif->fLensName.Set_ASCII(str.trimmed().toLatin1().constData());

        str = meta->getExifTagString("Exif.Panasonic.LensSerialNumber");
        if (!str.isEmpty()) exif->fLensSerialNumber.Set_ASCII(str.trimmed().toLatin1().constData());

        // Sony Makernotes

        if (meta->getExifTagLong("Exif.Sony2.LensID", val))
        {
            exif->fLensID.Set_ASCII(QString::fromUtf8("%1").arg(val).toLatin1().constData());
        }

        str = meta->getExifTagString("Exif.Photo.LensModel");

        if (str.isEmpty())
        {
            str = meta->getExifTagString("Exif.Sony2.LensID");
        }

        if (!str.isEmpty()) exif->fLensName.Set_ASCII(str.trimmed().toLatin1().constData());

        // -------------------------------------------

        if ((exif->fLensName.IsEmpty())          &&
            (exif->fLensInfo[0].As_real64() > 0) &&
            (exif->fLensInfo[1].As_real64() > 0))
        {
            QString     lensName;
            QTextStream stream(&lensName);
            double      dval = (double)exif->fLensInfo[0].n / (double)exif->fLensInfo[0].d;
            stream << QString::fromUtf8("%1").arg(dval, 0, 'f', 1);

            if (exif->fLensInfo[0].As_real64() != exif->fLensInfo[1].As_real64())
            {
                dval = (double)exif->fLensInfo[1].n / (double)exif->fLensInfo[1].d;
                stream << QString::fromUtf8("-%1").arg(dval, 0, 'f', 1);
            }

            stream << " mm";

            if (exif->fLensInfo[2].As_real64() > 0)
            {
                dval = (double)exif->fLensInfo[2].n / (double)exif->fLensInfo[2].d;
                stream << QString::fromUtf8(" 1/%1").arg(dval, 0, 'f', 1);
            }

            if ((exif->fLensInfo[3].As_real64() > 0) &&
                (exif->fLensInfo[2].As_real64() != exif->fLensInfo[3].As_real64()))
            {
                dval = (double)exif->fLensInfo[3].n / (double)exif->fLensInfo[3].d;
                stream << QString::fromUtf8("-%1").arg(dval, 0, 'f', 1);
            }

            exif->fLensName.Set_ASCII(lensName.toLatin1().constData());
        }

        // Markernote backup.

        QByteArray mkrnts = meta->getExifTagData("Exif.Photo.MakerNote");

        if (!mkrnts.isEmpty())
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Backup Makernote (" << mkrnts.size() << " bytes)" ;

            dng_memory_allocator memalloc1(gDefaultDNGMemoryAllocator);
            dng_memory_stream stream(memalloc1);
            stream.Put(mkrnts.data(), mkrnts.size());
            AutoPtr<dng_memory_block> block(host.Allocate(mkrnts.size()));
            stream.SetReadPosition(0);
            stream.Get(block->Buffer(), mkrnts.size());

            if ((identifyMake->make != QLatin1String("Canon")) &&
                (identifyMake->make != QLatin1String("Panasonic")))
            {
                negative->SetMakerNote(block);
                negative->SetMakerNoteSafety(true);
            }
/*
            long mknOffset       = 0;
            QString mknByteOrder = meta->getExifTagString("Exif.MakerNote.ByteOrder");

            if ((meta->getExifTagLong("Exif.MakerNote.Offset", mknOffset)) && !mknByteOrder.isEmpty())
            {
                dng_memory_stream streamPriv(memalloc);
                streamPriv.SetBigEndian();

                streamPriv.Put("Adobe", 5);
                streamPriv.Put_uint8(0x00);
                streamPriv.Put("MakN", 4);
                streamPriv.Put_uint32(mkrnts.size() + mknByteOrder.size() + 4);
                streamPriv.Put(mknByteOrder.toLatin1().constData(), mknByteOrder.size());
                streamPriv.Put_uint32(mknOffset);
                streamPriv.Put(mkrnts.data(), mkrnts.size());
                AutoPtr<dng_memory_block> blockPriv(host.Allocate(streamPriv.Length()));
                streamPriv.SetReadPosition(0);
                streamPriv.Get(blockPriv->Buffer(), streamPriv.Length());
                negative->SetPrivateData(blockPriv);
            }
*/
        }
    }

    if (cancel)
    {
        return PROCESS_CANCELED;
    }

    return PROCESS_CONTINUE;
}

} // namespace Digikam
