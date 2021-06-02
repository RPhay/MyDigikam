/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-22
 * Description : PNG DImg plugin.
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

#include "dimgpngplugin.h"

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
#include "dimgpngloader.h"

namespace DigikamPNGDImgPlugin
{

DImgPNGPlugin::DImgPNGPlugin(QObject* const parent)
    : DPluginDImg(parent)
{
}

DImgPNGPlugin::~DImgPNGPlugin()
{
}

QString DImgPNGPlugin::name() const
{
    return i18n("PNG loader");
}

QString DImgPNGPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon DImgPNGPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("image-png"));
}

QString DImgPNGPlugin::description() const
{
    return i18n("An image loader based on Libpng codec");
}

QString DImgPNGPlugin::details() const
{
    return i18n("<p>This plugin allow to load and save image using Libpng codec.</p>"
                "<p>Portable Network Graphics (PNG) is a raster-graphics file-format that supports "
                "lossless data compression. PNG was developed as an improved, non-patented replacement "
                "for Graphics Interchange Format.</p>"
                "<p>See <a href='https://en.wikipedia.org/wiki/Portable_Network_Graphics'>"
                "Portable Network Graphics documentation</a> for details.</p>"
    );
}

QList<DPluginAuthor> DImgPNGPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2005-2021"))
            ;
}

void DImgPNGPlugin::setup(QObject* const /*parent*/)
{
    // Nothing to do
}

QString DImgPNGPlugin::loaderName() const
{
    return QLatin1String("PNG");
}

QString DImgPNGPlugin::typeMimes() const
{
    return QLatin1String("PNG");
}

int DImgPNGPlugin::canRead(const QFileInfo& fileInfo, bool magic) const
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

    uchar pngID[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };

    if (memcmp(&header, &pngID, 8) == 0)
    {
        return 10;
    }

    return 0;
}

int DImgPNGPlugin::canWrite(const QString& format) const
{
    return typeMimes().contains(format.toUpper()) ? 10 : 0;
}

DImgLoader* DImgPNGPlugin::loader(DImg* const image, const DRawDecoding&) const
{
    return new DImgPNGLoader(image);
}

} // namespace DigikamPNGDImgPlugin
