/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-09-24
 * Description : identify properties from image with ImageMagick.
 *
 * Copyright (C) 2020-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes

#include <QString>
#include <QStringList>
#include <QDebug>
#include <QCoreApplication>

// Local includes

#include "metaengine.h"

// ImageMagick includes

#include <Magick++.h>

using namespace Magick;
using namespace MagickCore;
using namespace Digikam;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    if (argc != 2)
    {
        qDebug() << "magickidentify - load image metadata with Image Magick";
        qDebug() << "Usage: <image>";
        return -1;
    }

    MagickCoreGenesis((char*)nullptr, MagickFalse);

    const int msize                   = 20;
    MagickCore::ImageInfo* image_info = nullptr;
    ExceptionInfo ex                  = *AcquireExceptionInfo();

    // Syntax description of IM percent escape formats: https://imagemagick.org/script/escape.php

    const QString filters             = QLatin1String(
                                            "Exif.Image.ImageLength=%[height]\n"
                                            "Exif.Image.ImageWidth=%[width]\n"
                                            "Exif.Image.Compression=%[compression]\n"
                                            "Exif.Image.BitsPerSample=%[bit-depth]\n"
                                            "Exif.Image.ImageDescription=%[caption]\n"
                                            "Exif.Photo.ColorSpace=%[colorspace]\n"
                                            "Exif.Image.CameraLabel=%[label]\n"
                                            "Exif.Image.Orientation=%[orientation]\n"
                                            "Exif.Photo.UserComment=%[comment]\n"
                                            "Exif.Image.DateTime=%[date:create]\n"
                                            "Exif.Image.DateTimeOriginal=%[date:create]\n"
                                            "%[*]\n"    // Extra properties from IM/
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
        strcpy(image_info->filename, argv[1]);
        image_info->ping    = MagickTrue;
        image_info->verbose = MagickTrue;
        image_info->debug   = MagickTrue;

        // Insert percent escape format for IM identification

        int identargc       = argc + 2;
        char** identargv    = new char*[identargc];
        identargv[0]        = argv[0];
        identargv[1]        = (char*)"-format";
        identargv[2]        = filters.toLatin1().data();
        identargv[3]        = argv[1];

        // Call IM core identification

        IdentifyImageCommand(image_info, identargc, identargv, metadata, &ex);

        // Post process metadata

        QString                 output;
        MetaEngine::MetaDataMap tagsMap;

        for (int i = 0 ; i < msize ; ++i)
        {
            if (metadata[i][0] != '\0')
            {
                output.append(QString::fromLatin1(metadata[i]));
            }
        }

        QStringList lines = output.split(QLatin1Char('\n'));
        QString key;
        QString val;

        foreach (const QString& tupple, lines)
        {
            key = tupple.section(QLatin1Char('='), 0, 0);
            val = tupple.section(QLatin1Char('='), 1, 1);

            if (!key.startsWith(QLatin1String("Exif")))
            {
                key = QLatin1String("Xmp.IMProperties.") + key.remove(QLatin1Char(':'));
            }

            qDebug() << key << " :: " << val;
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
                   << argv[1]
                   << "] due to ImageMagick exception:"
                   << error_.what();
    }

    MagickCoreTerminus();

    return 0;
}
