/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-04-13
 * Description : ExifTool tags translator for Exiv2
 *
 * Copyright (C) 2020-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "exiftooltranslator_p.h"

namespace Digikam
{

void ExifToolTranslator::Private::populateExifHashTable()
{
    // --- EXIF Table -------------------------------------------------------------------

    // Exif Iop Group

//     mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Camera.Make"),                         QLatin1String("Exif.Iop."));
//     mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Camera.Model"),                        QLatin1String("Exif.Iop."));
//     mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Camera.Rating"),                       QLatin1String("Exif.Iop."));
    mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Camera.RelatedImageFileFormat"),       QLatin1String("Exif.Iop.RelatedImageFileFormat"));
    mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Camera.RelatedImageHeight"),           QLatin1String("Exif.Iop.RelatedImageLength"));
    mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Camera.RelatedImageWidth"),            QLatin1String("Exif.Iop.RelatedImageWidth"));
//     mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Image.BitsPerSample"),                 QLatin1String("Exif.Iop."));
//     mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Image.CellLength"),                    QLatin1String("Exif.Iop."));
//     mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Image.CellWidth"),                     QLatin1String("Exif.Iop."));
//     mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Image.Compression"),                   QLatin1String("Exif.Iop."));
//     mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Image.DocumentName"),                  QLatin1String("Exif.Iop."));
//     mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Image.FillOrder"),                     QLatin1String("Exif.Iop."));
//     mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Image.ImageDescription"),              QLatin1String("Exif.Iop."));
//     mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Image.ImageHeight"),                   QLatin1String("Exif.Iop."));
//     mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Image.ImageWidth"),                    QLatin1String("Exif.Iop."));
    mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Image.InteropIndex"),                  QLatin1String("Exif.Iop.InteroperabilityIndex"));
    mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Image.InteropVersion"),                QLatin1String("Exif.Iop.InteroperabilityVersion"));
//     mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Image.OldSubfileType"),                QLatin1String("Exif.Iop."));
//     mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Image.PhotometricInterpretation"),     QLatin1String("Exif.Iop."));
//     mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Image.ProcessingSoftware"),            QLatin1String("Exif.Iop."));
//     mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Image.SubfileType"),                   QLatin1String("Exif.Iop."));
//     mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Image.Thresholding"),                  QLatin1String("Exif.Iop."));

    // Exif Image Group

    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Author.Copyright"),                   QLatin1String("Exif.Image.Copyright"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Author.XPAuthor"),                    QLatin1String("Exif.Image.XPAuthor"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Camera.Make"),                        QLatin1String("Exif.Image.Make"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Camera.Model"),                       QLatin1String("Exif.Image.Model"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.DocumentName"),                 QLatin1String("Exif.Image.DocumentName"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.ImageWidth"),                   QLatin1String("Exif.Image.ImageWidth"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.ImageHeight"),                  QLatin1String("Exif.Image.ImageLength"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.BitsPerSample"),                QLatin1String("Exif.Image.BitsPerSample"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.Compression"),                  QLatin1String("Exif.Image.Compression"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.PhotometricInterpretation"),    QLatin1String("Exif.Image.PhotometricInterpretation"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.SamplesPerPixel"),              QLatin1String("Exif.Image.SamplesPerPixel"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.PlanarConfiguration"),          QLatin1String("Exif.Image.PlanarConfiguration"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.ProcessingSoftware"),           QLatin1String("Exif.Image.ProcessingSoftware"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.Rating"),                       QLatin1String("Exif.Image.Rating"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.RatingPercent"),                QLatin1String("Exif.Image.RatingPercent"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.ImageDescription"),             QLatin1String("Exif.Image.ImageDescription"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.Orientation"),                  QLatin1String("Exif.Image.Orientation"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.XResolution"),                  QLatin1String("Exif.Image.XResolution"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.YResolution"),                  QLatin1String("Exif.Image.YResolution"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.ResolutionUnit"),               QLatin1String("Exif.Image.ResolutionUnit"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.Software"),                     QLatin1String("Exif.Image.Software"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.YCbCrPositioning"),             QLatin1String("Exif.Image.YCbCrPositioning"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.DNGPrivateData"),               QLatin1String("Exif.Image.DNGPrivateData"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.Predictor"),                    QLatin1String("Exif.Image.Predictor"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.Padding"),                      QLatin1String("..."));        // TODO: do not exist yet in Exiv2
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Time.ModifyDate"),                    QLatin1String("..."));        // TODO: do not exist yet in Exiv2
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Time.PreviewDateTime"),               QLatin1String("..."));        // TODO: do not exist yet in Exiv2

    // Exif Photo Group

    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.OffsetSchema"),              QLatin1String("..."));        // TODO: do not exist yet in Exiv2
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.Padding"),                   QLatin1String("..."));        // TODO: do not exist yet in Exiv2

    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.BrightnessValue"),           QLatin1String("Exif.Photo.BrightnessValue"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.ImageUniqueID"),             QLatin1String("Exif.Photo.ImageUniqueID"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.UserComment"),               QLatin1String("Exif.Photo.UserComment"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.ColorSpace"),                QLatin1String("Exif.Photo.ColorSpace"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.ExifImageWidth"),            QLatin1String("Exif.Photo.PixelXDimension"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.ExifImageHeight"),           QLatin1String("Exif.Photo.PixelYDimension"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.ExposureTime"),              QLatin1String("Exif.Photo.ExposureTime"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.FNumber"),                   QLatin1String("Exif.Photo.FNumber"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.ISO"),                       QLatin1String("Exif.Photo.ISOSpeed"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.ExifVersion"),               QLatin1String("Exif.Photo.ExifVersion"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.ComponentsConfiguration"),   QLatin1String("Exif.Photo.ComponentsConfiguration"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.CompressedBitsPerPixel"),    QLatin1String("Exif.Photo.CompressedBitsPerPixel"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.ExposureCompensation"),      QLatin1String("Exif.Photo.ExposureBiasValue"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.FlashpixVersion"),           QLatin1String("Exif.Photo.FlashpixVersion"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.FileSource"),                QLatin1String("Exif.Photo.FileSource"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.SceneType"),                 QLatin1String("Exif.Photo.SceneType"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.CustomRendered"),            QLatin1String("Exif.Photo.CustomRendered"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.LensInfo"),                  QLatin1String("Exif.Photo.LensSpecification"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.LensModel"),                 QLatin1String("Exif.Photo.LensModel"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.RecommendedExposureIndex"),  QLatin1String("Exif.Photo.RecommendedExposureIndex"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.SensitivityType"),           QLatin1String("Exif.Photo.SensitivityType"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.ApertureValue"),             QLatin1String("Exif.Photo.ApertureValue"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.ShutterSpeedValue"),         QLatin1String("Exif.Photo.ShutterSpeedValue"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.ExposureProgram"),          QLatin1String("Exif.Photo.ExposureProgram"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.MaxApertureValue"),         QLatin1String("Exif.Photo.MaxApertureValue"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.MeteringMode"),             QLatin1String("Exif.Photo.MeteringMode"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.LightSource"),              QLatin1String("Exif.Photo.LightSource"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.Flash"),                    QLatin1String("Exif.Photo.Flash"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.FocalLength"),              QLatin1String("Exif.Photo.FocalLength"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.ExposureMode"),             QLatin1String("Exif.Photo.ExposureMode"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.DigitalZoomRatio"),         QLatin1String("Exif.Photo.DigitalZoomRatio"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.FocalLengthIn35mmFormat"),  QLatin1String("Exif.Photo.FocalLengthIn35mmFilm"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.SceneCaptureType"),         QLatin1String("Exif.Photo.SceneCaptureType"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.GainControl"),              QLatin1String("Exif.Photo.GainControl"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.Contrast"),                 QLatin1String("Exif.Photo.Contrast"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.Saturation"),               QLatin1String("Exif.Photo.Saturation"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.Sharpness"),                QLatin1String("Exif.Photo.Sharpness"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.SubjectDistanceRange"),     QLatin1String("Exif.Photo.SubjectDistanceRange"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.WhiteBalance"),             QLatin1String("Exif.Photo.WhiteBalance"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.FocalPlaneResolutionUnit"), QLatin1String("Exif.Photo.FocalPlaneResolutionUnit"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.FocalPlaneXResolution"),    QLatin1String("Exif.Photo.FocalPlaneXResolution"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.FocalPlaneYResolution"),    QLatin1String("Exif.Photo.FocalPlaneYResolution"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.SensingMethod"),            QLatin1String("Exif.Photo.SensingMethod"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Time.DateTimeOriginal"),           QLatin1String("Exif.Photo.DateTimeOriginal"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Time.CreateDate"),                 QLatin1String("Exif.Photo.DateTimeDigitized"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Time.SubSecTimeDigitized"),        QLatin1String("Exif.Photo.SubSecTimeDigitized"));
    mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Time.SubSecTimeOriginal"),         QLatin1String("Exif.Photo.SubSecTimeOriginal"));

    // Exif Thumbnail Group

    mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Camera.Make"),                        QLatin1String("Exif.Thumbnail.Make"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Camera.Model"),                       QLatin1String("Exif.Thumbnail.Model"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.ImageWidth"),                   QLatin1String("Exif.Thumbnail.ImageWidth"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.ImageHeight"),                  QLatin1String("Exif.Thumbnail.ImageLength"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.ImageDescription"),             QLatin1String("Exif.Thumbnail.ImageDescription"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.Software"),                     QLatin1String("Exif.Thumbnail.Software"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.Compression"),                  QLatin1String("Exif.Thumbnail.Compression"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.XResolution"),                  QLatin1String("Exif.Thumbnail.XResolution"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.YResolution"),                  QLatin1String("Exif.Thumbnail.YResolution"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.ResolutionUnit"),               QLatin1String("Exif.Thumbnail.ResolutionUnit"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.ThumbnailLength"),              QLatin1String("Exif.Thumbnail.JPEGInterchangeFormatLength"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.Orientation"),                  QLatin1String("Exif.Thumbnail.Orientation"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.YCbCrPositioning"),             QLatin1String("Exif.Thumbnail.YCbCrPositioning"));
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.ThumbnailOffset"),              QLatin1String("..."));        // TODO: do not exist yet in Exiv2
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Preview.ThumbnailImage"),             QLatin1String("..."));        // TODO: do not exist yet in Exiv2
    mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Time.ModifyDate"),                    QLatin1String("..."));        // TODO: do not exist yet in Exiv2

    // Exif GPSInfo Group
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Location.GPSAltitude"),                       QLatin1String("Exif.GPSInfo.GPSAltitude"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Location.GPSAltitudeRef"),                    QLatin1String("Exif.GPSInfo.GPSAltitudeRef"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Location.GPSLatitude"),                       QLatin1String("Exif.GPSInfo.GPSLatitude"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Location.GPSLatitudeRef"),                    QLatin1String("Exif.GPSInfo.GPSLatitudeRef"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Location.GPSLongitude"),                      QLatin1String("Exif.GPSInfo.GPSLongitude"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Location.GPSLongitudeRef"),                   QLatin1String("Exif.GPSInfo.GPSLongitudeRef"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Location.GPSVersionID"),                      QLatin1String("Exif.GPSInfo.GPSVersionID"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSAreaInformation"),                    QLatin1String("Exif.GPSInfo.GPSAreaInformation"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSDOP"),                                QLatin1String("Exif.GPSInfo.GPSDOP"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSDateStamp"),                          QLatin1String("Exif.GPSInfo.GPSDateStamp"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSDestBearing"),                        QLatin1String("Exif.GPSInfo.GPSDestBearing"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSDestBearingRef"),                     QLatin1String("Exif.GPSInfo.GPSDestBearingRef"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSDestDistance"),                       QLatin1String("Exif.GPSInfo.GPSDestDistance"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSDestDistanceRef"),                    QLatin1String("Exif.GPSInfo.GPSDestDistanceRef"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSDestLatitude"),                       QLatin1String("Exif.GPSInfo.GPSDestLatitude"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSDestLatitudeRef"),                    QLatin1String("Exif.GPSInfo.GPSDestLatitudeRef"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSDestLongitude"),                      QLatin1String("Exif.GPSInfo.GPSDestLongitude"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSDestLongitudeRef"),                   QLatin1String("Exif.GPSInfo.GPSDestLongitudeRef"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSDifferential"),                       QLatin1String("Exif.GPSInfo.GPSDifferential"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSHPositioningError"),                  QLatin1String("Exif.GPSInfo.GPSHPositioningError"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSImgDirection"),                       QLatin1String("Exif.GPSInfo.GPSImgDirection"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSImgDirectionRef"),                    QLatin1String("Exif.GPSInfo.GPSImgDirectionRef"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSMapDatum"),                           QLatin1String("Exif.GPSInfo.GPSMapDatum"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSMeasureMode"),                        QLatin1String("Exif.GPSInfo.GPSMeasureMode"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSProcessingMethod"),                   QLatin1String("Exif.GPSInfo.GPSProcessingMethod"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSSatellites"),                         QLatin1String("Exif.GPSInfo.GPSSatellites"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSSpeed"),                              QLatin1String("Exif.GPSInfo.GPSSpeed"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSSpeedRef"),                           QLatin1String("Exif.GPSInfo.GPSSpeedRef"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSStatus"),                             QLatin1String("Exif.GPSInfo.GPSStatus"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSTimeStamp"),                          QLatin1String("Exif.GPSInfo.GPSTimeStamp"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSTrack"),                              QLatin1String("Exif.GPSInfo.GPSTrack"));
    mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Time.GPSTrackRef"),                           QLatin1String("Exif.GPSInfo.GPSTrackRef"));

}

} // namespace Digikam
