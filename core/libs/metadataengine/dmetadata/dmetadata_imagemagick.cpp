/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-12-23
 * Description : item metadata interface - ImageMagick helpers.
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

#include "dmetadata.h"

// Qt includes

#include <QString>
#include <QStringList>
#include <QFileInfo>

// Local includes

#include "digikam_config.h"
#include "digikam_debug.h"
#include "drawdecoder.h"

// ImageMagick includes

#ifdef HAVE_IMAGE_MAGICK

#include <Magick++.h>

using namespace Magick;
using namespace MagickCore;

#endif // HAVE_IMAGE_MAGICK

namespace Digikam
{

bool DMetadata::loadUsingImageMagick(const QString& filePath)
{
    bool ret = false;

#ifdef HAVE_IMAGE_MAGICK

    QFileInfo fileInfo(filePath);
    QString rawFilesExt  = DRawDecoder::rawFiles();
    QString ext          = fileInfo.suffix().toUpper();

    if (
        !fileInfo.exists() || ext.isEmpty() ||
        rawFilesExt.toUpper().contains(ext) ||    // Ignore RAW files
        (ext == QLatin1String("HEIF"))      ||    // Ignore HEIF files
        (ext == QLatin1String("HEIC"))      ||    // Ignore HEIC files
        (ext == QLatin1String("XCF"))             // Ignore XCF files
       )
    {
        return false;
    }

    MagickCore::ImageInfo* image_info = nullptr;
    ExceptionInfo ex                  = *AcquireExceptionInfo();

    // Syntax description of IM percent escape formats: https://imagemagick.org/script/escape.php

    const QString filters             = QLatin1String
                                        (
                                            // Standard XMP namespaces

                                            "Xmp.tiff.ImageLength=%[height]\n"                      // Text
                                            "Xmp.tiff.ImageWidth=%[width]\n"                        // Text
                                            "Xmp.tiff.Compression=%[compression]\n"                 // Text
                                            "Xmp.tiff.BitsPerSample=%[bit-depth]\n"                 // Seq
                                            "Xmp.tiff.ImageDescription=%[caption]\n"                // LangAlt
                                            "Xmp.tiff.Orientation=%[orientation]\n"                 // Text
                                            "Xmp.tiff.DateTime=%[date:create]\n"                    // Text
                                            "Xmp.tiff.XResolution=%[resolution.x]\n"                // Text
                                            "Xmp.tiff.YResolution=%[resolution.y]\n"                // Text
                                            "Xmp.tiff.ResolutionUnit=%[units]\n"                    // Text
                                            "Xmp.exif.UserComment=%[comment]\n"                     // LangAlt
                                            "Xmp.exif.ColorSpace=%[colorspace]\n"                   // Text
                                            "Xmp.exif.DateTimeOriginal=%[date:create]\n"            // Text
                                            "Xmp.exifEX.Gamma=%[gamma]\n"                           // Text
                                            "Xmp.xmp.CreateDate=%[date:create]\n"                   // Text
                                            "Xmp.xmp.ModifyDate=%[date:modify]\n"                   // Text
                                            "Xmp.xmp.Label=%[label]\n"                              // Text

                                            // ImageMagick Attributes namepsace

                                            "Xmp.IMAttributes.Version=%[version]\n"                 // Text
                                            "Xmp.IMAttributes.Copyright=%[copyright]\n"             // Text
                                            "Xmp.IMAttributes.BaseName=%[basename]\n"               // Text
                                            "Xmp.IMAttributes.Extension=%[extension]\n"             // Text
                                            "Xmp.IMAttributes.Codec=%[magick]\n"                    // Text
                                            "Xmp.IMAttributes.Channels=%[channels]\n"               // Text
                                            "Xmp.IMAttributes.Interlace=%[interlace]\n"             // Text
                                            "Xmp.IMAttributes.Profiles=%[profiles]\n"               // Text
/*
    NOTE: ignore these attributes as these break IM identify call.

                                            "Xmp.IMAttributes.ProfileICC=%[profile:icc]\n"          // Text
                                            "Xmp.IMAttributes.ProfileICM=%[profile:icm]\n"          // Text
                                            "Xmp.IMAttributes.XPrintSize=%[printsize.x]\n"          // Text
                                            "Xmp.IMAttributes.YPrintSize=%[printsize.y]\n"          // Text
                                            "Xmp.IMAttributes.Size=%[size]\n"                       // Text
*/

/*
   NOTE: values calculated which introduce non negligible time latency:

                                            %[colors]                   (not specified in IM doc)
                                            %[entropy]                  (specified as CALCULATED in doc)
                                            %[kurtosis]                 (specified as CALCULATED in doc)
                                            %[max]                      (specified as CALCULATED in doc)
                                            %[mean]                     (specified as CALCULATED in doc)
                                            %[median]                   (specified as CALCULATED in doc)
                                            %[min]                      (specified as CALCULATED in doc)
                                            %[opaque]                   (specified as CALCULATED in doc)
                                            %[skewness]                 (specified as CALCULATED in doc)
                                            %[standard-deviation]       (specified as CALCULATED in doc)
                                            %[type]                     (specified as CALCULATED in doc)
*/

                                            "Xmp.IMAttributes.Quality=%[quality]\n"                 // Text
                                            "Xmp.IMAttributes.Rendering=%[rendering-intent]\n"      // Text
                                            "Xmp.IMAttributes.Scene=%[scene]\n"                     // Text

                                            // ImageMagick Properties namespace

                                            "%[*]\n"                                                // Text
                                        );


    try
    {
        // Allocate metadata container for IM.

        const int msize     = 256;                  // Number of internal metadata entries prepared for IM.
        char** metadata     = new char*[msize];

        for (int i = 0 ; i < msize ; ++i)
        {
            // NOTE: prepare metadata items with null pointers which will be allocated with malloc on demand by ImageMagick.

            metadata[i] = nullptr;
        }

        // Prepare image info for IM isentification

        image_info          = CloneImageInfo((ImageInfo*)nullptr);
        strcpy(image_info->filename, filePath.toLatin1().constData());
        image_info->ping    = MagickTrue;
        image_info->verbose = MagickTrue;
        image_info->debug   = MagickTrue;

        // Insert percent escape format for IM identification call.

        int identargc       = 4;                            // Number or arguments passed to IM identification call.
        char** identargv    = new char*[identargc];         // Container to store arguments.
        identargv[0]        = (char*)"identify";            // String content is not important but must be present.
        identargv[1]        = (char*)"-format";             // We will pass percent escape options.
        QByteArray ba       = filters.toLatin1();
        identargv[2]        = ba.data();                    // Percent escape format description.
        ba                  = filePath.toLatin1();
        identargv[3]        = ba.data();                    // The file path to parse (even if this also passed through IM::ImageInfo container).

        // Call ImageMagick core identification.
        // This is a fast IM C API call, not the IM CLI tool process.

        if (IdentifyImageCommand(image_info, identargc, identargv, metadata, &ex) == MagickTrue)
        {
            // Post process metadata

            registerXmpNameSpace(QLatin1String("https://imagemagick.org/IMAttributes/"), QLatin1String("IMAttributes"));
            registerXmpNameSpace(QLatin1String("https://imagemagick.org/IMProperties/"), QLatin1String("IMProperties"));

            QString output;
            int lbytes = 0;

            for (int i = 0 ; i < msize ; ++i)
            {
                if (metadata[i])
                {
                    QString tmp = QString::fromUtf8(metadata[i]);
                    output.append(tmp);
                    lbytes     += tmp.size();
/*
                    qCDebug(DIGIKAM_METAENGINE_LOG) << "Append new metadata line of" << tmp.size() << "bytes";
                    qCDebug(DIGIKAM_METAENGINE_LOG) << metadata[i];
*/
                }
            }

            QStringList lines = output.split(QLatin1Char('\n'));
/*
            qCDebug(DIGIKAM_METAENGINE_LOG) << "Total metadata lines parsed:" << lines.count() << "Total bytes:" << lbytes;
*/
            QString key;
            QString val;

            foreach (const QString& tupple, lines)
            {
                key = tupple.section(QLatin1Char('='), 0, 0);
                val = tupple.section(QLatin1Char('='), 1, 1);

                if (val.startsWith(QLatin1Char('\'')))
                {
                    val = val.section(QLatin1Char('\''), 1, 1);             // Drop tag description string on the right side (stage1).
                    val = val.remove(QLatin1Char('\''));
                }

                if (val.contains(QLatin1String(" / ")))
                {
                    val = val.section(QLatin1String(" / "), 0, 0);          // Drop tag description string on the right side (stage2).
                }

                if (key.isEmpty()                                 ||
                    key.startsWith(QLatin1String("comment"))      ||
                    key.startsWith(QLatin1String("date:create"))  ||
                    key.startsWith(QLatin1String("date:modify")))
                {
                    // These tags are already handled with Exif or key or val are empty.

                    continue;
                }

                if (val.isEmpty())
                {
                    val = QLatin1String("None");        // Mimic IM "none" strings, not i18n
                }

                if (!key.startsWith(QLatin1String("Xmp.")))
                {
                    // Create a dedicated XMP namespace to store ImageMagick properties.

                    key = QLatin1String("Xmp.IMProperties.") + key.remove(QLatin1Char(':'));
                }

                key = key.remove(QLatin1Char('-'));
                key = key.remove(QLatin1Char('_'));
                val = val.simplified();

                // Register XMP tags in container

                if      (key == QLatin1String("Xmp.tiff.ImageDescription") ||
                         key == QLatin1String("Xmp.exif.UserComment"))
                {
                    setXmpTagStringLangAlt(key.toLatin1().constData(), val, QString());
                }
                else if (key == QLatin1String("Xmp.tiff.BitsPerSample"))
                {
                    setXmpTagStringSeq(key.toLatin1().constData(), QStringList() << val);
                }
                else
                {
                    setXmpTagString(key.toLatin1().constData(), val);
                }
            }

            ret = true;
        }
        else
        {
            qCWarning(DIGIKAM_METAENGINE_LOG) << "Cannot parse metadata from ["
                                              << filePath
                                              << "] with ImageMagick identify";
            ret = false;
        }

        // Clear memory

        DestroyImageInfo(image_info);

        for (int i = 0 ; i < msize ; ++i)
        {
            if (metadata[i])
            {
                // Note: IM use malloc(), not new operator. Do not use delete operator here.

                free (metadata[i]);
            }
        }

        delete [] metadata;
        delete [] identargv;
    }
    catch (Exception& error_)
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "Cannot read metadata from ["
                                          << filePath
                                          << "] due to ImageMagick exception:"
                                          << error_.what();
        ret = false;
    }

#endif // HAVE_IMAGE_MAGICK

    return ret;
}

} // namespace Digikam
