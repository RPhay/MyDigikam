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

// Local includes

#include "digikam_config.h"
#include "digikam_debug.h"

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

#ifdef HAVE_IMAGE_MAGICK

    const int msize                   = 20;
    MagickCore::ImageInfo* image_info = nullptr;
    ExceptionInfo ex                  = *AcquireExceptionInfo();

    // Syntax description of IM percent escape formats: https://imagemagick.org/script/escape.php

    const QString filters             = QLatin1String(
                                            "Xmp.tiff.ImageLength=%[height]\n"                      // Text
                                            "Xmp.tiff.ImageWidth=%[width]\n"                        // Text
                                            "Xmp.tiff.Compression=%[compression]\n"                 // Text
                                            "Xmp.tiff.BitsPerSample=%[bit-depth]\n"                 // Seq
                                            "Xmp.tiff.ImageDescription=%[caption]\n"                // LangAlt
                                            "Xmp.xmp.Label=%[label]\n"                              // Text
                                            "Xmp.tiff.Orientation=%[orientation]\n"                 // Text
                                            "Xmp.tiff.DateTime=%[date:create]\n"                    // Text
                                            "Xmp.tiff.XResolution=%[resolution.x]\n"                // Text
                                            "Xmp.tiff.YResolution=%[resolution.y]\n"                // Text
                                            "Xmp.tiff.ResolutionUnit=%[units]\n"                    // Text
                                            "Xmp.exif.UserComment=%[comment]\n"                     // LangAlt
                                            "Xmp.exif.ColorSpace=%[colorspace]\n"                   // Text
                                            "Xmp.exif.DateTimeOriginal=%[date:create]\n"            // Text
                                            "Xmp.IMProperties.IMVersion=%[version]\n"               // Text
                                            "Xmp.IMProperties.IMCopyright=%[copyright]\n"           // Text
                                            "Xmp.IMProperties.IMBaseName=%[basename]\n"             // Text
                                            "Xmp.IMProperties.IMExtension=%[extension]\n"           // Text
                                            "Xmp.IMProperties.IMCodec=%[magick]\n"                  // Text
                                            "Xmp.IMProperties.IMChannels=%[channels]\n"             // Text
                                            "Xmp.IMProperties.IMInterlace=%[interlace]\n"           // Text
                                            "Xmp.IMProperties.IMProfiles=%[profiles]\n"             // Text
                                            "Xmp.IMProperties.IMQuality=%[quality]\n"               // Text
                                            "Xmp.IMProperties.IMRendering=%[rendering-intent]\n"    // Text
                                            "Xmp.IMProperties.IMScene=%[scene]\n"                   // Text
                                            "%[*]\n"    // Extra properties from IM                 // Text

// Values calculated (not specified in IM doc) and take a while
//                                            "Xmp.IMProperties.IMColors=%[colors]\n"
//                                            "Xmp.IMProperties.IMGamma=%[gamma]\n"

                                        );

    try
    {
        // Allocate metadata container for IM.

        char** metadata     = new char*[msize];

        for (int i = 0 ; i < msize; ++i)
        {
            // NOTE: prepare metadata items with empty strings.

            metadata[i]     = (char*)malloc(sizeof(char));
            metadata[i][0]  = '\0';
        }

        // Prepare image info for IM isentification

        image_info          = CloneImageInfo((ImageInfo*)nullptr);
        strcpy(image_info->filename, filePath.toLatin1().constData());
        image_info->ping    = MagickTrue;
        image_info->verbose = MagickTrue;
        image_info->debug   = MagickTrue;

        // Insert percent escape format for IM identification

        int identargc       = 4;
        char** identargv    = new char*[identargc];
        identargv[0]        = (char*)"dmetadata";
        identargv[1]        = (char*)"-format";
        identargv[2]        = filters.toLatin1().data();
        identargv[3]        = filePath.toLatin1().data();

        // Call IM core identification

        IdentifyImageCommand(image_info, identargc, identargv, metadata, &ex);

        // Post process metadata

        registerXmpNameSpace(QLatin1String("https://imagemagick.org/"), QLatin1String("IMProperties"));

        QString                 output;
        MetaEngine::MetaDataMap tagsMap;

        for (int i = 0 ; i < msize ; ++i)
        {
            if (metadata[i][0] != '\0')
            {
                output.append(QString::fromUtf8(metadata[i]));
            }
        }

        QStringList lines = output.split(QLatin1Char('\n'));
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

            if (!key.startsWith(QLatin1String("Xmp")))
            {
                // Create a dedicated XMP namespace to store extra IM properties.

                key = QLatin1String("Xmp.IMProperties.") + key.remove(QLatin1Char(':'));
            }

            key = key.remove(QLatin1Char('-'));
            key = key.remove(QLatin1Char('_'));
            val = val.simplified();

            // Register XMP tags in container

            if      (
                     key == QLatin1String("Xmp.tiff.ImageDescription") ||
                     key == QLatin1String("Xmp.exif.UserComment")
                    )
            {
                setXmpTagStringLangAlt(key.toLatin1().constData(), val, QString());
            }
            else if (
                     key == QLatin1String("Xmp.tiff.BitsPerSample")
                    )
            {
                setXmpTagStringSeq(key.toLatin1().constData(), QStringList() << val);
            }
            else
            {
                setXmpTagString(key.toLatin1().constData(), val);
            }
        }

        // Clear memory

        DestroyImageInfo(image_info);

        for (int i = 0 ; i < msize ; ++i)
        {
            // NOTE: IM use realloc to change memory allocation.

            free (metadata[i]);
        }

        delete [] metadata;
        delete [] identargv;
    }
    catch (Exception& error_)
    {
        qWarning() << "Cannot read metadata from ["
                   << filePath
                   << "] due to ImageMagick exception:"
                   << error_.what();
    }


#endif // HAVE_IMAGE_MAGICK

    return false;
}

} // namespace Digikam
