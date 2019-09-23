/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-22
 * Description : PNG DImg plugin.
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

#include "dimgpngplugin.h"

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
    return i18n("PNG DImg loader");
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
    return i18n("A DImg image loader based on Libpng codec");
}

QString DImgPNGPlugin::details() const
{
    return i18n("<p>This plugin permit to load and save image with DImg using "
                "Libpng codec</p>"
                "<p>See <a href='https://en.wikipedia.org/wiki/Libpng'>Libpng documentation</a> for details.</p>"
    );
}

QList<DPluginAuthor> DImgPNGPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2005-2019"))
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
    return QLatin1String("png");
}

bool DImgPNGPlugin::canRead(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists())
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "File " << filePath << " does not exist";
        return false;
    }

    // First simply check file extension

    QString ext = fileInfo.suffix().toUpper();

    if (!ext.isEmpty() && (ext == QLatin1String("PNG")))
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

    uchar pngID[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };

    if (memcmp(&header, &pngID, 8) == 0)
    {
        return true;
    }

    return false;
}

bool DImgPNGPlugin::canWrite(const QString& format) const
{
    if (format == QLatin1String("PNG"))
    {
        return true;
    }

    return false;
}

DImgLoader* DImgPNGPlugin::loader(DImg* const image) const
{
    return new DImgPNGLoader(image);
}

} // namespace DigikamPNGDImgPlugin
