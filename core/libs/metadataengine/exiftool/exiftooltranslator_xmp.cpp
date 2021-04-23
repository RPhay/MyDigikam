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

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-x.Document.XMPToolkit"),                             QLatin1String("..."));        // TODO: do not exist yet in Exiv2

    // Xmp aux Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-aux.Camera.FlashCompensation"),                      QLatin1String("Xmp.aux.FlashCompensation"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-aux.Camera.Lens"),                                   QLatin1String("Xmp.aux.Lens"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-aux.Camera.LensID"),                                 QLatin1String("Xmp.aux.LensID"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-aux.Camera.LensInfo"),                               QLatin1String("Xmp.aux.LensInfo"));

    // Xmp tiff Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Time.DateTime"),                                QLatin1String("Xmp.tiff.DateTime"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.Software"),                               QLatin1String("Xmp.tiff.Software"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.XResolution"),                            QLatin1String("Xmp.tiff.XResolution"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.YResolution"),                            QLatin1String("Xmp.tiff.YResolution"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.ResolutionUnit"),                         QLatin1String("Xmp.tiff.ResolutionUnit"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.ImageWidth"),                             QLatin1String("Xmp.tiff.ImageWidth"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.ImageHeight"),                            QLatin1String("Xmp.tiff.ImageLength"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.BitsPerSample"),                          QLatin1String("Xmp.tiff.BitsPerSample"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.PhotometricInterpretation"),              QLatin1String("Xmp.tiff.PhotometricInterpretation"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.SamplesPerPixel"),                        QLatin1String("Xmp.tiff.SamplesPerPixel"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.PlanarConfiguration"),                    QLatin1String("Xmp.tiff.PlanarConfiguration"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.Orientation"),                            QLatin1String("Xmp.tiff.Orientation"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.NativeDigest"),                           QLatin1String("Xmp.tiff.NativeDigest"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.Compression"),                            QLatin1String("Xmp.tiff.Compression"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Image.ImageDescription"),                       QLatin1String("Xmp.tiff.ImageDescription"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Camera.Make"),                                  QLatin1String("Xmp.tiff.Make"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tiff.Camera.Model"),                                 QLatin1String("Xmp.tiff.Model"));

    // Xmp xmp group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Author.BaseURL"),                                QLatin1String("Xmp.xmp.BaseURL"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Image.Advisory"),                                QLatin1String("Xmp.xmp.Advisory"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Image.CreatorTool"),                             QLatin1String("Xmp.xmp.CreatorTool"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Image.Rating"),                                  QLatin1String("Xmp.xmp.Rating"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Preview.Thumbnails"),                            QLatin1String("Xmp.xmp.Thumbnails"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Time.Identifier"),                               QLatin1String("Xmp.xmp.Identifier"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Time.Label"),                                    QLatin1String("Xmp.xmp.Label"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Time.Nickname"),                                 QLatin1String("Xmp.xmp.Nickname"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Time.CreateDate"),                               QLatin1String("Xmp.xmp.CreateDate"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Time.ModifyDate"),                               QLatin1String("Xmp.xmp.ModifyDate"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Time.MetadataDate"),                             QLatin1String("Xmp.xmp.MetadataDate"));
/*
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Author.Author"),                                 QLatin1String("Xmp.xmp."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Preview.PageImage"),                             QLatin1String("Xmp.xmp."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Preview.PageImagePageNumber"),                   QLatin1String("Xmp.xmp."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Preview.PageImageWidth"),                        QLatin1String("Xmp.xmp."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Preview.ThumbnailFormat"),                       QLatin1String("Xmp.xmp."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Preview.ThumbnailHeight"),                       QLatin1String("Xmp.xmp."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Preview.ThumbnailImage"),                        QLatin1String("Xmp.xmp."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Preview.ThumbnailWidth"),                        QLatin1String("Xmp.xmp."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Preview.Title"),                                 QLatin1String("Xmp.xmp."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Time.Description"),                              QLatin1String("Xmp.xmp."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Time.Format"),                                   QLatin1String("Xmp.xmp."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Time.Keywords"),                                 QLatin1String("Xmp.xmp."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Time.PageImageFormat"),                          QLatin1String("Xmp.xmp."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Time.PageImageHeight"),                          QLatin1String("Xmp.xmp."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmp.Time.PageInfo"),                                 QLatin1String("Xmp.xmp."));
*/
    // Xmp exif Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Image.ExifVersion"),                            QLatin1String("Xmp.exif.ExifVersion"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Time.DateTimeOriginal"),                        QLatin1String("Xmp.exif.DateTimeOriginal"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Time.DateTimeDigitized"),                       QLatin1String("Xmp.exif.DateTimeDigitized"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Image.ColorSpace"),                             QLatin1String("Xmp.exif.ColorSpace"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Image.ExifImageWidth"),                         QLatin1String("Xmp.exif.PixelXDimension"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Image.ExifImageHeight"),                        QLatin1String("Xmp.exif.PixelYDimension"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Image.NativeDigest"),                           QLatin1String("Xmp.exif.NativeDigest"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Image.GPSLatitudeRef"),                         QLatin1String("Xmp.exif.GPSLatitudeRef"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Image.GPSLongitudeRef"),                        QLatin1String("Xmp.exif.GPSLongitudeRef"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Image.UserComment"),                            QLatin1String("Xmp.exif.UserComment"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Location.GPSVersionID"),                        QLatin1String("Xmp.exif.GPSVersionID"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Location.GPSMapDatum"),                         QLatin1String("Xmp.exif.GPSMapDatum"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Location.GPSAltitudeRef"),                      QLatin1String("Xmp.exif.GPSAltitudeRef"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Location.GPSAltitude"),                         QLatin1String("Xmp.exif.GPSAltitude"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Location.GPSLatitude"),                         QLatin1String("Xmp.exif.GPSLatitude"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-exif.Location.GPSLongitude"),                        QLatin1String("Xmp.exif.GPSLongitude"));

    // Xmp photoshop Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-photoshop.Time.DateCreated"),                        QLatin1String("Xmp.photoshop.DateCreated"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-photoshop.Image.Urgency"),                           QLatin1String("Xmp.photoshop.Urgency"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-photoshop.Image.ColorMode"),                         QLatin1String("Xmp.photoshop.ColorMode"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-photoshop.Image.History"),                           QLatin1String("Xmp.photoshop.History"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-photoshop.Image.LegacyIPTCDigest"),                  QLatin1String("Xmp.photoshop.LegacyIPTCDigest"));

    // Xmp digiKam group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-digiKam.Image.PickLabel"),                           QLatin1String("Xmp.digiKam.PickLabel"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-digiKam.Image.ColorLabel"),                          QLatin1String("Xmp.digiKam.ColorLabel"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-digiKam.Image.TagsList"),                            QLatin1String("Xmp.digiKam.TagsList"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-digiKam.Image.ImageUniqueID"),                       QLatin1String("Xmp.digiKam.ImageUniqueID"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-digiKam.Image.ImageHistory"),                        QLatin1String("Xmp.digiKam.ImageHistory"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-digiKam.Image.CaptionsAuthorNames"),                 QLatin1String("Xmp.digiKam.CaptionsAuthorNames"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-digiKam.Time.CaptionsDateTimeStamps"),               QLatin1String("Xmp.digiKam.CaptionsDateTimeStamps"));
    // NOTE: From legacy namespace
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tmp0.Unknown.PickLabel"),                            QLatin1String("Xmp.digiKam.PickLabel"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tmp0.Unknown.ColorLabel"),                           QLatin1String("Xmp.digiKam.ColorLabel"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tmp0.Unknown.TagsList"),                             QLatin1String("Xmp.digiKam.TagsList"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tmp0.Unknown.ImageUniqueID"),                        QLatin1String("Xmp.digiKam.ImageUniqueID"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tmp0.Unknown.ImageHistory"),                         QLatin1String("Xmp.digiKam.ImageHistory"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tmp0.Unknown.CaptionsAuthorNames"),                  QLatin1String("Xmp.digiKam.CaptionsAuthorNames"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-tmp0.Unknown.CaptionsDateTimeStamps"),               QLatin1String("Xmp.digiKam.CaptionsDateTimeStamps"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-kipi.Unknown.PanoramaInputFiles"),                   QLatin1String("Xmp.digiKam.PanoramaInputFiles"));

    // Xmp microsoft Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-microsoft.Image.RatingPercent"),                     QLatin1String("Xmp.MicrosoftPhoto.Rating"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-microsoft.Image.LastKeywordXMP"),                    QLatin1String("Xmp.MicrosoftPhoto.LastKeywordXMP"));

    // Xmp mwg-rs Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-mwg-rs.Image.RegionList"),                           QLatin1String("Xmp.mwg-rs.Regions"));

    // Xmp xmpMM Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.DocumentID"),                            QLatin1String("Xmp.xmpMM.DocumentID"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.InstanceID"),                            QLatin1String("Xmp.xmpMM.InstanceID"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.DerivedFromInstanceID"),                 QLatin1String("Xmp.xmpMM.DerivedFrom/stRef:instanceID"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.DerivedFromDocumentID"),                 QLatin1String("Xmp.xmpMM.DerivedFrom/stRef:documentID"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.OriginalDocumentID"),                    QLatin1String("Xmp.xmpMM.OriginalDocumentID"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.PreservedFileName"),                     QLatin1String("Xmp.xmpMM.PreservedFileName"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Author.ManageTo"),                             QLatin1String("Xmp.xmpMM.ManageTo"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Author.ManageUI"),                             QLatin1String("Xmp.xmpMM.ManageUI"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Author.Manager"),                              QLatin1String("Xmp.xmpMM.Manager"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Author.ManagerVariant"),                       QLatin1String("Xmp.xmpMM.ManagerVariant"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.DerivedFrom"),                           QLatin1String("Xmp.xmpMM.DerivedFrom"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.Ingredients"),                            QLatin1String("Xmp.xmpMM.Ingredients"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.LastURL"),                                QLatin1String("Xmp.xmpMM.LastURL"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFrom"),                            QLatin1String("Xmp.xmpMM.ManagedFrom"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.Pantry"),                                 QLatin1String("Xmp.xmpMM.Pantry"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionClass"),                         QLatin1String("Xmp.xmpMM.RenditionClass"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOf"),                            QLatin1String("Xmp.xmpMM.RenditionOf"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionParams"),                        QLatin1String("Xmp.xmpMM.RenditionParams"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.SaveID"),                                 QLatin1String("Xmp.xmpMM.SaveID"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.VersionID"),                              QLatin1String("Xmp.xmpMM.VersionID"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.Versions"),                               QLatin1String("Xmp.xmpMM.Versions"));
/*
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.DerivedFromAlternatePaths"),             QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.DerivedFromFilePath"),                   QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.DerivedFromFromPart"),                   QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.DerivedFromLastModifyDate"),              QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.DerivedFromLastURL"),                     QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.DerivedFromLinkCategory"),                QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.DerivedFromLinkForm"),                    QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.DerivedFromManageTo"),                    QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.DerivedFromManageUI"),                    QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.DerivedFromManager"),                     QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.DerivedFromManagerVariant"),              QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.DerivedFromMaskMarkers"),                 QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.DerivedFromOriginalDocumentID"),          QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.DerivedFromPartMapping"),                 QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.DerivedFromPlacedResolutionUnit"),        QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.DerivedFromPlacedXResolution"),           QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.DerivedFromPlacedYResolution"),           QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.DerivedFromRenditionClass"),              QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.DerivedFromRenditionParams"),             QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.DerivedFromToPart"),                      QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.DerivedFromVersionID"),                   QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Author.Manifest"),                             QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Author.ManifestLinkForm"),                     QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Author.ManifestPlacedResolutionUnit"),         QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Author.ManifestPlacedXResolution"),            QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Author.ManifestPlacedYResolution"),            QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Author.ManifestReference"),                    QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Author.ManifestReferenceAlternatePaths"),      QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Author.ManifestReferenceDocumentID"),          QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Author.ManifestReferenceFilePath"),            QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Author.ManifestReferenceFromPart"),            QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Author.ManifestReferenceInstanceID"),          QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsAlternatePaths"),              QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsDocumentID"),                  QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsFilePath"),                    QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsFromPart"),                    QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsInstanceID"),                  QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsLastModifyDate"),              QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsLastURL"),                     QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsLinkCategory"),                QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsLinkForm"),                    QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsManageTo"),                    QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsManageUI"),                    QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsManager"),                     QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsManagerVariant"),              QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsMaskMarkers"),                 QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsOriginalDocumentID"),          QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsPartMapping"),                 QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsPlacedResolutionUnit"),        QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsPlacedXResolution"),           QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsPlacedYResolution"),           QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsRenditionClass"),              QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsRenditionParams"),             QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsToPart"),                      QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.IngredientsVersionID"),                   QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromAlternatePaths"),              QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromDocumentID"),                  QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromFilePath"),                    QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromFromPart"),                    QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromInstanceID"),                  QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromLastModifyDate"),              QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromLastURL"),                     QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromLinkCategory"),                QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromLinkForm"),                    QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromManageTo"),                    QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromManageUI"),                    QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromManager"),                     QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromManagerVariant"),              QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromMaskMarkers"),                 QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromOriginalDocumentID"),          QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromPartMapping"),                 QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromPlacedResolutionUnit"),        QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromPlacedXResolution"),           QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromPlacedYResolution"),           QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromRenditionClass"),              QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromRenditionParams"),             QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromToPart"),                      QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManagedFromVersionID"),                   QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManifestReferenceLastModifyDate"),        QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManifestReferenceLastURL"),               QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManifestReferenceLinkCategory"),          QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManifestReferenceLinkForm"),              QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManifestReferenceManageTo"),              QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManifestReferenceManageUI"),              QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManifestReferenceManager"),               QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManifestReferenceManagerVariant"),        QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManifestReferenceMaskMarkers"),           QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManifestReferenceOriginalDocumentID"),    QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManifestReferencePartMapping"),           QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManifestReferencePlacedResolutionUnit"),  QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManifestReferencePlacedXResolution"),     QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManifestReferencePlacedYResolution"),     QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManifestReferenceRenditionClass"),        QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManifestReferenceRenditionParams"),       QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManifestReferenceToPart"),                QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.ManifestReferenceVersionID"),             QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.PantryInstanceID"),                       QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfAlternatePaths"),              QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfDocumentID"),                  QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfFilePath"),                    QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfFromPart"),                    QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfInstanceID"),                  QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfLastModifyDate"),              QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfLastURL"),                     QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfLinkCategory"),                QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfLinkForm"),                    QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfManageTo"),                    QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfManageUI"),                    QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfManager"),                     QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfManagerVariant"),              QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfMaskMarkers"),                 QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfOriginalDocumentID"),          QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfPartMapping"),                 QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfPlacedResolutionUnit"),        QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfPlacedXResolution"),           QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfPlacedYResolution"),           QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfRenditionClass"),              QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfRenditionParams"),             QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfToPart"),                      QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.RenditionOfVersionID"),                   QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.Subject"),                                QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.VersionsComments"),                       QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.VersionsEvent"),                          QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.VersionsEventAction"),                    QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.VersionsEventChanged"),                   QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.VersionsEventInstanceID"),                QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.VersionsEventParameters"),                QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.VersionsEventSoftwareAgent"),             QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.VersionsEventWhen"),                      QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.VersionsModifier"),                       QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.VersionsModifyDate"),                     QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.VersionsVersion"),                        QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.History"),                                QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.HistoryAction"),                          QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.HistoryChanged"),                         QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.HistoryInstanceID"),                      QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.HistoryParameters"),                      QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.HistorySoftwareAgent"),                   QLatin1String("Xmp.xmpMM."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.HistoryAction"),                         QLatin1String("..."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.HistoryChanged"),                        QLatin1String("..."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.HistoryInstanceID"),                     QLatin1String("..."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Other.HistorySoftwareAgent"),                  QLatin1String("..."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpMM.Time.HistoryWhen"),                            QLatin1String("..."));
*/
    // Xmp MP group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-MP.Image.RegionInfoRegions"),                        QLatin1String("Xmp.MP.RegionInfo"));

    // Xmp lr group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-lr.Image.HierarchicalSubject"),                      QLatin1String("Xmp.lr.hierarchicalSubject"));

    // Xmp dc Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Author.Creator"),                                 QLatin1String("Xmp.dc.creator"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Author.Rights"),                                  QLatin1String("Xmp.dc.rights"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Author.Contributor"),                             QLatin1String("Xmp.dc.contributor"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Author.Coverage"),                                QLatin1String("Xmp.dc.coverage"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Author.Publisher"),                               QLatin1String("Xmp.dc.publisher"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Author.Relation"),                                QLatin1String("Xmp.dc.relation"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Author.Source"),                                  QLatin1String("Xmp.dc.source"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Image.Subject"),                                  QLatin1String("Xmp.dc.subject"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Image.Description"),                              QLatin1String("Xmp.dc.description"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Image.Format"),                                   QLatin1String("Xmp.dc.format"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Image.Title"),                                    QLatin1String("Xmp.dc.title"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Image.Identifier"),                               QLatin1String("Xmp.dc.identifier"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Image.Language"),                                 QLatin1String("Xmp.dc.language"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Image.Type"),                                     QLatin1String("Xmp.dc.type"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-dc.Time.Date"),                                      QLatin1String("Xmp.dc.date"));

    // Xmp rdf Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-rdf.Document.About"),                                QLatin1String("..."));        // TODO: do not exist yet in Exiv2

    // Xmp video Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-video.Unknown.DateTimeDigitized"),                   QLatin1String("Xmp.video.DateTimeDigitized"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-video.Unknown.DateTimeOriginal"),                    QLatin1String("Xmp.video.DateTimeOriginal"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-video.Unknown.DateUTC"),                             QLatin1String("Xmp.video.DateUTC"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-video.Unknown.ModificationDate"),                    QLatin1String("Xmp.video.ModificationDate"));

    // Xmp iptcExt Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-iptcExt.Location.LocationCreatedCity"),              QLatin1String("Xmp.iptcExt.LocationCreated"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-iptcExt.Location.LocationCreatedCountryName"),       QLatin1String("Xmp.iptcExt.CountryName"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-iptcExt.Location.LocationCreatedProvinceState"),     QLatin1String("Xmp.iptcExt.ProvinceState"));

    // Xmp crs Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-crs.Image.AlreadyApplied"),                          QLatin1String("Xmp.crs.AlreadyApplied"));

    // Xmp xmpRights Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpRights.Author.Certificate"),                      QLatin1String("Xmp.xmpRights.Certificate"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpRights.Author.Marked"),                           QLatin1String("Xmp.xmpRights.Marked"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpRights.Author.Owner"),                            QLatin1String("Xmp.xmpRights.Owner"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpRights.Author.UsageTerms"),                       QLatin1String("Xmp.xmpRights.UsageTerms"));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpRights.Author.WebStatement"),                     QLatin1String("Xmp.xmpRights.WebStatement"));

    // Xmp xmpBJ Group

    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpBJ.Other.JobRef"),                                QLatin1String("Xmp.xmpBJ.JobRef"));
/*
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpBJ.Other.JobRefId"),                              QLatin1String("Xmp.xmpBJ."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpBJ.Other.JobRefName"),                            QLatin1String("Xmp.xmpBJ."));
    mapETtoExiv2.insert(QLatin1String("XMP.XMP-xmpBJ.Other.JobRefUrl"),                             QLatin1String("Xmp.xmpBJ."));
*/
}

} // namespace Digikam
