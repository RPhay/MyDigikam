/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-22
 * Description : HEIF DImg plugin.
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

#include "dimgheifplugin.h"

// C++ includes

#include <cstdio>

// Qt includes

#include <QFile>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"
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
                "<p>Encoding HEIC is relevant of optional libde265 codec. %1</p>"
                "<p>See <a href='https://en.wikipedia.org/wiki/High_Efficiency_Image_File_Format'>"
                "High Efficiency Image File Format</a> for details.</p>", x265Notice);
}

QList<DPluginAuthor> DImgHEIFPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2019-2021"))
            ;
}

void DImgHEIFPlugin::setup(QObject* const /*parent*/)
{
    // Nothing to do
}

QMap<QString, QString> DImgHEIFPlugin::extraAboutData() const
{
    QMap<QString, QString> map;
    map.insert(QLatin1String("HEIC"), i18n("High efficiency image coding"));
    map.insert(QLatin1String("HEIF"), i18n("High efficiency image file format"));

    return map;
}

bool DImgHEIFPlugin::previewSupported() const
{
    return true;
}

QString DImgHEIFPlugin::loaderName() const
{
    return QLatin1String("HEIF");
}

QString DImgHEIFPlugin::typeMimes() const
{
    return QLatin1String("HEIC HEIF");
}

int DImgHEIFPlugin::canRead(const QFileInfo& fileInfo, bool magic) const
{
    QString filePath = fileInfo.filePath();
    QString format   = fileInfo.suffix().toUpper();

    // First simply check file extension

    if (!magic)
    {
        return typeMimes().contains(format) ? 10 : 0;
    }

    // In second, we trying to parse file header.

#ifdef Q_OS_WIN

    FILE* const f = _wfopen((const wchar_t*)filePath.utf16(), L"rb");

#else

    FILE* const f = fopen(filePath.toUtf8().constData(), "rb");

#endif

    if (!f)
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "Failed to open file " << filePath;
        return 0;
    }

    const int headerLen = 12;

    unsigned char header[headerLen];

    if (fread(&header, headerLen, 1, f) != 1)
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "Failed to read header of file " << filePath;
        fclose(f);
        return 0;
    }

    fclose(f);

    if (
        (memcmp(&header[4], "ftypheic", 8) == 0) ||
        (memcmp(&header[4], "ftypheix", 8) == 0) ||
        (memcmp(&header[4], "ftypmif1", 8) == 0)
       )
    {
        return 10;
    }

    return 0;
}

int DImgHEIFPlugin::canWrite(const QString& format) const
{

#ifdef HAVE_X265

    return typeMimes().contains(format.toUpper()) ? 10 : 0;

#else

    Q_UNUSED(format);

    return 0;

#endif
}

DImgLoader* DImgHEIFPlugin::loader(DImg* const image, const DRawDecoding&) const
{
    return new DImgHEIFLoader(image);
}

} // namespace DigikamHEIFDImgPlugin
