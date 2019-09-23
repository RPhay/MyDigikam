/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-22
 * Description : JPEG-2000 DImg plugin.
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

#include "dimgjpeg2000plugin.h"

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
#include "dimgjpeg2000loader.h"

namespace DigikamJPEG2000DImgPlugin
{

DImgJPEG2000Plugin::DImgJPEG2000Plugin(QObject* const parent)
    : DPluginDImg(parent)
{
}

DImgJPEG2000Plugin::~DImgJPEG2000Plugin()
{
}

QString DImgJPEG2000Plugin::name() const
{
    return i18n("JPEG-2000 DImg loader");
}

QString DImgJPEG2000Plugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon DImgJPEG2000Plugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("image-jpeg2000"));
}

QString DImgJPEG2000Plugin::description() const
{
    return i18n("A DImg image loader based on Libjasper codec");
}

QString DImgJPEG2000Plugin::details() const
{
    return i18n("<p>This plugin permit to load and save image with DImg using Libjasper codec</p>"
                "<p>The JPEG (Joint Photographic Experts Group) 2000 standard, finalized in 2001, "
                "defines a image-coding scheme using state-of-the-art compression techniques based "
                "on wavelet technology. Its architecture is useful for many diverse applications, "
                "including image archiving, security systems, digital photography, and medical imaging.</p>"
                "<p>See <a href='https://en.wikipedia.org/wiki/JPEG_2000'>"
                "Joint Photographic Experts Group documentation</a> for details.</p>"
    );
}

QList<DPluginAuthor> DImgJPEG2000Plugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2006-2019"))
            ;
}

void DImgJPEG2000Plugin::setup(QObject* const /*parent*/)
{
    // Nothing to do
}

QString DImgJPEG2000Plugin::loaderName() const
{
    return QLatin1String("JPEG2000");
}

QString DImgJPEG2000Plugin::typeMimes() const
{
    return QLatin1String("jp2 jpx jpc jp2k pgx");
}

bool DImgJPEG2000Plugin::canRead(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists())
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "File " << filePath << " does not exist";
        return false;
    }

    // First simply check file extension

    QString ext = fileInfo.suffix().toUpper();

    if (!ext.isEmpty() &&
        (ext == QLatin1String("JP2") || ext == QLatin1String("JPX") || // JPEG2000 file format
         ext == QLatin1String("JPC") || ext == QLatin1String("J2K") || // JPEG2000 code stream
         ext == QLatin1String("PGX"))                                  // JPEG2000 WM format
       )
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

    uchar jp2ID[5] = { 0x6A, 0x50, 0x20, 0x20, 0x0D, };
    uchar jpcID[2] = { 0xFF, 0x4F };

    if (memcmp(&header[4], &jp2ID, 5) == 0 ||
        memcmp(&header,    &jpcID, 2) == 0)
    {
        return true;
    }

    return false;
}

bool DImgJPEG2000Plugin::canWrite(const QString& format) const
{
    if (format == QLatin1String("JP2") || format == QLatin1String("JPX") || // JPEG2000 file format
        format == QLatin1String("JPC") || format == QLatin1String("J2K") || // JPEG2000 code stream
        format == QLatin1String("PGX"))                                     // JPEG2000 WM format
    {
        return true;
    }

    return false;
}

DImgLoader* DImgJPEG2000Plugin::loader(DImg* const image, const DRawDecoding&) const
{
    return new DImgJPEG2000Loader(image);
}

} // namespace DigikamJPEG2000DImgPlugin
