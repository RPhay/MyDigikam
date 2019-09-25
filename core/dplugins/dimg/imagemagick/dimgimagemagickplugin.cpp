/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-21
 * Description : ImageMagick DImg plugin.
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

#include "dimgimagemagickplugin.h"

#include <Magick++.h>
using namespace Magick;

#if MagickLibVersion < 0x700
#   include <magick/magick.h>
#endif

// Qt includes

#include <QFileInfo>
#include <QImageReader>
#include <QImageWriter>
#include <QMimeDatabase>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "dimgimagemagickloader.h"
#include "drawdecoder.h"

namespace DigikamImageMagickDImgPlugin
{

DImgImageMagickPlugin::DImgImageMagickPlugin(QObject* const parent)
    : DPluginDImg(parent)
{
}

DImgImageMagickPlugin::~DImgImageMagickPlugin()
{
}

QString DImgImageMagickPlugin::name() const
{
    return i18n("ImageMagick loader");
}

QString DImgImageMagickPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon DImgImageMagickPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("image-x-generic"));
}

QString DImgImageMagickPlugin::description() const
{
    return i18n("An image loader based on ImageMagick coders");
}

QString DImgImageMagickPlugin::details() const
{
    return i18n("<p>This plugin permit to load and save image using ImageMagick coders.</p>"
                "<p>ImageMagick is a free and open-source software suite for converting raster image and vector image files. "
                "It can read and write over 200 image file formats.</p>"
                "<p>See <a href='https://en.wikipedia.org/wiki/ImageMagick'>ImageMagick documentation</a> for details.</p>"
    );
}

QList<DPluginAuthor> DImgImageMagickPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Maik Qualmann"),
                             QString::fromUtf8("metzpinguin at gmail dot com"),
                             QString::fromUtf8("(C) 2019"))
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2006-2019"))
            ;
}

void DImgImageMagickPlugin::setup(QObject* const /*parent*/)
{
    // Nothing to do
}

QString DImgImageMagickPlugin::loaderName() const
{
    return QLatin1String("IMAGEMAGICK");
}

QString DImgImageMagickPlugin::typeMimes() const
{
    QStringList formats;
    MagickCore::ExceptionInfo ex;
    size_t n                              = 0;
    const MagickCore::MagickInfo** inflst = MagickCore::GetMagickInfoList("*", &n, &ex);

    for (uint i = 0 ; i < n ; ++i)
    {
        const MagickCore::MagickInfo* inf = inflst[i];

        if (inf->decoder)
        {
#if (MagickLibVersion >= 0x69A && defined(magick_module))
            formats.append(QString::fromLatin1(inf->magick_module).toUpper());
#else
            formats.append(QString::fromLatin1(inf->module).toUpper());
#endif
        }
    }

    qDebug() << "ImageMagick support this formats:" << formats;

    formats.removeAll(QLatin1String("JPEG"));  // JPEG file format
    formats.removeAll(QLatin1String("JPG"));   // JPEG file format
    formats.removeAll(QLatin1String("JPE"));   // JPEG file format
    formats.removeAll(QLatin1String("PNG"));
    formats.removeAll(QLatin1String("TIFF"));
    formats.removeAll(QLatin1String("TIF"));
    formats.removeAll(QLatin1String("PGF"));
    formats.removeAll(QLatin1String("JP2"));   // JPEG2000 file format
    formats.removeAll(QLatin1String("JPX"));   // JPEG2000 file format
    formats.removeAll(QLatin1String("JPC"));   // JPEG2000 code stream
    formats.removeAll(QLatin1String("J2K"));   // JPEG2000 code stream
    formats.removeAll(QLatin1String("PGX"));   // JPEG2000 WM format

    QString rawFilesExt = QString::fromLatin1(DRawDecoder::rawFiles()).remove(QLatin1String("*.")).toUpper();

    foreach (const QString& str, rawFilesExt.split(QLatin1Char(' ')))
    {
        formats.removeAll(str);                // All Raw image formats
    }

    QString ret;

    foreach (const QString& str, formats)
    {
        ret += QString::fromUtf8("%1 ").arg(str.toUpper());
    }

    return ret;
}

bool DImgImageMagickPlugin::canRead(const QString& filePath) const
{
    QString mimeType(QMimeDatabase().mimeTypeForFile(filePath).name());

    // Ignore non image format.

    if (
        mimeType.startsWith(QLatin1String("video/")) ||
        mimeType.startsWith(QLatin1String("audio/"))
       )
    {
        return false;
    }

    // Ignore native loaders.

    if (
        mimeType.contains(QLatin1String("image/jpeg"))  ||
        mimeType.contains(QLatin1String("image/jpg"))   ||
        mimeType.contains(QLatin1String("image/jpe"))   ||
        mimeType.contains(QLatin1String("image/png"))   ||
        mimeType.contains(QLatin1String("image/tif"))   ||
        mimeType.contains(QLatin1String("image/tiff"))  ||
        mimeType.contains(QLatin1String("image/x-pgf")) ||
        mimeType.contains(QLatin1String("image/jp2"))   ||
        mimeType.contains(QLatin1String("image/jpx"))   ||
        mimeType.contains(QLatin1String("image/jpc"))   ||
        mimeType.contains(QLatin1String("image/j2k"))   ||
        mimeType.contains(QLatin1String("image/pgx"))
       )
    {
        return false;
    }

    // Ignore RAW files

    QString ext         = QFileInfo(filePath).suffix().toUpper();
    QString rawFilesExt = QString::fromLatin1(DRawDecoder::rawFiles()).remove(QLatin1String("*.")).toUpper();

    if (rawFilesExt.toUpper().contains(ext))
    {
        return false;
    }

    QStringList formats;
    MagickCore::ExceptionInfo ex;
    size_t n                              = 0;
    const MagickCore::MagickInfo** inflst = MagickCore::GetMagickInfoList("*", &n, &ex);

    for (uint i = 0 ; i < n ; ++i)
    {
        const MagickCore::MagickInfo* inf = inflst[i];

        if (inf->decoder)
        {
#if (MagickLibVersion >= 0x69A && defined(magick_module))
            formats.append(QString::fromLatin1(inf->magick_module).toUpper());
#else
            formats.append(QString::fromLatin1(inf->module).toUpper());
#endif
        }
    }

    if (!formats.contains(ext))
    {
        return false;
    }

    return true;
}

bool DImgImageMagickPlugin::canWrite(const QString& format) const
{
    QString blackList = QString::fromLatin1(DRawDecoder::rawFiles()).remove(QLatin1String("*.")).toUpper(); // Ignore RAW files
    blackList.append(QLatin1String(" JPEG JPG JPE PNG TIF TIFF PGF JP2 JPX JPC J2K PGX "));                 // Ignore native loaders

    if (blackList.toUpper().contains(format))
    {
        return false;
    }

    // NOTE: Native loaders support are previously black-listed.
    // For ex, if tiff is supported in write mode by ImageMagick it will never be handled.

    QStringList formats;
    MagickCore::ExceptionInfo ex;
    size_t n                              = 0;
    const MagickCore::MagickInfo** inflst = MagickCore::GetMagickInfoList("*", &n, &ex);

    for (uint i = 0 ; i < n ; ++i)
    {
        const MagickCore::MagickInfo* inf = inflst[i];

        if (inf->encoder)
        {
#if (MagickLibVersion >= 0x69A && defined(magick_module))
            formats.append(QString::fromLatin1(inf->magick_module).toUpper());
#else
            formats.append(QString::fromLatin1(inf->module).toUpper());
#endif
        }
    }

    if (!formats.contains(format))
    {
        return false;
    }

    return true;
}

DImgLoader* DImgImageMagickPlugin::loader(DImg* const image, const DRawDecoding&) const
{
    return new DImgImageMagickLoader(image);
}

} // namespace DigikamImageMagickDImgPlugin
