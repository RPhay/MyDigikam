/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-22
 * Description : TIFF DImg plugin.
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

#include "dimgtiffplugin.h"

// C++ includes

#include <cstdio>

// Qt includes

#include <QFile>
#include <QFileInfo>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "dimgtiffloader.h"

namespace DigikamTIFFDImgPlugin
{

DImgTIFFPlugin::DImgTIFFPlugin(QObject* const parent)
    : DPluginDImg(parent)
{
}

DImgTIFFPlugin::~DImgTIFFPlugin()
{
}

QString DImgTIFFPlugin::name() const
{
    return i18n("TIFF loader");
}

QString DImgTIFFPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon DImgTIFFPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("image-tiff"));
}

QString DImgTIFFPlugin::description() const
{
    return i18n("An image loader based on Libtiff codec");
}

QString DImgTIFFPlugin::details() const
{
    return i18n("<p>This plugin permit to load and save image using Libtiff codec.</p>"
                "<p>Tagged Image File Format, abbreviated TIFF or TIF, is a computer file format "
                "for storing raster graphics images, popular among graphic artists, the publishing "
                "industry, and photographers. TIFF is widely supported by scanning, faxing, "
                "word processing, optical character recognition, image manipulation, "
                "desktop publishing, and page-layout applications.</p>"
                "<p>See <a href='https://en.wikipedia.org/wiki/TIFF'>"
                "Tagged Image File Format documentation</a> for details.</p>"
    );
}

QList<DPluginAuthor> DImgTIFFPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Renchi Raju"),
                             QString::fromUtf8("renchi dot raju at gmail dot com"),
                             QString::fromUtf8("(C) 2005"))
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2006-2019"))
            ;
}

void DImgTIFFPlugin::setup(QObject* const /*parent*/)
{
    // Nothing to do
}

QString DImgTIFFPlugin::loaderName() const
{
    return QLatin1String("TIFF");
}

QString DImgTIFFPlugin::typeMimes() const
{
    return QLatin1String("TIF TIFF");
}

bool DImgTIFFPlugin::canRead(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists())
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "File " << filePath << " does not exist";
        return false;
    }

    // First simply check file extension

    QString ext = fileInfo.suffix().toUpper();

    if (!ext.isEmpty() && (ext == QLatin1String("TIFF") || ext == QLatin1String("TIF")))
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

    const int headerLen = 9;

    unsigned char header[headerLen];

    if (fread(&header, headerLen, 1, f) != 1)
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "Failed to read header of file " << filePath;
        fclose(f);
        return false;
    }

    fclose(f);

    uchar tiffBigID[2] = { 0x4D, 0x4D };
    uchar tiffLilID[2] = { 0x49, 0x49 };

    if (memcmp(&header, &tiffBigID, 2) == 0 ||
        memcmp(&header, &tiffLilID, 2) == 0)
    {
        return true;
    }

    return false;
}

bool DImgTIFFPlugin::canWrite(const QString& format) const
{
    if ((format == QLatin1String("TIFF") || format == QLatin1String("TIF")))
    {
        return true;
    }

    return false;
}

DImgLoader* DImgTIFFPlugin::loader(DImg* const image, const DRawDecoding&) const
{
    return new DImgTIFFLoader(image);
}

} // namespace DigikamTIFFDImgPlugin
