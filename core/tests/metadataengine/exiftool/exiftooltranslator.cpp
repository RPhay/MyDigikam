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

#include "exiftooltranslator.h"

// Qt includes

#include <QHash>
#include <QStringList>

namespace Digikam
{

class Q_DECL_HIDDEN ExifToolTranslatorCreator
{
public:

    ExifToolTranslator object;
};

Q_GLOBAL_STATIC(ExifToolTranslatorCreator, creator)

ExifToolTranslator* ExifToolTranslator::instance()
{
    return &creator->object;
}

// -------------------------------------------------------------------

class Q_DECL_HIDDEN ExifToolTranslator::Private
{
public:

    explicit Private()
    {
        // --- EXIF Table -------------------------------------------------------------------

        // Exif Iop Group
        mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Image.InteropIndex"),           QLatin1String("Exif.Iop.InteroperabilityIndex"));
        mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Image.InteropVersion"),         QLatin1String("Exif.Iop.InteroperabilityVersion"));
        // Exif Image Group
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
        mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Camera.Make"),                        QLatin1String("Exif.Image.Make"));
        mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Camera.Model"),                       QLatin1String("Exif.Image.Model"));
        mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Time.ModifyDate"),                    QLatin1String("..."));        // TODO: do not exist yet in Exiv2
        mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Time.PreviewDateTime"),               QLatin1String("..."));        // TODO: do not exist yet in Exiv2
        // Exif Photo Group
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
        mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Time.DateTimeOriginal"),           QLatin1String("Exif.Photo.DateTimeOriginal"));
        mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Time.CreateDate"),                 QLatin1String("Exif.Photo.DateTimeDigitized"));
        // Exif Thumbnail Group
        mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.Compression"),                  QLatin1String("Exif.Thumbnail.Compression"));
        mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.XResolution"),                  QLatin1String("Exif.Thumbnail.XResolution"));
        mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.YResolution"),                  QLatin1String("Exif.Thumbnail.YResolution"));
        mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.ResolutionUnit"),               QLatin1String("Exif.Thumbnail.ResolutionUnit"));
        mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.ThumbnailLength"),              QLatin1String("Exif.Thumbnail.JPEGInterchangeFormatLength"));
        mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.ThumbnailOffset"),              QLatin1String("..."));        // TODO: do not exist yet in Exiv2
        mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Preview.ThumbnailImage"),             QLatin1String("..."));        // TODO: do not exist yet in Exiv2
        // Exif GPSInfo Group
        mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Location.GPSVersionID"),               QLatin1String("Exif.GPSInfo.GPSVersionID"));
        mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Location.GPSMapDatum"),                QLatin1String("Exif.GPSInfo.GPSMapDatum"));
        mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Location.GPSLatitudeRef"),             QLatin1String("Exif.GPSInfo.GPSLatitudeRef"));
        mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Location.GPSLatitude"),                QLatin1String("Exif.GPSInfo.GPSLatitude"));
        mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Location.GPSLongitudeRef"),            QLatin1String("Exif.GPSInfo.GPSLongitudeRef"));
        mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Location.GPSLongitude"),               QLatin1String("Exif.GPSInfo.GPSLongitude"));
        mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Location.GPSAltitudeRef"),             QLatin1String("Exif.GPSInfo.GPSAltitudeRef"));
        mapETtoExiv2.insert(QLatin1String("EXIF.GPS.Location.GPSAltitude"),                QLatin1String("Exif.GPSInfo.GPSAltitude"));

        // --- IPTC Table -------------------------------------------------------------------

        // Iptc Envelope Group
        mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.CodedCharacterSet"),            QLatin1String("Iptc.Envelope.CharacterSet"));
        mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.EnvelopeRecordVersion"),        QLatin1String("Iptc.Envelope.ModelVersion"));
        // Iptc Application2 Group
        mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.ApplicationRecordVersion"),     QLatin1String("Iptc.Application2.RecordVersion"));
        mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.Caption-Abstract"),             QLatin1String("Iptc.Application2.Caption"));
        mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.OriginatingProgram"),           QLatin1String("Iptc.Application2.Program"));
        mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.ProgramVersion"),               QLatin1String("Iptc.Application2.ProgramVersion"));
        mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.Keywords"),                     QLatin1String("Iptc.Application2.Keywords"));
        mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.DateCreated"),                   QLatin1String("Iptc.Application2.DateCreated"));
        mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.TimeCreated"),                   QLatin1String("Iptc.Application2.TimeCreated"));

        // --- XMP Table --------------------------------------------------------------------

        // Xmp x group
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-x.Document.XMPToolkit"),                QLatin1String("..."));        // TODO: do not exist yet in Exiv2
        // Xmp tiff Group
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Time.DateTime"),                   QLatin1String("Xmp.tiff.DateTime"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.Software"),                  QLatin1String("Xmp.tiff.Software"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.XResolution"),               QLatin1String("Xmp.tiff.XResolution"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.YResolution"),               QLatin1String("Xmp.tiff.YResolution"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.ResolutionUnit"),            QLatin1String("Xmp.tiff.ResolutionUnit"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.ImageWidth"),                QLatin1String("Xmp.tiff.ImageWidth"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.ImageHeight"),               QLatin1String("Xmp.tiff.ImageLength"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.BitsPerSample"),             QLatin1String("Xmp.tiff.BitsPerSample"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.PhotometricInterpretation"), QLatin1String("Xmp.tiff.PhotometricInterpretation"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.SamplesPerPixel"),           QLatin1String("Xmp.tiff.SamplesPerPixel"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.PlanarConfiguration"),       QLatin1String("Xmp.tiff.PlanarConfiguration"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.Orientation"),               QLatin1String("Xmp.tiff.Orientation"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.NativeDigest"),              QLatin1String("Xmp.tiff.NativeDigest"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.Compression"),               QLatin1String("Xmp.tiff.Compression"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Camera.Make"),                     QLatin1String("Xmp.tiff.Make"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Camera.Model"),                    QLatin1String("Xmp.tiff.Model"));
        // Xmp xmp group
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Image.CreatorTool"),                QLatin1String("Xmp.xmp.CreatorTool"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Image.Rating"),                     QLatin1String("Xmp.xmp.Rating"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Time.CreateDate"),                  QLatin1String("Xmp.xmp.CreateDate"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Time.ModifyDate"),                  QLatin1String("Xmp.xmp.ModifyDate"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Time.MetadataDate"),                QLatin1String("Xmp.xmp.MetadataDate"));
        // Xmp exif Group
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Time.DateTimeOriginal"),           QLatin1String("Xmp.exif.DateTimeOriginal"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Image.ColorSpace"),                QLatin1String("Xmp.exif.ColorSpace"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Image.ExifImageWidth"),            QLatin1String("Xmp.exif.PixelXDimension"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Image.ExifImageHeight"),           QLatin1String("Xmp.exif.PixelYDimension"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Image.NativeDigest"),              QLatin1String("Xmp.exif.NativeDigest"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Location.GPSVersionID"),           QLatin1String("Xmp.exif.GPSVersionID"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Location.GPSMapDatum"),            QLatin1String("Xmp.exif.GPSMapDatum"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Location.GPSAltitudeRef"),         QLatin1String("Xmp.exif.GPSAltitudeRef"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Location.GPSAltitude"),            QLatin1String("Xmp.exif.GPSAltitude"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Location.GPSLatitude"),            QLatin1String("Xmp.exif.GPSLatitude"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Location.GPSLongitude"),           QLatin1String("Xmp.exif.GPSLongitude"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Image.GPSLatitudeRef"),            QLatin1String("Xmp.exif.GPSLatitudeRef"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Image.GPSLongitudeRef"),           QLatin1String("Xmp.exif.GPSLongitudeRef"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Image.UserComment"),               QLatin1String("Xmp.exif.UserComment"));
        // Xmp photoshop Group
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-photoshop.Time.DateCreated"),           QLatin1String("Xmp.photoshop.DateCreated"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-photoshop.Image.Urgency"),              QLatin1String("Xmp.photoshop.Urgency"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-photoshop.Image.ColorMode"),            QLatin1String("Xmp.photoshop.ColorMode"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-photoshop.Image.History"),              QLatin1String("Xmp.photoshop.History"));
        // Xmp digiKam group
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-digiKam.Image.PickLabel"),              QLatin1String("Xmp.digiKam.PickLabel"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-digiKam.Image.ColorLabel"),             QLatin1String("Xmp.digiKam.ColorLabel"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-digiKam.Image.TagsList"),               QLatin1String("Xmp.digiKam.TagsList"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-digiKam.Image.ImageUniqueID"),          QLatin1String("Xmp.digiKam.ImageUniqueID"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-digiKam.Image.ImageHistory"),           QLatin1String("Xmp.digiKam.ImageHistory"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-digiKam.Image.CaptionsAuthorNames"),    QLatin1String("Xmp.digiKam.CaptionsAuthorNames"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-digiKam.Time.CaptionsDateTimeStamps"),  QLatin1String("Xmp.digiKam.CaptionsDateTimeStamps"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-tmp0.Unknown.PickLabel"),               QLatin1String("Xmp.digiKam.PickLabel"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-tmp0.Unknown.ColorLabel"),              QLatin1String("Xmp.digiKam.ColorLabel"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-tmp0.Unknown.TagsList"),                QLatin1String("Xmp.digiKam.TagsList"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-kipi.Unknown.PanoramaInputFiles"),      QLatin1String("Xmp.digiKam.PanoramaInputFiles"));
        // Xmp microsoft Group
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-microsoft.Image.RatingPercent"),        QLatin1String("Xmp.MicrosoftPhoto.Rating"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-microsoft.Image.LastKeywordXMP"),       QLatin1String("Xmp.MicrosoftPhoto.LastKeywordXMP"));
        // Xmp mwg-rs Group
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-mwg-rs.Image.RegionList"),              QLatin1String("Xmp.mwg-rs.Regions"));
        // Xmp xmpMM Group
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.DocumentID"),               QLatin1String("Xmp.xmpMM.DocumentID"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.InstanceID"),               QLatin1String("Xmp.xmpMM.InstanceID"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.DerivedFromInstanceID"),    QLatin1String("Xmp.xmpMM.DerivedFrom/stRef:instanceID"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.DerivedFromDocumentID"),    QLatin1String("Xmp.xmpMM.DerivedFrom/stRef:documentID"));
        // Xmp MP group
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-MP.Image.RegionInfoRegions"),           QLatin1String("Xmp.MP.RegionInfo"));
        // Xmp lr group
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-lr.Image.HierarchicalSubject"),         QLatin1String("Xmp.lr.hierarchicalSubject"));
        // Xmp dc Group
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Image.Subject"),                     QLatin1String("Xmp.dc.subject"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Image.Description"),                 QLatin1String("Xmp.dc.description"));
        mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Image.Format"),                      QLatin1String("Xmp.dc.format"));

        // --- Create the inverse map of tag names

        QHash<QString, QString>::const_iterator it = mapETtoExiv2.constBegin();

        while (it != mapETtoExiv2.constEnd())
        {
            mapExiv2toET.insert(it.value(), it.key());
            ++it;
        }

        // --- Init list of ignored groups from ExifTool.

        ignoreGroupsET
                       << QLatin1String("...")                          // Internal info from ExifTool
                       << QLatin1String("ExifTool.ExifTool.ExifTool.")  // Internal info from ExifTool
                       << QLatin1String("File.System.")                 // Info from file system
                       << QLatin1String("File.File.")                   // Info from file system
                       << QLatin1String("JFIF.JFIF.")                   // JFIF section             (do not exist yet in Exiv2)
                       << QLatin1String("ICC_Profile.")                 // Icc profile section      (do not exist yet in Exiv2)
                       << QLatin1String("Composite.")                   // Exif extended section ?  (do not exist yet in Exiv2)
                       << QLatin1String("Photoshop.")                   // Exif App15 section ?     (do not exist yet in Exiv2)
                       ;
    }

public:

    /**
     * Internal map to translate ExifTool Tag names to Exiv2 Tags
     * Key   = ExifTool tag name properties "group0.group1.group2.name"
     * value = Exiv2 tag name properties "family.group.name"
     */
    QHash<QString, QString> mapETtoExiv2;

    /**
     * Same than previous map but in inverse order.
     */
    QHash<QString, QString> mapExiv2toET;

    /**
     * List of groups to ignore from Exiftool
     */
    QStringList             ignoreGroupsET;
};

ExifToolTranslator::ExifToolTranslator()
    : d(new Private)
{
}

ExifToolTranslator::~ExifToolTranslator()
{
    delete d;
}

bool ExifToolTranslator::isBlackListedGroup(const QString& group) const
{
    foreach (const QString& bgrp, d->ignoreGroupsET)
    {
        if (group.startsWith(bgrp))
        {
            return true;
        }
    }

    return false;
}

QString ExifToolTranslator::translateToExiv2(const QString& tagName) const
{
    QHash<QString, QString>::iterator it = d->mapETtoExiv2.find(tagName);

    if (it != d->mapETtoExiv2.end())
    {
        return it.value();
    }

    return QString();
}

QString ExifToolTranslator::translateToExifTool(const QString& tagName) const
{
    QHash<QString, QString>::iterator it = d->mapExiv2toET.find(tagName);

    if (it != d->mapExiv2toET.end())
    {
        return it.value();
    }

    return QString();
}

} // namespace Digikam
