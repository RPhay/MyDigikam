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
    // --- EXIF Table -------------------------------------------------------------------

    // Exif Iop Group
    m_mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Image.InteropIndex"),           QLatin1String("Exif.Iop.InteroperabilityIndex"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.InteropIFD.Image.InteropVersion"),         QLatin1String("Exif.Iop.InteroperabilityVersion"));
    // Exif Image Group
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.ImageWidth"),                   QLatin1String("Exif.Image.ImageWidth"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.ImageHeight"),                  QLatin1String("Exif.Image.ImageLength"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.BitsPerSample"),                QLatin1String("Exif.Image.BitsPerSample"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.Compression"),                  QLatin1String("Exif.Image.Compression"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.PhotometricInterpretation"),    QLatin1String("Exif.Image.PhotometricInterpretation"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.SamplesPerPixel"),              QLatin1String("Exif.Image.SamplesPerPixel"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.PlanarConfiguration"),          QLatin1String("Exif.Image.PlanarConfiguration"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.ProcessingSoftware"),           QLatin1String("Exif.Image.ProcessingSoftware"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.Rating"),                       QLatin1String("Exif.Image.Rating"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.RatingPercent"),                QLatin1String("Exif.Image.RatingPercent"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.ImageDescription"),             QLatin1String("Exif.Image.ImageDescription"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.Orientation"),                  QLatin1String("Exif.Image.Orientation"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.XResolution"),                  QLatin1String("Exif.Image.XResolution"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.YResolution"),                  QLatin1String("Exif.Image.YResolution"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.ResolutionUnit"),               QLatin1String("Exif.Image.ResolutionUnit"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.Software"),                     QLatin1String("Exif.Image.Software"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Image.YCbCrPositioning"),             QLatin1String("Exif.Image.YCbCrPositioning"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Camera.Make"),                        QLatin1String("Exif.Image.Make"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Camera.Model"),                       QLatin1String("Exif.Image.Model"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD0.Time.ModifyDate"),                    QLatin1String(""));
    // Exif Photo Group
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.ColorSpace"),                QLatin1String("Exif.Photo.ColorSpace"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.ExifImageWidth"),            QLatin1String("Exif.Photo.PixelXDimension"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.ExifImageHeight"),           QLatin1String("Exif.Photo.PixelYDimension"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.ExposureTime"),              QLatin1String("Exif.Photo.ExposureTime"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.FNumber"),                   QLatin1String("Exif.Photo.FNumber"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.ISO"),                       QLatin1String("Exif.Photo.ISOSpeed"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.ExifVersion"),               QLatin1String("Exif.Photo.ExifVersion"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.ComponentsConfiguration"),   QLatin1String("Exif.Photo.ComponentsConfiguration"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.CompressedBitsPerPixel"),    QLatin1String("Exif.Photo.CompressedBitsPerPixel"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.ExposureCompensation"),      QLatin1String("Exif.Photo.ExposureBiasValue"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.FlashpixVersion"),           QLatin1String("Exif.Photo.FlashpixVersion"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.FileSource"),                QLatin1String("Exif.Photo.FileSource"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.SceneType"),                 QLatin1String("Exif.Photo.SceneType"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Image.CustomRendered"),            QLatin1String("Exif.Photo.CustomRendered"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.ExposureProgram"),          QLatin1String("Exif.Photo.ExposureProgram"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.MaxApertureValue"),         QLatin1String("Exif.Photo.MaxApertureValue"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.MeteringMode"),             QLatin1String("Exif.Photo.MeteringMode"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.LightSource"),              QLatin1String("Exif.Photo.LightSource"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.Flash"),                    QLatin1String("Exif.Photo.Flash"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.FocalLength"),              QLatin1String("Exif.Photo.FocalLength"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.ExposureMode"),             QLatin1String("Exif.Photo.ExposureMode"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.DigitalZoomRatio"),         QLatin1String("Exif.Photo.DigitalZoomRatio"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.FocalLengthIn35mmFormat"),  QLatin1String("Exif.Photo.FocalLengthIn35mmFilm"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.SceneCaptureType"),         QLatin1String("Exif.Photo.SceneCaptureType"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.GainControl"),              QLatin1String("Exif.Photo.GainControl"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.Contrast"),                 QLatin1String("Exif.Photo.Contrast"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.Saturation"),               QLatin1String("Exif.Photo.Saturation"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.Sharpness"),                QLatin1String("Exif.Photo.Sharpness"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.SubjectDistanceRange"),     QLatin1String("Exif.Photo.SubjectDistanceRange"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Camera.WhiteBalance"),             QLatin1String("Exif.Photo.WhiteBalance"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Time.DateTimeOriginal"),           QLatin1String("Exif.Photo.DateTimeOriginal"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.ExifIFD.Time.CreateDate"),                 QLatin1String("Exif.Photo.DateTimeDigitized"));
    // Exif Thumbnail Group
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.Compression"),                  QLatin1String("Exif.Thumbnail.Compression"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.ThumbnailOffset"),              QLatin1String(""));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.ThumbnailLength"),              QLatin1String("Exif.Thumbnail.JPEGInterchangeFormatLength"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.XResolution"),                  QLatin1String("Exif.Thumbnail.XResolution"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.YResolution"),                  QLatin1String("Exif.Thumbnail.YResolution"));
    m_mapETtoExiv2.insert(QLatin1String("EXIF.IFD1.Image.ResolutionUnit"),               QLatin1String("Exif.Thumbnail.ResolutionUnit"));

    // --- IPTC Table -------------------------------------------------------------------

    // Iptc Envelope Group
    m_mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.CodedCharacterSet"),            QLatin1String("Iptc.Envelope.CharacterSet"));
    // Iptc Application2 Group
    m_mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.ApplicationRecordVersion"),     QLatin1String("Iptc.Application2.RecordVersion"));
    m_mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.Caption-Abstract"),             QLatin1String("Iptc.Application2.Caption"));
    m_mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.OriginatingProgram"),           QLatin1String("Iptc.Application2.Program"));
    m_mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.ProgramVersion"),               QLatin1String("Iptc.Application2.ProgramVersion"));
    m_mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.Keywords"),                     QLatin1String("Iptc.Application2.Keywords"));
    m_mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.DateCreated"),                   QLatin1String("Iptc.Application2.DateCreated"));
    m_mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.TimeCreated"),                   QLatin1String("Iptc.Application2.TimeCreated"));

    // --- XMP Table --------------------------------------------------------------------

    // Xmp x group
    m_mapETtoExiv2.insert(QLatin1String("XMP.XMP-x.Document.XMPToolkit"),                QLatin1String(""));
    // Xmp tiff Group
    m_mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.Software"),                  QLatin1String("Xmp.tiff.Software"));
    m_mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Time.DateTime"),                   QLatin1String("Xmp.tiff.DateTime"));
    // Xmp xmp Group
    m_mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Image.CreatorTool"),                QLatin1String("Xmp.xmp.CreatorTool"));
    m_mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Image.Rating"),                     QLatin1String("Xmp.xmp.Rating"));
    m_mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Time.CreateDate"),                  QLatin1String("Xmp.xmp.CreateDate"));
    m_mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Time.ModifyDate"),                  QLatin1String("Xmp.xmp.ModifyDate"));
    m_mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Time.MetadataDate"),                QLatin1String("Xmp.xmp.MetadataDate"));
    // Xmp exif Group
    m_mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Time.DateTimeOriginal"),           QLatin1String("Xmp.exif.DateTimeOriginal"));
    // Xmp photoshop Group
    m_mapETtoExiv2.insert(QLatin1String("XMP.XMP-photoshop.Time.DateCreated"),           QLatin1String("Xmp.photoshop.DateCreated"));
    m_mapETtoExiv2.insert(QLatin1String("XMP.XMP-photoshop.Image.Urgency"),              QLatin1String("Xmp.photoshop.Urgency"));
    // Xmp digiKam group
    m_mapETtoExiv2.insert(QLatin1String("XMP.XMP-tmp0.Unknown.PickLabel"),               QLatin1String("Xmp.digiKam.PickLabel"));
    m_mapETtoExiv2.insert(QLatin1String("XMP.XMP-tmp0.Unknown.ColorLabel"),              QLatin1String("Xmp.digiKam.ColorLabel"));
    m_mapETtoExiv2.insert(QLatin1String("XMP.XMP-tmp0.Unknown.TagsList"),                QLatin1String("Xmp.digiKam.TagsList"));
    // Xmp microsoft Group
    m_mapETtoExiv2.insert(QLatin1String("XMP.XMP-microsoft.Image.RatingPercent"),        QLatin1String("Xmp.MicrosoftPhoto.Rating"));
    m_mapETtoExiv2.insert(QLatin1String("XMP.XMP-microsoft.Image.LastKeywordXMP"),       QLatin1String("Xmp.MicrosoftPhoto.LastKeywordXMP"));
    // Xmp mwg-rs Group
    m_mapETtoExiv2.insert(QLatin1String("XMP.XMP-mwg-rs.Image.RegionList"),              QLatin1String("Xmp.mwg-rs.Regions"));
    // Xmp xmpMM Group
    m_mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.DocumentID"),               QLatin1String("Xmp.xmpMM.DocumentID"));
    // Xmp MP group
    m_mapETtoExiv2.insert(QLatin1String("XMP.XMP-MP.Image.RegionInfoRegions"),           QLatin1String("Xmp.MP.RegionInfo"));
    // Xmp lr group
    m_mapETtoExiv2.insert(QLatin1String("XMP.XMP-lr.Image.HierarchicalSubject"),         QLatin1String("Xmp.lr.hierarchicalSubject"));
    // Xmp dc Group
    m_mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Image.Subject"),                     QLatin1String("Xmp.dc.subject"));
    m_mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Image.Description"),                 QLatin1String("Xmp.dc.description"));

    // --- Create the inverse map

    QHash<QString, QString>::const_iterator it = m_mapETtoExiv2.constBegin();

    while (it != m_mapETtoExiv2.constEnd())
    {
        m_mapExiv2toET.insert(it.value(), it.key());
        ++it;
    }
}

ExifToolTranslator::~ExifToolTranslator()
{
}

QString ExifToolTranslator::translateToExiv2(const QString& tagName) const
{
    QHash<QString, QString>::const_iterator it = m_mapETtoExiv2.find(tagName);

    if (it != m_mapETtoExiv2.constEnd())
    {
        return it.value();
    }

    return QString();
}

QString ExifToolTranslator::translateToExifTool(const QString& tagName) const
{
    QHash<QString, QString>::const_iterator it = m_mapExiv2toET.find(tagName);

    if (it != m_mapExiv2toET.constEnd())
    {
        return it.value();
    }

    return QString();
}

} // namespace Digikam
