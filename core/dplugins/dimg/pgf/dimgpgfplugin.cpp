/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-22
 * Description : PGF DImg plugin.
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

#include "dimgpgfplugin.h"

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
#include "dimgpgfloader.h"

namespace DigikamPGFDImgPlugin
{

DImgPGFPlugin::DImgPGFPlugin(QObject* const parent)
    : DPluginDImg(parent)
{
}

DImgPGFPlugin::~DImgPGFPlugin()
{
}

QString DImgPGFPlugin::name() const
{
    return i18n("PGF loader");
}

QString DImgPGFPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon DImgPGFPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("image-x-generic"));
}

QString DImgPGFPlugin::description() const
{
    return i18n("A DImg image loader based on Libpgf codec");
}

QString DImgPGFPlugin::details() const
{
    return i18n("<p>This plugin permit to load and save image with DImg using Libpgf codec.</p>"
                "<p>The Progressive Graphics File (PGF) is an efficient image file format, "
                "that is based on a fast, discrete wavelet transform with progressive coding "
                "features. PGF can be used for lossless and lossy compression. It's most suitable "
                "for natural images. PGF can be used as a very efficient and fast replacement of JPEG-2000.</p>"
                "<p>See <a href='https://en.wikipedia.org/wiki/Progressive_Graphics_File'>"
                "Progressive Graphics File documentation</a> for details.</p>"
    );
}

QList<DPluginAuthor> DImgPGFPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2009-2019"))
            ;
}

void DImgPGFPlugin::setup(QObject* const /*parent*/)
{
    // Nothing to do
}

QString DImgPGFPlugin::loaderName() const
{
    return QLatin1String("PGF");
}

QString DImgPGFPlugin::typeMimes() const
{
    return QLatin1String("PGF");
}

bool DImgPGFPlugin::canRead(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists())
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "File " << filePath << " does not exist";
        return false;
    }

    // First simply check file extension

    QString ext = fileInfo.suffix().toUpper();

    if (!ext.isEmpty() && (ext == QLatin1String("PGF")))
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

    uchar pgfID[3] = { 0x50, 0x47, 0x46 };

    if (memcmp(&header, &pgfID, 3) == 0)
    {
        return true;
    }

    return false;
}

bool DImgPGFPlugin::canWrite(const QString& format) const
{
    if (format == QLatin1String("PGF"))
    {
        return true;
    }

    return false;
}

DImgLoader* DImgPGFPlugin::loader(DImg* const image, const DRawDecoding&) const
{
    return new DImgPGFLoader(image);
}

} // namespace DigikamPGFDImgPlugin
