/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-25
 * Description : a tool to convert RAW file to DNG - Exif Metadata storage.
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

int DNGWriter::Private::storeMakernote(DNGWriterHost& host,
                                       AutoPtr<dng_negative>& negative,
                                       DRawInfo* const /*identify*/,
                                       DRawInfo* const identifyMake,
                                       DMetadata* const meta)
{
    QString str;

    if (metaLoaded)     // We only process Makernote storage is Metadat can be loaded at Exif stage.
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Updating Makernote metadata to DNG Negative" ;

        long int   num, den;
        long       val;

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
