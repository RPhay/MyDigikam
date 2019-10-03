/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-22
 * Description : HEIF DImg plugin.
 *
 * Copyright (C) 2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "dimgheifplugin.h"

// C++ includes

#include <cstdio>

// Qt includes

#include <QFile>
#include <QFileInfo>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_config.h"
#include "digikam_debug.h"
#include "digikam_globals.h"
#include "dimgheifloader.h"

namespace DigikamHEIFDImgPlugin
{

DImgHEIFPlugin::DImgHEIFPlugin(QObject* const parent)
    : DPluginDImg(parent)
{
}

DImgHEIFPlugin::~DImgHEIFPlugin()
{
}

QString DImgHEIFPlugin::name() const
{
    return i18n("HEIF loader");
}

QString DImgHEIFPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon DImgHEIFPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("image-x-generic"));
}

QString DImgHEIFPlugin::description() const
{
    return i18n("An image loader based on Libheif codec");
}

QString DImgHEIFPlugin::details() const
{
    QString x265Notice = i18n("This library is not present on your system.");

#ifdef HAVE_X265
    int depth = DImgHEIFLoader::x265MaxBitsDepth();

    if (depth != -1)
    {
        x265Notice = i18n("This library is available on your system with a maximum color depth "
                          "support of %1 bits.", depth);
    }
    else
    {
        x265Notice = i18n("This library is available on your system but is not able to encode "
                          "image with a suitable color depth.");
    }
#endif

    return i18n("<p>This plugin permit to load and save image using Libheif codec.</p>"
                "<p>High Efficiency Image File Format (HEIF), also known as High Efficiency Image Coding (HEIC), "
                "is a file format for individual images and image sequences. It was developed by the "
                "Moving Picture Experts Group (MPEG) and it claims that twice as much information can be "
                "stored in a HEIF image as in a JPEG image of the same size, resulting in a better quality image. "
                "HEIF also supports animation, and is capable of storing more information than an animated GIF "
                "at a small fraction of the size.</p>"
                "<p>Encoding HEIC is relevant of optional libx265 codec. %1</p>"
                "<p>See <a href='https://en.wikipedia.org/wiki/High_Efficiency_Image_File_Format'>"
                "High Efficiency Image File Format</a> for details.</p>", x265Notice);
}

QList<DPluginAuthor> DImgHEIFPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2019"))
            ;
}

void DImgHEIFPlugin::setup(QObject* const /*parent*/)
{
    // Nothing to do
}

QString DImgHEIFPlugin::loaderName() const
{
    return QLatin1String("HEIF");
}

QString DImgHEIFPlugin::typeMimes() const
{
    return QLatin1String("HEIC");
}

bool DImgHEIFPlugin::canRead(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists())
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "File " << filePath << " does not exist";
        return false;
    }

    // First simply check file extension

    QString ext = fileInfo.suffix().toUpper();

    if (!ext.isEmpty() && (ext == QLatin1String("HEIC")))
    {
        return true;
    }

    // In second, we trying to parse file header.

    FILE* const f = fopen(QFile::encodeName(filePath).constData(), "rb");

    if (!f)
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "Failed to open file " << filePath;
        return false;
    }

    const int headerLen = 12;

    unsigned char header[headerLen];

    if (fread(&header, headerLen, 1, f) != 1)
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "Failed to read header of file " << filePath;
        fclose(f);
        return false;
    }

    fclose(f);

    if ((memcmp(&header[4], "ftyp", 4) == 0) ||
        (memcmp(&header[8], "heic", 4) == 0) ||
        (memcmp(&header[8], "heix", 4) == 0) ||
        (memcmp(&header[8], "mif1", 4) == 0))
    {
        return true;
    }

    return false;
}

bool DImgHEIFPlugin::canWrite(const QString& format) const
{
#ifdef HAVE_X265
    if (format.toUpper() == QLatin1String("HEIC"))
    {
        return true;
    }
#endif
    return false;
}

DImgLoader* DImgHEIFPlugin::loader(DImg* const image, const DRawDecoding&) const
{
    return new DImgHEIFLoader(image);
}

} // namespace DigikamHEIFDImgPlugin
