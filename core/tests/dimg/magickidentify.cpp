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

    MagickCore::Image* image          = nullptr;
    MagickCore::ImageInfo* image_info = nullptr;
    ExceptionInfo ex                  = *AcquireExceptionInfo();

    QApplication app(argc, argv);

    try
    {
        image_info          = CloneImageInfo((ImageInfo*)nullptr);
        strcpy(image_info->filename, argv[1]);
        image_info->ping    = MagickTrue;
        image_info->verbose = MagickFalse;
        image               = PingImages(image_info, argv[1], &ex);
        DestroyImageInfo(image_info);
        DestroyImage(image);
    }
    catch (Exception& error_)
    {
        qWarning() << "Cannot read [" << argv[1] << "] due to ImageMagick exception:" << error_.what();
    }

    MagickCoreTerminus();

    return 0;
}
