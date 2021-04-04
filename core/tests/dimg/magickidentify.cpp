/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-09-24
 * Description : Test ImageMagick loader to QImage.
 *
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <QDebug>
#include <QApplication>

// ImageMagick includes

#include <Magick++.h>

using namespace Magick;
using namespace MagickCore;

int main(int argc, char** argv)
{
    MagickCoreGenesis((char*)nullptr, MagickFalse);

    MagickCore::ImageInfo* image_info = nullptr;
    ExceptionInfo ex                  = *AcquireExceptionInfo();

    QApplication app(argc, argv);

    try
    {
        char** metadata                   = (char**)malloc(256 * sizeof(char*));

        for (int i = 0 ; i < 256; ++i)
        {
            metadata[i]    = (char*)malloc(256 * sizeof(char));
            metadata[i][0] = '\0';
        }

        image_info          = CloneImageInfo((ImageInfo*)nullptr);
        strcpy(image_info->filename, argv[1]);
        image_info->ping    = MagickTrue;
        image_info->verbose = MagickTrue;
        image_info->debug   = MagickTrue;
        IdentifyImageCommand(image_info, argc, argv, metadata, &ex);

        for (int i = 0 ; i < 256; ++i)
        {
            printf("%i: %s\n", i, metadata[i]);
        }

        DestroyImageInfo(image_info);

        for (int i = 0 ; i < 256; ++i)
        {
            free(metadata[i]);
        }

        free (metadata);
    }
    catch (Exception& error_)
    {
        qWarning() << "Cannot read [" << argv[1] << "] due to ImageMagick exception:" << error_.what();
    }

    MagickCoreTerminus();

    return 0;
}
