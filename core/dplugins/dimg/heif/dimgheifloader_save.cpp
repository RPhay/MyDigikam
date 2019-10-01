/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-26
 * Description : A HEIF IO file for DImg framework - save operations
 *
 * Copyright (C) 2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "dimgheifloader.h"

// Qt includes

#include <QFile>
#include <QVariant>
#include <QByteArray>
#include <QTextStream>
#include <QDataStream>
#include <qplatformdefs.h>

// Local includes

#include "digikam_debug.h"
#include "dimg.h"
#include "dimgloaderobserver.h"
#include "metaengine.h"

namespace Digikam
{

bool DImgHEIFLoader::save(const QString& filePath, DImgLoaderObserver* const observer)
{
#ifdef HAVE_X265

    m_observer = observer;

    // -------------------------------------------------------------------
    // Open the file

    FILE* const f = fopen(QFile::encodeName(filePath).constData(), "wb");

    if (!f)
    {
        qWarning() << "Cannot open target image file.";
        return false;
    }

    QVariant qualityAttr  = imageGetAttribute(QLatin1String("quality"));
    int quality           = qualityAttr.isValid() ? qualityAttr.toInt() : 50;
    QVariant losslessAttr = imageGetAttribute(QLatin1String("lossless"));
    bool lossless         = losslessAttr.isValid() ? qualityAttr.toBool() : false;
    heif_chroma chroma    = imageHasAlpha() ? heif_chroma_interleaved_RGBA
                                            : heif_chroma_interleaved_RGB;

    // --- use standard HEVC encoder

    struct heif_context* const ctx = heif_context_alloc();

    if (!ctx)
    {
        qWarning() << "Cannot create HEIC context!";
        return false;
    }

    struct heif_encoder* encoder   = nullptr;
    struct heif_error error        = heif_context_get_encoder_for_format(ctx, heif_compression_HEVC, &encoder);

    if (!isHeifSuccess(&error))
    {
        heif_context_free(ctx);
        return false;
    }

    heif_encoder_set_lossy_quality(encoder, quality);
    heif_encoder_set_lossless(encoder, lossless);

    struct heif_image* image = nullptr;
    error                    = heif_image_create(imageWidth(), imageHeight(), heif_colorspace_RGB, chroma, &image);

    if (!isHeifSuccess(&error))
    {
        heif_encoder_release(encoder);
        heif_context_free(ctx);
        return false;
    }

    // --- Save color profile before to create image data, as converting to color space can be processed at this stage.

    saveHEICColorProfile(image);

    // --- Add image data

    int nbBytesPerColor = imageHasAlpha()   ?                 4 : 3;
    nbBytesPerColor     = imageSixteenBit() ? nbBytesPerColor*2 : nbBytesPerColor;

    error = heif_image_add_plane(image,
                                 heif_channel_interleaved,
                                 imageWidth(),
                                 imageHeight(),
                                 nbBytesPerColor * 8);

    if (!isHeifSuccess(&error))
    {
        heif_encoder_release(encoder);
        heif_context_free(ctx);
        return false;
    }

    int stride    = 0;
    uint8_t* data = heif_image_get_plane(image, heif_channel_interleaved, &stride);

    if (!data)
    {
        qWarning() << "Cannot get HEIC RGB plane!";
        heif_encoder_release(encoder);
        heif_context_free(ctx);
        return false;
    }

    qDebug() << "HEIC plane details: ptr=" << data << " stride=" << stride;

    uint checkpoint       = 0;
    unsigned short r      = 0;
    unsigned short g      = 0;
    unsigned short b      = 0;
    unsigned short a      = 0;
    unsigned char* pixel  = nullptr;
    unsigned char* ptr    = nullptr;
    unsigned short* ptr16 = nullptr;

    for (unsigned int y = 0 ; y < imageHeight() ; ++y)
    {
        if (m_observer && y == (long)checkpoint)
        {
            checkpoint += granularity(m_observer, imageHeight(), 0.8F);

            if (!m_observer->continueQuery(m_image))
            {
                heif_encoder_release(encoder);
                heif_context_free(ctx);
                return false;
            }

            m_observer->progressInfo(m_image, 0.1 + (0.8 * (((float)y) / ((float)imageHeight()))));
        }

        for (unsigned int x = 0 ; x < imageWidth() ; ++x)
        {
            pixel = &imageData()[((y * imageWidth()) + x) * imageBytesDepth()];

            if (imageSixteenBit())          // 16 bits image.
            {
                b = (unsigned short)(pixel[0] + 256 * pixel[1]);
                g = (unsigned short)(pixel[2] + 256 * pixel[3]);
                r = (unsigned short)(pixel[4] + 256 * pixel[5]);

                if (imageHasAlpha())
                {
                    a = (unsigned short)(pixel[6] + 256 * pixel[7]);
                }

                ptr16    = reinterpret_cast<unsigned short*>(&data[((y * imageWidth()) + x) * nbBytesPerColor]);
                ptr16[0] = r;
                ptr16[1] = g;
                ptr16[2] = b;

                if (imageHasAlpha())
                {
                    ptr16[3] = a;
                }
            }
            else                            // 8 bits image.
            {
                b = (unsigned char)pixel[0];
                g = (unsigned char)pixel[1];
                r = (unsigned char)pixel[2];

                if (imageHasAlpha())
                {
                    a = (unsigned char)(pixel[3]);
                }

                ptr    = reinterpret_cast<unsigned char*>(&data[((y * imageWidth()) + x) * nbBytesPerColor]);
                ptr[0] = r;
                ptr[1] = g;
                ptr[2] = b;

                if (imageHasAlpha())
                {
                    ptr[3] = a;
                }
            }
        }
    }

    // --- encode and write image

    struct heif_encoding_options* options = heif_encoding_options_alloc();
    options->save_alpha_channel           = imageHasAlpha() ? 1 : 0;
    struct heif_image_handle* hdl         = nullptr;
    error                                 = heif_context_encode_image(ctx, image, encoder, options, &hdl);

    if (!isHeifSuccess(&error))
    {
        heif_encoding_options_free(options);
        heif_image_handle_release(hdl);
        heif_encoder_release(encoder);
        heif_context_free(ctx);
        return false;
    }

    heif_encoding_options_free(options);
    heif_encoder_release(encoder);

    // --- Add Exif and XMP metadata

    saveHEICMetadata(ctx, hdl);

    heif_image_handle_release(hdl);

    // --- TODO: Add thumnail image.

    // --- write HEIF file

    error = heif_context_write_to_file(ctx, QFile::encodeName(filePath).constData());

    if (!isHeifSuccess(&error))
    {
        heif_context_free(ctx);
        return false;
    }

    heif_context_free(ctx);

    imageSetAttribute(QLatin1String("savedFormat"), QLatin1String("HEIC"));
    saveMetadata(filePath);

#endif

    return true;
}

bool DImgHEIFLoader::saveHEICColorProfile(struct heif_image* const image)
{
#if LIBHEIF_NUMERIC_VERSION >= 0x01040000

    QByteArray profile = m_image->getIccProfile().data();

    if (!profile.isEmpty())
    {
        // Save color profile.

        struct heif_error error = heif_image_set_raw_color_profile(image,
                                                                   "prof",           // FIXME: detect string in profile data
                                                                   profile.data(),
                                                                   profile.size());

        if (error.code != 0)
        {
            qWarning() << "Cannot set HEIC color profile!";
            return false;
        }
    }
#else
    Q_UNUSED(image_handle);
#endif

    return true;
}

bool DImgHEIFLoader::saveHEICMetadata(struct heif_context* const heif_context,
                                      struct heif_image_handle* const image_handle)
{
    MetaEngine meta(m_image->getMetadata());

    if (!meta.hasExif() && !meta.hasXmp())
    {
        return false;
    }

    struct heif_error error;

    if (meta.hasExif())
    {
        QByteArray exif = meta.getExifEncoded(true);
        error           = heif_context_add_exif_metadata(heif_context,
                                                         image_handle,
                                                         exif.data(),
                                                         exif.size());

        if (error.code != 0)
        {
            qWarning() << "Cannot set HEIC Exif metadata!";
            return false;
        }
    }

    if (meta.hasXmp())
    {
        QByteArray xmp = meta.getExifEncoded();
        error          = heif_context_add_XMP_metadata(heif_context,
                                                       image_handle,
                                                       xmp.data(),
                                                       xmp.size());

        if (error.code != 0)
        {
            qWarning() << "Cannot set HEIC Xmp metadata!";
            return false;
        }
    }

    return true;
}

} // namespace Digikam
