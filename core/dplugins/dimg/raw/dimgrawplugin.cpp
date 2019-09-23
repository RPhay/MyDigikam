/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-22
 * Description : RAW DImg plugin.
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

#include "dimgrawplugin.h"

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
#include "dimgrawloader.h"
#include "drawdecoder.h"

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
    return i18n("A DImg image loader based on Libraw codec");
}

QString DImgRAWPlugin::details() const
{
    return i18n("<p>This plugin permit to load and save image with DImg using Libraw codec.</p>"
                "<p>A camera raw image file contains minimally processed data from the image sensor "
                "of either a digital camera, a motion picture film scanner, or other image scanner. "
                "Raw files are not yet processed and therefore are not ready to be printed or edited "
                "with a bitmap graphics editor. RAW are processed by a raw converter in a wide-gamut "
                "internal color space where precise adjustments can be made before conversion to a TIFF "
                "or JPEG for storage, printing, or further manipulation. This often encodes the image in "
                "a device-dependent color space.</p>"
                "The list of type-mimes supported by this plugin are: %1</p>"
                "<p>See <a href='https://en.wikipedia.org/wiki/Raw_image_format'>"
                "Raw Image File documentation</a> for details.</p>",
                typeMimes()
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
                             QString::fromUtf8("(C) 2009-2019"))
            ;
}

void DImgRAWPlugin::setup(QObject* const /*parent*/)
{
    // Nothing to do
}

QString DImgRAWPlugin::loaderName() const
{
    return QLatin1String("RAW");
}

QString DImgRAWPlugin::typeMimes() const
{
    return QString::fromLatin1(DRawDecoder::rawFiles()).toUpper().remove(QLatin1String("*."));
}

bool DImgRAWPlugin::canRead(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists())
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "File " << filePath << " does not exist";
        return false;
    }

    QString ext         = QFileInfo(filePath).suffix().toUpper();
    QString rawFilesExt = QLatin1String(DRawDecoder::rawFiles());

    if (rawFilesExt.toUpper().contains(ext))
    {
        return true;
    }

    return false;
}

bool DImgRAWPlugin::canWrite(const QString& /*format*/) const
{
    // NOTE: Raw are read only files.
    return false;
}

DImgLoader* DImgRAWPlugin::loader(DImg* const image, const DRawDecoding& rawSettings) const
{
    return new DImgRAWLoader(image, rawSettings);
}

} // namespace DigikamRAWDImgPlugin
