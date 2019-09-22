/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-21
 * Description : JPEG DImg plugin.
 *
 * Copyright (C) 2019      by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <QFileInfo>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
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
    return i18n("JPEG DImg loader");
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
    return i18n("A DImg image loader based on Libjpeg codec");
}

QString DImgJPEGPlugin::details() const
{
    return i18n("<p>This plugin permit to load and save image with DImg using "
                "Libjpeg codec</p>"
                "<p>See <a href='https://en.wikipedia.org/wiki/Libjpeg'>Libjpeg documentation</a> for details.</p>"
    );
}

QList<DPluginAuthor> DImgJPEGPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2019"))
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
    return QLatin1String("jpg jpeg jpe");
}

bool DImgJPEGPlugin::canRead(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists())
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "File " << filePath << " does not exist";
        return false;
    }

    // First simply check file extension

    QString ext = fileInfo.suffix().toUpper();

    if (!ext.isEmpty() && (ext == QLatin1String("JPEG") || ext == QLatin1String("JPG") || ext == QLatin1String("JPE")))
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

    uchar jpegID[2] = { 0xFF, 0xD8 };

    if (memcmp(&header, &jpegID, 2) == 0)
    {
        return true;
    }

    return false;
}

bool DImgJPEGPlugin::canWrite(const QString& format) const
{
    if ((format == QLatin1String("JPEG") || format == QLatin1String("JPG") || format == QLatin1String("JPE")))
    {
        return true;
    }

    return false;
}

DImgLoader* DImgJPEGPlugin::loader(DImg* const image) const
{
    return new DImgJPEGLoader(image);
}

} // namespace DigikamJPEGDImgPlugin
