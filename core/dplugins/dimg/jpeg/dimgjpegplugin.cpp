/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-21
 * Description : JPEG DImg plugin.
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

#include "dimgjpegplugin.h"

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
#include "dimgjpegloader.h"

namespace DigikamJPEGDImgPlugin
{

DImgJPEGPlugin::DImgJPEGPlugin(QObject* const parent)
    : DPluginDImg(parent)
{
}

DImgJPEGPlugin::~DImgJPEGPlugin()
{
}

QString DImgJPEGPlugin::name() const
{
    return i18n("JPEG loader");
}

QString DImgJPEGPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon DImgJPEGPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("image-jpeg"));
}

QString DImgJPEGPlugin::description() const
{
    return i18n("An image loader based on Libjpeg codec");
}

QString DImgJPEGPlugin::details() const
{
    return i18n("<p>This plugin permit to load and save image using Libjpeg codec.</p>"
                "<p>Joint Photographic Experts Group (JPEG) is a commonly used method of lossy "
                "compression for digital images, particularly for those images produced by "
                "digital photography. The degree of compression can be adjusted, allowing "
                "a selectable tradeoff between storage size and image quality.</p>"
                "<p>See <a href='https://en.wikipedia.org/wiki/JPEG'>"
                "Joint Photographic Experts Group documentation</a> for details.</p>"
    );
}

QList<DPluginAuthor> DImgJPEGPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Renchi Raju"),
                             QString::fromUtf8("renchi dot raju at gmail dot com"),
                             QString::fromUtf8("(C) 2005"))
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2006-2021"))
            ;
}

void DImgJPEGPlugin::setup(QObject* const /*parent*/)
{
    // Nothing to do
}

QString DImgJPEGPlugin::loaderName() const
{
    return QLatin1String("JPEG");
}

QString DImgJPEGPlugin::typeMimes() const
{
    return QLatin1String("JPG JPEG JPE");
}

int DImgJPEGPlugin::canRead(const QFileInfo& fileInfo, bool magic) const
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

    const int headerLen = 9;

    unsigned char header[headerLen];

    if (fread(&header, headerLen, 1, f) != 1)
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "Failed to read header of file " << filePath;
        fclose(f);
        return 0;
    }

    fclose(f);

    uchar jpegID[2] = { 0xFF, 0xD8 };

    if (memcmp(&header, &jpegID, 2) == 0)
    {
        return 10;
    }

    return 0;
}

int DImgJPEGPlugin::canWrite(const QString& format) const
{
    return typeMimes().contains(format.toUpper()) ? 10 : 0;
}

DImgLoader* DImgJPEGPlugin::loader(DImg* const image, const DRawDecoding&) const
{
    return new DImgJPEGLoader(image);
}

} // namespace DigikamJPEGDImgPlugin
