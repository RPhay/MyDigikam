/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-22
 * Description : RAW DImg plugin.
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

#include "dimgrawplugin.h"

// C++ includes

#include <cstdio>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "dimgrawloader.h"
#include "drawdecoder.h"
#include "drawfiles.h"

namespace DigikamRAWDImgPlugin
{

DImgRAWPlugin::DImgRAWPlugin(QObject* const parent)
    : DPluginDImg(parent)
{
}

DImgRAWPlugin::~DImgRAWPlugin()
{
}

QString DImgRAWPlugin::name() const
{
    return i18n("RAW loader");
}

QString DImgRAWPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon DImgRAWPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("image-x-adobe-dng"));
}

QString DImgRAWPlugin::description() const
{
    return i18n("An image loader based on Libraw codec");
}

QString DImgRAWPlugin::details() const
{
    return i18n("<p>This plugin permit to load and save image using Libraw codec.</p>"
                "<p>A camera raw image file contains minimally processed data from the image sensor "
                "of either a digital camera, a motion picture film scanner, or other image scanner. "
                "Raw files are not yet processed and therefore are not ready to be printed or edited "
                "with a bitmap graphics editor. RAW are processed by a raw converter in a wide-gamut "
                "internal color space where precise adjustments can be made before conversion to a TIFF "
                "or JPEG for storage, printing, or further manipulation. This often encodes the image in "
                "a device-dependent color space.</p>"
                "<p>See <a href='https://en.wikipedia.org/wiki/Raw_image_format'>"
                "Raw Image File documentation</a> for details.</p>"
    );
}

QList<DPluginAuthor> DImgRAWPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Marcel Wiesweg"),
                             QString::fromUtf8("marcel dot wiesweg at gmx dot de"),
                             QString::fromUtf8("(C) 2005-2012"))
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2009-2021"))
            ;
}

void DImgRAWPlugin::setup(QObject* const /*parent*/)
{
    // Nothing to do
}

QMap<QString, QString> DImgRAWPlugin::extraAboutData() const
{
    return s_rawFileExtensionsdWithDesc();
}

QString DImgRAWPlugin::loaderName() const
{
    return QLatin1String("RAW");
}

QString DImgRAWPlugin::typeMimes() const
{
    return QString(DRawDecoder::rawFiles()).toUpper().remove(QLatin1String("*."));
}

int DImgRAWPlugin::canRead(const QFileInfo& fileInfo, bool magic) const
{
    if (!magic)
    {
        QString rawFilesExt = DRawDecoder::rawFiles();
        QString format      = fileInfo.suffix().toUpper();

        return (rawFilesExt.toUpper().contains(format)) ? 10 : 0;
    }

    return 0;
}

int DImgRAWPlugin::canWrite(const QString& /*format*/) const
{
    // NOTE: Raw are read only files.
    return 0;
}

DImgLoader* DImgRAWPlugin::loader(DImg* const image, const DRawDecoding& rawSettings) const
{
    return new DImgRAWLoader(image, rawSettings);
}

} // namespace DigikamRAWDImgPlugin
