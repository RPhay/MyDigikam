/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-06-14
 * Description : A QImage loader for DImg framework.
 *
 * Copyright (C) 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2019 by Caulier Gilles <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "dimgqimageloader.h"

// Qt includes

#include <QImage>
#include <QByteArray>
#include <QImageReader>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
#   include <QColorSpace>
#endif

// Local includes

#include "digikam_debug.h"
#include "dimgloaderobserver.h"

namespace DigikamQImageDImgPlugin
{

DImgQImageLoader::DImgQImageLoader(DImg* const image)
    : DImgLoader  (image),
      m_hasAlpha  (false),
      m_sixteenBit(false)
{
}

DImgQImageLoader::~DImgQImageLoader()
{
}

bool DImgQImageLoader::load(const QString& filePath, DImgLoaderObserver* const observer)
{
    readMetadata(filePath);

    // Loading is opaque to us. No support for stopping from observer,
    // progress info are only pseudo values

    QImageReader reader(filePath);
    reader.setDecideFormatFromContent(true);

    QByteArray readFormat = reader.format();
    QImage image          = reader.read();

    if (observer)
    {
        observer->progressInfo(0.9F);
    }

    if (image.isNull())
    {
        qCWarning(DIGIKAM_DIMG_LOG_QIMAGE) << "Can not load \"" << filePath << "\" using DImg::DImgQImageLoader!";
        qCWarning(DIGIKAM_DIMG_LOG_QIMAGE) << "Error message from loader:" << reader.errorString();
        loadingFailed();
        return false;
    }

    int colorModel    = DImg::COLORMODELUNKNOWN;
    int originalDepth = 0;

    switch (image.format())
    {
        case QImage::Format_Invalid:
        default:
            colorModel    = DImg::COLORMODELUNKNOWN;
            originalDepth = 0;
            break;

        case QImage::Format_Mono:
        case QImage::Format_MonoLSB:
            colorModel    = DImg::MONOCHROME;
            originalDepth = 1;
            break;

        case QImage::Format_Indexed8:
            colorModel    = DImg::INDEXED;
            originalDepth = 0;
            break;

        case QImage::Format_RGB32:
            colorModel    = DImg::RGB;
            originalDepth = 8;
            break;

        case QImage::Format_ARGB32:
        case QImage::Format_ARGB32_Premultiplied:
            colorModel    = DImg::RGB;
            originalDepth = 8;
            break;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))

        case QImage::Format_RGBX64:
        case QImage::Format_RGBA64:
        case QImage::Format_RGBA64_Premultiplied:
            colorModel    = DImg::RGB;
            m_sixteenBit  = true;
            originalDepth = 16;
            break;

#endif

    }

    QImage target;
    uint w      = 0;
    uint h      = 0;
    uchar* data = nullptr;

    if (!m_sixteenBit)
    {
        qCDebug(DIGIKAM_DIMG_LOG_QIMAGE) << filePath << "is a 8 bits per color per pixels QImage";

        m_hasAlpha    = (image.hasAlphaChannel() && (readFormat != "psd"));
        target        = image.convertToFormat(QImage::Format_ARGB32);
        w             = target.width();
        h             = target.height();
        data          = new_failureTolerant(w, h, 4);

        if (!data)
        {
            qCWarning(DIGIKAM_DIMG_LOG_QIMAGE) << "Failed to allocate memory for loading" << filePath;
            loadingFailed();
            return false;
        }

        const QRgb* sptr = reinterpret_cast<const QRgb*>(target.constBits());
        uchar*      dptr = data;

        for (uint i = 0 ; i < w * h ; ++i)
        {
            dptr[0] = qBlue(*sptr);
            dptr[1] = qGreen(*sptr);
            dptr[2] = qRed(*sptr);
            dptr[3] = m_hasAlpha ? qAlpha(*sptr) : 255;

            dptr   += 4;
            sptr++;
        }
    }
    else
    {

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))

        qCDebug(DIGIKAM_DIMG_LOG_QIMAGE) << filePath << "is a 16 bits per color per pixels QImage";

        m_hasAlpha    = (image.hasAlphaChannel() && (readFormat != "psd"));
        target        = image.convertToFormat(QImage::Format_RGBA64);
        w             = target.width();
        h             = target.height();
        data          = new_failureTolerant(w, h, 8);

        if (!data)
        {
            qCWarning(DIGIKAM_DIMG_LOG_QIMAGE) << "Failed to allocate memory for loading" << filePath;
            loadingFailed();
            return false;
        }

        const QRgba64* sptr = reinterpret_cast<const QRgba64*>(target.constBits());
        ushort*        dptr = reinterpret_cast<ushort*>(data);

        for (uint i = 0 ; i < w * h ; ++i)
        {
            dptr[0] = (*sptr).blue();
            dptr[1] = (*sptr).green();
            dptr[2] = (*sptr).red();
            dptr[3] = m_hasAlpha ? (*sptr).alpha() : 65535;

            dptr   += 4;
            sptr++;
        }

#endif

    }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))

    if (m_loadFlags & LoadICCData)
    {
        QByteArray iccRawProfile(target.colorSpace().iccProfile());

        if (!iccRawProfile.isEmpty())
        {
            imageSetIccProfile(IccProfile(iccRawProfile));
        }
    }

#endif

    if (observer)
    {
        observer->progressInfo(1.0F);
    }

    imageWidth()  = w;
    imageHeight() = h;
    imageData()   = data;

    imageSetAttribute(QLatin1String("format"),             QString::fromLatin1(readFormat).toUpper());
    imageSetAttribute(QLatin1String("originalColorModel"), colorModel);
    imageSetAttribute(QLatin1String("originalBitDepth"),   originalDepth);
    imageSetAttribute(QLatin1String("originalSize"),       QSize(w, h));

    return true;
}

bool DImgQImageLoader::save(const QString& filePath, DImgLoaderObserver* const observer)
{
    QVariant qualityAttr = imageGetAttribute(QLatin1String("quality"));
    int quality          = qualityAttr.isValid() ? qualityAttr.toInt() : 90;

    if (quality < 0)
    {
        quality = 90;
    }

    if (quality > 100)
    {
        quality = 100;
    }

    QVariant formatAttr = imageGetAttribute(QLatin1String("format"));
    QByteArray format   = formatAttr.toByteArray();
    QImage image        = m_image->copyQImage();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))

    QByteArray iccRawProfile = m_image->getIccProfile().data();

    if (!iccRawProfile.isEmpty())
    {
        image.setColorSpace(QColorSpace::fromIccProfile(iccRawProfile));
    }

#endif

    if (observer)
    {
        observer->progressInfo(0.1F);
    }

    // Saving is opaque to us. No support for stopping from observer,
    // progress info are only pseudo values
    bool success = image.save(filePath, format.toUpper().constData(), quality);

    if (observer && success)
    {
        observer->progressInfo(1.0F);
    }

    imageSetAttribute(QLatin1String("format"), format.toUpper());

    saveMetadata(filePath);

    return success;
}

bool DImgQImageLoader::hasAlpha() const
{
    return m_hasAlpha;
}

bool DImgQImageLoader::sixteenBit() const
{
    return m_sixteenBit;
}

bool DImgQImageLoader::isReadOnly() const
{
    return false;
}

} // namespace DigikamQImageDImgPlugin
