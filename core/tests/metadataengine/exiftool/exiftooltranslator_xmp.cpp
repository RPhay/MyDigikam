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

void ExifToolTranslator::Private::populateXmpHashTable()
{
    // --- XMP Table --------------------------------------------------------------------

    // Xmp x group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-x.Document.XMPToolkit"),                         QLatin1String("..."));        // TODO: do not exist yet in Exiv2

    // Xmp aux Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-aux.Camera.FlashCompensation"),                  QLatin1String("Xmp.aux.FlashCompensation"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-aux.Camera.Lens"),                               QLatin1String("Xmp.aux.Lens"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-aux.Camera.LensID"),                             QLatin1String("Xmp.aux.LensID"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-aux.Camera.LensInfo"),                           QLatin1String("Xmp.aux.LensInfo"));

    // Xmp tiff Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Time.DateTime"),                            QLatin1String("Xmp.tiff.DateTime"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.Software"),                           QLatin1String("Xmp.tiff.Software"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.XResolution"),                        QLatin1String("Xmp.tiff.XResolution"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.YResolution"),                        QLatin1String("Xmp.tiff.YResolution"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.ResolutionUnit"),                     QLatin1String("Xmp.tiff.ResolutionUnit"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.ImageWidth"),                         QLatin1String("Xmp.tiff.ImageWidth"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.ImageHeight"),                        QLatin1String("Xmp.tiff.ImageLength"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.BitsPerSample"),                      QLatin1String("Xmp.tiff.BitsPerSample"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.PhotometricInterpretation"),          QLatin1String("Xmp.tiff.PhotometricInterpretation"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.SamplesPerPixel"),                    QLatin1String("Xmp.tiff.SamplesPerPixel"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.PlanarConfiguration"),                QLatin1String("Xmp.tiff.PlanarConfiguration"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.Orientation"),                        QLatin1String("Xmp.tiff.Orientation"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.NativeDigest"),                       QLatin1String("Xmp.tiff.NativeDigest"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.Compression"),                        QLatin1String("Xmp.tiff.Compression"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.ImageDescription"),                   QLatin1String("Xmp.tiff.ImageDescription"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Camera.Make"),                              QLatin1String("Xmp.tiff.Make"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Camera.Model"),                             QLatin1String("Xmp.tiff.Model"));

    // Xmp xmp group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Image.CreatorTool"),                         QLatin1String("Xmp.xmp.CreatorTool"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Image.Rating"),                              QLatin1String("Xmp.xmp.Rating"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Time.CreateDate"),                           QLatin1String("Xmp.xmp.CreateDate"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Time.ModifyDate"),                           QLatin1String("Xmp.xmp.ModifyDate"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Time.MetadataDate"),                         QLatin1String("Xmp.xmp.MetadataDate"));

    // Xmp exif Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Time.DateTimeOriginal"),                    QLatin1String("Xmp.exif.DateTimeOriginal"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Time.DateTimeDigitized"),                   QLatin1String("Xmp.exif.DateTimeDigitized"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Image.ColorSpace"),                         QLatin1String("Xmp.exif.ColorSpace"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Image.ExifImageWidth"),                     QLatin1String("Xmp.exif.PixelXDimension"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Image.ExifImageHeight"),                    QLatin1String("Xmp.exif.PixelYDimension"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Image.NativeDigest"),                       QLatin1String("Xmp.exif.NativeDigest"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Image.GPSLatitudeRef"),                     QLatin1String("Xmp.exif.GPSLatitudeRef"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Image.GPSLongitudeRef"),                    QLatin1String("Xmp.exif.GPSLongitudeRef"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Image.UserComment"),                        QLatin1String("Xmp.exif.UserComment"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Location.GPSVersionID"),                    QLatin1String("Xmp.exif.GPSVersionID"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Location.GPSMapDatum"),                     QLatin1String("Xmp.exif.GPSMapDatum"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Location.GPSAltitudeRef"),                  QLatin1String("Xmp.exif.GPSAltitudeRef"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Location.GPSAltitude"),                     QLatin1String("Xmp.exif.GPSAltitude"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Location.GPSLatitude"),                     QLatin1String("Xmp.exif.GPSLatitude"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Location.GPSLongitude"),                    QLatin1String("Xmp.exif.GPSLongitude"));

    // Xmp photoshop Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-photoshop.Time.DateCreated"),                    QLatin1String("Xmp.photoshop.DateCreated"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-photoshop.Image.Urgency"),                       QLatin1String("Xmp.photoshop.Urgency"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-photoshop.Image.ColorMode"),                     QLatin1String("Xmp.photoshop.ColorMode"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-photoshop.Image.History"),                       QLatin1String("Xmp.photoshop.History"));

    // Xmp digiKam group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-digiKam.Image.PickLabel"),                       QLatin1String("Xmp.digiKam.PickLabel"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-digiKam.Image.ColorLabel"),                      QLatin1String("Xmp.digiKam.ColorLabel"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-digiKam.Image.TagsList"),                        QLatin1String("Xmp.digiKam.TagsList"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-digiKam.Image.ImageUniqueID"),                   QLatin1String("Xmp.digiKam.ImageUniqueID"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-digiKam.Image.ImageHistory"),                    QLatin1String("Xmp.digiKam.ImageHistory"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-digiKam.Image.CaptionsAuthorNames"),             QLatin1String("Xmp.digiKam.CaptionsAuthorNames"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-digiKam.Time.CaptionsDateTimeStamps"),           QLatin1String("Xmp.digiKam.CaptionsDateTimeStamps"));
    // NOTE: From legacy namespace
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tmp0.Unknown.PickLabel"),                        QLatin1String("Xmp.digiKam.PickLabel"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tmp0.Unknown.ColorLabel"),                       QLatin1String("Xmp.digiKam.ColorLabel"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tmp0.Unknown.TagsList"),                         QLatin1String("Xmp.digiKam.TagsList"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tmp0.Unknown.ImageUniqueID"),                    QLatin1String("Xmp.digiKam.ImageUniqueID"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tmp0.Unknown.ImageHistory"),                     QLatin1String("Xmp.digiKam.ImageHistory"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tmp0.Unknown.CaptionsAuthorNames"),              QLatin1String("Xmp.digiKam.CaptionsAuthorNames"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tmp0.Unknown.CaptionsDateTimeStamps"),           QLatin1String("Xmp.digiKam.CaptionsDateTimeStamps"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-kipi.Unknown.PanoramaInputFiles"),               QLatin1String("Xmp.digiKam.PanoramaInputFiles"));

    // Xmp microsoft Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-microsoft.Image.RatingPercent"),                 QLatin1String("Xmp.MicrosoftPhoto.Rating"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-microsoft.Image.LastKeywordXMP"),                QLatin1String("Xmp.MicrosoftPhoto.LastKeywordXMP"));

    // Xmp mwg-rs Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-mwg-rs.Image.RegionList"),                       QLatin1String("Xmp.mwg-rs.Regions"));

    // Xmp xmpMM Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.DocumentID"),                        QLatin1String("Xmp.xmpMM.DocumentID"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.InstanceID"),                        QLatin1String("Xmp.xmpMM.InstanceID"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.DerivedFromInstanceID"),             QLatin1String("Xmp.xmpMM.DerivedFrom/stRef:instanceID"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.DerivedFromDocumentID"),             QLatin1String("Xmp.xmpMM.DerivedFrom/stRef:documentID"));

    // Xmp MP group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-MP.Image.RegionInfoRegions"),                    QLatin1String("Xmp.MP.RegionInfo"));

    // Xmp lr group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-lr.Image.HierarchicalSubject"),                  QLatin1String("Xmp.lr.hierarchicalSubject"));

    // Xmp dc Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Image.Subject"),                              QLatin1String("Xmp.dc.subject"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Image.Description"),                          QLatin1String("Xmp.dc.description"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Image.Format"),                               QLatin1String("Xmp.dc.format"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Image.Title"),                                QLatin1String("Xmp.dc.title"));

    // Xmp rdf Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-rdf.Document.About"),                            QLatin1String("..."));        // TODO: do not exist yet in Exiv2

    // Xmp video Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-video.Unknown.DateTimeDigitized"),               QLatin1String("Xmp.video.DateTimeDigitized"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-video.Unknown.DateTimeOriginal"),                QLatin1String("Xmp.video.DateTimeOriginal"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-video.Unknown.DateUTC"),                         QLatin1String("Xmp.video.DateUTC"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-video.Unknown.ModificationDate"),                QLatin1String("Xmp.video.ModificationDate"));

    // Xmp iptcExt Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-iptcExt.Location.LocationCreatedCity"),          QLatin1String("Xmp.iptcExt.LocationCreated"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-iptcExt.Location.LocationCreatedCountryName"),   QLatin1String("Xmp.iptcExt.CountryName"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-iptcExt.Location.LocationCreatedProvinceState"), QLatin1String("Xmp.iptcExt.ProvinceState"));
}

} // namespace Digikam
