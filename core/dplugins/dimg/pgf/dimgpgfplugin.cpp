/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-22
 * Description : PGF DImg plugin.
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

#include "dimgpgfplugin.h"

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
    return i18n("An image loader based on Libpgf codec");
}

QString DImgPGFPlugin::details() const
{
    return i18n("<p>This plugin permit to load and save image using Libpgf codec.</p>"
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
                             QString::fromUtf8("(C) 2009-2021"))
            ;
}

void DImgPGFPlugin::setup(QObject* const /*parent*/)
{
    // Nothing to do
}

QMap<QString, QString> DImgPGFPlugin::extraAboutData() const
{
    QMap<QString, QString> map;
    map.insert(QLatin1String("PGF"), i18n("Progressive Graphics File"));
    return map;
}

QString DImgPGFPlugin::loaderName() const
{
    return QLatin1String("PGF");
}

QString DImgPGFPlugin::typeMimes() const
{
    return QLatin1String("PGF");
}

int DImgPGFPlugin::canRead(const QFileInfo& fileInfo, bool magic) const
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

    uchar pgfID[3] = { 0x50, 0x47, 0x46 };

    if (memcmp(&header, &pgfID, 3) == 0)
    {
        return 10;
    }

    return 0;
}

int DImgPGFPlugin::canWrite(const QString& format) const
{
    return typeMimes().contains(format.toUpper()) ? 10 : 0;
}

DImgLoader* DImgPGFPlugin::loader(DImg* const image, const DRawDecoding&) const
{
    return new DImgPGFLoader(image);
}

} // namespace DigikamPGFDImgPlugin
