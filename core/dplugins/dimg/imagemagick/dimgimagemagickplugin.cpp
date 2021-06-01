/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-21
 * Description : ImageMagick DImg plugin.
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

#include "dimgimagemagickplugin.h"

// Qt includes

#include <QMimeDatabase>

// KDE includes

#include <klocalizedstring.h>

// Image Magick includes

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wkeyword-macro"
#endif

#include <Magick++.h>

#if MagickLibVersion < 0x700
#   include <magick/magick.h>
#endif

using namespace Magick;
using namespace MagickCore;

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic pop
#endif

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "dimgimagemagickloader.h"

namespace DigikamImageMagickDImgPlugin
{

DImgImageMagickPlugin::DImgImageMagickPlugin(QObject* const parent)
    : DPluginDImg(parent)
{
    MagickCoreGenesis((char*)nullptr, MagickFalse);

    m_readFormats  = decoderFormats();
    m_writeFormats = encoderFormats();
}

DImgImageMagickPlugin::~DImgImageMagickPlugin()
{
}

void DImgImageMagickPlugin::cleanUp()
{
    MagickCoreTerminus();
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
    return i18n("<p>This plugin allow to load and save image using ImageMagick coders.</p>"
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
                             QString::fromUtf8("(C) 2006-2021"))
            ;
}

void DImgImageMagickPlugin::setup(QObject* const /*parent*/)
{
    // Nothing to do
}

QMap<QString, QString> DImgImageMagickPlugin::extraAboutData() const
{
    QString mimes = typeMimes();
    QMap<QString, QString> map;

    try
    {
        ExceptionInfo ex          = *AcquireExceptionInfo();
        size_t n                  = 0;
        const MagickInfo** inflst = GetMagickInfoList("*", &n, &ex);

        if (!inflst)
        {
            qCWarning(DIGIKAM_DIMG_LOG_MAGICK) << "ImageMagick coders list is null!";
            return QMap<QString, QString>();
        }

        for (uint i = 0 ; i < n ; ++i)
        {
            const MagickInfo* inf = inflst[i];

            if (inf)
            {
                QString mod =

#if (MagickLibVersion >= 0x69A && defined(magick_module))

                    QString::fromLatin1(inf->magick_module).toUpper();

#else

                    QString::fromLatin1(inf->module).toUpper();

#endif

                if (mimes.contains(mod))
                {
                    map.insert(mod, QLatin1String(inf->description));
                }
            }
        }

        free(inflst);
    }
    catch (Exception& error)
    {
        qCWarning(DIGIKAM_DIMG_LOG) << "ImageMagickInfo exception:" << error.what();
        return QMap<QString, QString>();
    }

    return map;
}

QString DImgImageMagickPlugin::loaderName() const
{
    return QLatin1String("IMAGEMAGICK");
}

QString DImgImageMagickPlugin::typeMimes() const
{
    QStringList formats = m_readFormats;
    formats.sort();

    QString ret;

    foreach (const QString& str, formats)
    {
        if (!ret.contains(str))
        {
            ret += QString::fromUtf8("%1 ").arg(str.toUpper());
        }
    }

    return ret;
}

int DImgImageMagickPlugin::canRead(const QFileInfo& fileInfo, bool magic) const
{
    QString filePath = fileInfo.filePath();
    QString format   = fileInfo.suffix().toUpper();

    if (!magic)
    {
        QString mimeType(QMimeDatabase().mimeTypeForFile(filePath).name());

        // Ignore non image format.

        if (
            mimeType.startsWith(QLatin1String("video/")) ||
            mimeType.startsWith(QLatin1String("audio/"))
           )
        {
            return 0;
        }

        if (m_readFormats.contains(format))
        {
            if (format == QLatin1String("WEBP"))
            {
                return 70;
            }
            else
            {
                return 90;
            }
        }
    }

    return 0;
}

int DImgImageMagickPlugin::canWrite(const QString& format) const
{
    if (m_writeFormats.contains(format.toUpper()))
    {
        if (format.toUpper() == QLatin1String("WEBP"))
        {
            return 70;
        }
        else
        {
            return 90;
        }
    }

    return 0;
}

DImgLoader* DImgImageMagickPlugin::loader(DImg* const image, const DRawDecoding&) const
{
    return new DImgImageMagickLoader(image);
}

QStringList DImgImageMagickPlugin::decoderFormats() const
{
    QStringList formats;

    try
    {
        ExceptionInfo ex          = *AcquireExceptionInfo();
        size_t n                  = 0;
        const MagickInfo** inflst = GetMagickInfoList("*", &n, &ex);

        if (!inflst)
        {
            qWarning() << "ImageMagick coders list is null!";
            return QStringList();
        }

        for (uint i = 0 ; i < n ; ++i)
        {
            const MagickInfo* inf = inflst[i];

            if (inf && inf->decoder)
            {

#if (MagickLibVersion >= 0x69A && defined(magick_module))

            formats.append(QString::fromLatin1(inf->magick_module).toUpper());

#else

            formats.append(QString::fromLatin1(inf->module).toUpper());

#endif

            }
        }

        free(inflst);
    }
    catch (Exception& error)
    {
        qCWarning(DIGIKAM_DIMG_LOG) << "ImageMagickInfo exception:" << error.what();
        return QStringList();
    }

    if (formats.contains(QLatin1String("JPEG")))
    {
        formats.append(QLatin1String("JPG"));
        formats.append(QLatin1String("JPE"));
    }

    if (formats.contains(QLatin1String("FITS")))
    {
        formats.append(QLatin1String("FTS"));
        formats.append(QLatin1String("FIT"));
    }

    // Remove known formats that are not stable.

    formats.removeAll(QLatin1String("XCF"));

    return formats;
}

QStringList DImgImageMagickPlugin::encoderFormats() const
{
    QStringList formats;

    try
    {
        ExceptionInfo ex          = *AcquireExceptionInfo();
        size_t n                  = 0;
        const MagickInfo** inflst = GetMagickInfoList("*", &n, &ex);

        if (!inflst)
        {
            qWarning() << "ImageMagick coders list is null!";
            return QStringList();
        }

        for (uint i = 0 ; i < n ; ++i)
        {
            const MagickInfo* inf = inflst[i];

            if (inf && inf->encoder)
            {

#if (MagickLibVersion >= 0x69A && defined(magick_module))

                formats.append(QString::fromLatin1(inf->magick_module).toUpper());

#else

                formats.append(QString::fromLatin1(inf->module).toUpper());

#endif

            }
        }

        free(inflst);
    }
    catch (Exception& error)
    {
        qCWarning(DIGIKAM_DIMG_LOG) << "ImageMagickInfo exception:" << error.what();
        return QStringList();
    }

    if (formats.contains(QLatin1String("FITS")))
    {
        formats.append(QLatin1String("FTS"));
    }

    return formats;
}

} // namespace DigikamImageMagickDImgPlugin
