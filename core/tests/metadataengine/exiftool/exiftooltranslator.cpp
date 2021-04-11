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

namespace Digikam
{

ExifToolTranslator::ExifToolTranslator()
{
    // --- Exif table

    // Exif Iop Group
    m_map.insert(QLatin1String("EXIF.InteropIFD.Image.InteropIndex"),           QLatin1String("Exif.Iop.InteroperabilityIndex"));
    m_map.insert(QLatin1String("EXIF.InteropIFD.Image.InteropVersion"),         QLatin1String("Exif.Iop.InteroperabilityVersion"));
    // Exif Image Group
    m_map.insert(QLatin1String("EXIF.IFD0.Camera.Make"),                        QLatin1String("Exif.Image.Make"));
    m_map.insert(QLatin1String("EXIF.IFD0.Camera.Model"),                       QLatin1String("Exif.Image.Model"));
    m_map.insert(QLatin1String("EXIF.IFD0.Image.ProcessingSoftware"),           QLatin1String("Exif.Image.ProcessingSoftware"));
    m_map.insert(QLatin1String("EXIF.IFD0.Image.Rating"),                       QLatin1String("Exif.Image.Rating"));
    m_map.insert(QLatin1String("EXIF.IFD0.Image.RatingPercent"),                QLatin1String("Exif.Image.RatingPercent"));
    m_map.insert(QLatin1String("EXIF.IFD0.Image.ImageDescription"),             QLatin1String("Exif.Image.ImageDescription"));
    m_map.insert(QLatin1String("EXIF.IFD0.Image.Orientation"),                  QLatin1String("Exif.Image.Orientation"));
    m_map.insert(QLatin1String("EXIF.IFD0.Image.XResolution"),                  QLatin1String("Exif.Image.XResolution"));
    m_map.insert(QLatin1String("EXIF.IFD0.Image.YResolution"),                  QLatin1String("Exif.Image.YResolution"));
    m_map.insert(QLatin1String("EXIF.IFD0.Image.ResolutionUnit"),               QLatin1String("Exif.Image.ResolutionUnit"));
    m_map.insert(QLatin1String("EXIF.IFD0.Image.Software"),                     QLatin1String("Exif.Image.Software"));
    m_map.insert(QLatin1String("EXIF.IFD0.Image.YCbCrPositioning"),             QLatin1String("Exif.Image.YCbCrPositioning"));
    m_map.insert(QLatin1String("EXIF.IFD0.Time.ModifyDate"),                    QLatin1String(""));
    // Exif Photo Group
    m_map.insert(QLatin1String("EXIF.ExifIFD.Image.ColorSpace"),                QLatin1String("Exif.Photo.ColorSpace"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Image.ExifImageWidth"),            QLatin1String("Exif.Photo.PixelXDimension"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Image.ExifImageHeight"),           QLatin1String("Exif.Photo.PixelYDimension"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Image.ExposureTime"),              QLatin1String("Exif.Photo.ExposureTime"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Image.FNumber"),                   QLatin1String("Exif.Photo.FNumber"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Image.ISO"),                       QLatin1String("Exif.Photo.ISOSpeed"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Image.ExifVersion"),               QLatin1String("Exif.Photo.ExifVersion"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Image.ComponentsConfiguration"),   QLatin1String("Exif.Photo.ComponentsConfiguration"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Image.CompressedBitsPerPixel"),    QLatin1String("Exif.Photo.CompressedBitsPerPixel"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Image.ExposureCompensation"),      QLatin1String("Exif.Photo.ExposureBiasValue"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Image.FlashpixVersion"),           QLatin1String("Exif.Photo.FlashpixVersion"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Image.FileSource"),                QLatin1String("Exif.Photo.FileSource"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Image.SceneType"),                 QLatin1String("Exif.Photo.SceneType"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Image.CustomRendered"),            QLatin1String("Exif.Photo.CustomRendered"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Camera.ExposureProgram"),          QLatin1String("Exif.Photo.ExposureProgram"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Camera.MaxApertureValue"),         QLatin1String("Exif.Photo.MaxApertureValue"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Camera.MeteringMode"),             QLatin1String("Exif.Photo.MeteringMode"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Camera.LightSource"),              QLatin1String("Exif.Photo.LightSource"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Camera.Flash"),                    QLatin1String("Exif.Photo.Flash"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Camera.FocalLength"),              QLatin1String("Exif.Photo.FocalLength"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Camera.ExposureMode"),             QLatin1String("Exif.Photo.ExposureMode"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Camera.DigitalZoomRatio"),         QLatin1String("Exif.Photo.DigitalZoomRatio"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Camera.FocalLengthIn35mmFormat"),  QLatin1String("Exif.Photo.FocalLengthIn35mmFilm"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Camera.SceneCaptureType"),         QLatin1String("Exif.Photo.SceneCaptureType"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Camera.GainControl"),              QLatin1String("Exif.Photo.GainControl"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Camera.Contrast"),                 QLatin1String("Exif.Photo.Contrast"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Camera.Saturation"),               QLatin1String("Exif.Photo.Saturation"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Camera.Sharpness"),                QLatin1String("Exif.Photo.Sharpness"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Camera.SubjectDistanceRange"),     QLatin1String("Exif.Photo.SubjectDistanceRange"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Camera.WhiteBalance"),             QLatin1String("Exif.Photo.WhiteBalance"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Time.DateTimeOriginal"),           QLatin1String("Exif.Photo.DateTimeOriginal"));
    m_map.insert(QLatin1String("EXIF.ExifIFD.Time.CreateDate"),                 QLatin1String("Exif.Photo.DateTimeDigitized"));
    // Exif Thumbnail Group
    m_map.insert(QLatin1String("EXIF.IFD1.Image.Compression"),                  QLatin1String("Exif.Thumbnail.Compression"));
    m_map.insert(QLatin1String("EXIF.IFD1.Image.ThumbnailOffset"),              QLatin1String(""));
    m_map.insert(QLatin1String("EXIF.IFD1.Image.ThumbnailLength"),              QLatin1String("Exif.Thumbnail.JPEGInterchangeFormatLength"));
    m_map.insert(QLatin1String("EXIF.IFD1.Image.XResolution"),                  QLatin1String("Exif.Thumbnail.XResolution"));
    m_map.insert(QLatin1String("EXIF.IFD1.Image.YResolution"),                  QLatin1String("Exif.Thumbnail.YResolution"));
    m_map.insert(QLatin1String("EXIF.IFD1.Image.ResolutionUnit"),               QLatin1String("Exif.Thumbnail.ResolutionUnit"));

    // --- Iptc table

    // Iptc Envelope Group
    m_map.insert(QLatin1String("IPTC.IPTC.Other.CodedCharacterSet"),            QLatin1String("Iptc.Envelope.CharacterSet"));
    // Iptc Application2 Group
    m_map.insert(QLatin1String("IPTC.IPTC.Other.ApplicationRecordVersion"),     QLatin1String("Iptc.Application2.RecordVersion"));
    m_map.insert(QLatin1String("IPTC.IPTC.Other.Caption-Abstract"),             QLatin1String("Iptc.Application2.Caption"));
    m_map.insert(QLatin1String("IPTC.IPTC.Other.OriginatingProgram"),           QLatin1String("Iptc.Application2.Program"));
    m_map.insert(QLatin1String("IPTC.IPTC.Other.ProgramVersion"),               QLatin1String("Iptc.Application2.ProgramVersion"));
    m_map.insert(QLatin1String("IPTC.IPTC.Other.Keywords"),                     QLatin1String("Iptc.Application2.Keywords"));
    m_map.insert(QLatin1String("IPTC.IPTC.Time.DateCreated"),                   QLatin1String("Iptc.Application2.DateCreated"));
    m_map.insert(QLatin1String("IPTC.IPTC.Time.TimeCreated"),                   QLatin1String("Iptc.Application2.TimeCreated"));

    // --- Xmp table

    m_map.insert(QLatin1String("XMP.XMP-x.Document.XMPToolkit"),                QLatin1String(""));
    m_map.insert(QLatin1String("XMP.XMP-tiff.Image.Software"),                  QLatin1String("Xmp.tiff.Software"));
    m_map.insert(QLatin1String("XMP.XMP-tiff.Time.DateTime"),                   QLatin1String("Xmp.tiff.DateTime"));
    m_map.insert(QLatin1String("XMP.XMP-xmp.Image.CreatorTool"),                QLatin1String("Xmp.xmp.CreatorTool"));
    m_map.insert(QLatin1String("XMP.XMP-xmp.Image.Rating"),                     QLatin1String("Xmp.xmp.Rating"));
    m_map.insert(QLatin1String("XMP.XMP-xmp.Time.CreateDate"),                  QLatin1String("Xmp.xmp.CreateDate"));
    m_map.insert(QLatin1String("XMP.XMP-xmp.Time.ModifyDate"),                  QLatin1String("Xmp.xmp.ModifyDate"));
    m_map.insert(QLatin1String("XMP.XMP-xmp.Time.MetadataDate"),                QLatin1String("Xmp.xmp.MetadataDate"));
    m_map.insert(QLatin1String("XMP.XMP-exif.Time.DateTimeOriginal"),           QLatin1String("Xmp.exif.DateTimeOriginal"));
    m_map.insert(QLatin1String("XMP.XMP-photoshop.Time.DateCreated"),           QLatin1String("Xmp.photoshop.DateCreated"));
    m_map.insert(QLatin1String("XMP.XMP-photoshop.Image.Urgency"),              QLatin1String("Xmp.photoshop.Urgency"));
    m_map.insert(QLatin1String("XMP.XMP-tmp0.Unknown.PickLabel"),               QLatin1String("Xmp.digiKam.PickLabel"));
    m_map.insert(QLatin1String("XMP.XMP-tmp0.Unknown.ColorLabel"),              QLatin1String("Xmp.digiKam.ColorLabel"));
    m_map.insert(QLatin1String("XMP.XMP-tmp0.Unknown.TagsList"),                QLatin1String("Xmp.digiKam.TagsList"));
    m_map.insert(QLatin1String("XMP.XMP-microsoft.Image.RatingPercent"),        QLatin1String("Xmp.MicrosoftPhoto.Rating"));
    m_map.insert(QLatin1String("XMP.XMP-microsoft.Image.LastKeywordXMP"),       QLatin1String("Xmp.MicrosoftPhoto.LastKeywordXMP"));
    m_map.insert(QLatin1String("XMP.XMP-mwg-rs.Image.RegionList"),              QLatin1String("Xmp.mwg-rs.Regions"));
    m_map.insert(QLatin1String("XMP.XMP-MP.Image.RegionInfoRegions"),           QLatin1String("Xmp.MP.RegionInfo"));
    m_map.insert(QLatin1String("XMP.XMP-lr.Image.HierarchicalSubject"),         QLatin1String("Xmp.lr.hierarchicalSubject"));
    m_map.insert(QLatin1String("XMP.XMP-dc.Image.Subject"),                     QLatin1String("Xmp.dc.subject"));
}

ExifToolTranslator::~ExifToolTranslator()
{
}

QString ExifToolTranslator::translateToExiv2(const QString& tagName) const
{
    QHash<QString, QString>::const_iterator it = m_map.find(tagName);

    if (it != m_map.constEnd())
    {
        return it.value();
    }

    return QString();
}

QString ExifToolTranslator::translateToExifTool(const QString& tagName) const
{
    return m_map.key(tagName);
}

} // namespace Digikam
