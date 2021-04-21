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

void ExifToolTranslator::Private::populateIptcHashTable()
{
    // --- IPTC Table -------------------------------------------------------------------
/*
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Author.By-lineTitle"),                       QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Author.Caption-Abstract"),                   QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Author.CopyrightNotice"),                    QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Author.LocalCaption"),                       QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Author.Writer-Editor"),                      QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.AudioSamplingRate"),                   QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.AudioSamplingResolution"),             QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.BitsPerComponent"),                    QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.CatalogSets"),                         QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.ClassifyState"),                       QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.ColorCalibrationMatrix"),              QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.ColorPalette"),                        QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.ColorRepresentation"),                 QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.ColorSequence"),                       QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.DataCompressionMethod"),               QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.DocumentHistory"),                     QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.DocumentNotes"),                       QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.EndPoints"),                           QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.ExcursionTolerance"),                  QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.ExifCameraInfo"),                      QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.GammaCompensatedValue"),               QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.ICC_Profile"),                         QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.IPTCBitsPerSample"),                   QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.IPTCImageHeight"),                     QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.IPTCImageRotation"),                   QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.IPTCImageWidth"),                      QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.IPTCPictureNumber"),                   QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.IPTCPixelHeight"),                     QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.IPTCPixelWidth"),                      QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.InterchangeColorSpace"),               QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.JobID"),                               QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.LanguageIdentifier"),                  QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.LookupTable"),                         QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.MasterDocumentID"),                    QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.MaximumDensityRange"),                 QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.NewsPhotoVersion"),                    QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.NumIndexEntries"),                     QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.ObjectPreviewFileFormat"),             QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.ObjectPreviewFileVersion"),            QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.OwnerID"),                             QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.Prefs"),                               QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.ProductID"),                           QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.QuantizationMethod"),                  QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.SampleStructure"),                     QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.ScanningDirection"),                   QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.ServiceIdentifier"),                   QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.ShortDocumentID"),                     QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.SimilarityIndex"),                     QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.SupplementalType"),                    QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.UniqueDocumentID"),                    QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Location.ContentLocationCode"),              QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Location.ContentLocationName"),              QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Location.Country-PrimaryLocationCode"),      QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Location.Country-PrimaryLocationName"),      QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Location.OriginalTransmissionReference"),    QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Location.Province-State"),                   QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Location.Sub-location"),                     QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.ApplicationRecordVersion"),            QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.ConfirmedObjectSize"),                 QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.EnvelopeRecordVersion"),               QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.FixtureIdentifier"),                   QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.MaxSubfileSize"),                      QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.MaximumObjectSize"),                   QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.ObjectAttributeReference"),            QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.ObjectSizeAnnounced"),                 QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.ObjectTypeReference"),                 QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.SizeMode"),                            QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.SubFile"),                             QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.SubjectReference"),                    QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.SupplementalCategories"),              QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Preview.ObjectPreviewData"),                 QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.ARMIdentifier"),                        QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.CodedCharacterSet"),                    QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.DigitalCreationDate"),                  QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.DigitalCreationTime"),                  QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.OriginatingProgram"),                   QLatin1String("Iptc.Application2."));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.UniqueObjectName"),                     QLatin1String("Iptc.Application2."));
*/
    // Iptc Envelope Group

    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.CodedCharacterSet"),                   QLatin1String("Iptc.Envelope.CharacterSet"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.EnvelopeRecordVersion"),               QLatin1String("Iptc.Envelope.ModelVersion"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.TimeSent"),                             QLatin1String("Iptc.Envelope.TimeSent"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.EnvelopeNumber"),                      QLatin1String("Iptc.Envelope.EnvelopeNumber"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.EnvelopePriority"),                    QLatin1String("Iptc.Envelope.EnvelopePriority"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.FileFormat"),                          QLatin1String("Iptc.Envelope.FileFormat"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.FileVersion"),                         QLatin1String("Iptc.Envelope.FileVersion"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Location.Destination"),                      QLatin1String("Iptc.Envelope.Destination"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.ARMVersion"),                           QLatin1String("Iptc.Envelope.ARMVersion"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.DateSent"),                             QLatin1String("Iptc.Envelope.DateSent"));

    // Iptc Application2 Group

    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.ApplicationRecordVersion"),            QLatin1String("Iptc.Application2.RecordVersion"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.Caption-Abstract"),                    QLatin1String("Iptc.Application2.Caption"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.OriginatingProgram"),                  QLatin1String("Iptc.Application2.Program"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.Category"),                            QLatin1String("Iptc.Application2.Category"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.EditStatus"),                          QLatin1String("Iptc.Application2.EditStatus"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.EditorialUpdate"),                     QLatin1String("Iptc.Application2.EditorialUpdate"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.Keywords"),                            QLatin1String("Iptc.Application2.Keywords"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.ObjectName"),                          QLatin1String("Iptc.Application2.ObjectName"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Other.Urgency"),                             QLatin1String("Iptc.Application2.Urgency"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Author.By-line"),                            QLatin1String("Iptc.Application2.Byline"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Author.CopyrightNotice"),                    QLatin1String("Iptc.Application2.Copyright"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Author.Contact"),                            QLatin1String("Iptc.Application2.Contact"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Author.Credit"),                             QLatin1String("Iptc.Application2.Credit"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Author.RasterizedCaption"),                  QLatin1String("Iptc.Application2.RasterizedCaption"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Author.Source"),                             QLatin1String("Iptc.Application2.Source"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.AudioDuration"),                       QLatin1String("Iptc.Application2.AudioDuration"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.AudioOutcue"),                         QLatin1String("Iptc.Application2.AudioOutcue"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.AudioType"),                           QLatin1String("Iptc.Application2.AudioType"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.ImageOrientation"),                    QLatin1String("Iptc.Application2.ImageOrientation"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Image.ImageType"),                           QLatin1String("Iptc.Application2.ImageType"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Location.Headline"),                         QLatin1String("Iptc.Application2.Headline"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Location.City"),                             QLatin1String("Iptc.Application2.City"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.DigitalCreationDate"),                  QLatin1String("Iptc.Application2.DigitizationDate"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.DigitalCreationTime"),                  QLatin1String("Iptc.Application2.DigitizationTime"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.ActionAdvised"),                        QLatin1String("Iptc.Application2.ActionAdvised"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.DateCreated"),                          QLatin1String("Iptc.Application2.DateCreated"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.ExpirationDate"),                       QLatin1String("Iptc.Application2.ExpirationDate"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.ExpirationTime"),                       QLatin1String("Iptc.Application2.ExpirationTime"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.ObjectCycle"),                          QLatin1String("Iptc.Application2.ObjectCycle"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.ProgramVersion"),                       QLatin1String("Iptc.Application2.ProgramVersion"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.ReferenceDate"),                        QLatin1String("Iptc.Application2.ReferenceDate"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.ReferenceNumber"),                      QLatin1String("Iptc.Application2.ReferenceNumber"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.ReferenceService"),                     QLatin1String("Iptc.Application2.ReferenceService"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.ReleaseDate"),                          QLatin1String("Iptc.Application2.ReleaseDate"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.ReleaseTime"),                          QLatin1String("Iptc.Application2.ReleaseTime"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.SpecialInstructions"),                  QLatin1String("Iptc.Application2.SpecialInstructions"));
    mapETtoExiv2.insert(QLatin1String("IPTC.IPTC.Time.TimeCreated"),                          QLatin1String("Iptc.Application2.TimeCreated"));



}

} // namespace Digikam
