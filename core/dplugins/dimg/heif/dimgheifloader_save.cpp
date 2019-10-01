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

// C includes

#include <math.h>

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

// libx265 includes

#ifdef HAVE_X265
#   include <x265.h>
#endif

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

    // --- Determine libx265 encoder bits depth capability: 8=standard, 10, 12, or later 16.

    int maxOutputBitsDepth = -1;

    for (int i = 16 ; i >= 8 ; i-=2)
    {
        qDebug() << "Check HEVC encoder for" << i << "bits encoding...";
        const x265_api* const api = x265_api_get(i);

        if (api)
        {
            maxOutputBitsDepth = i;
            break;
        }
    }

    qDebug() << "HEVC encoder max bits depth:" << maxOutputBitsDepth;

    if (maxOutputBitsDepth == -1)
    {
        qWarning() << "Cannot get max supported HEVC encoder bits depth!";
        return false;
    }

    heif_chroma chroma;

    if (maxOutputBitsDepth > 8)          // 16 bits image.
    {
        chroma = imageHasAlpha() ? heif_chroma_interleaved_RRGGBBAA_BE
                                 : heif_chroma_interleaved_RRGGBB_BE;
    }
    else
    {
        chroma = imageHasAlpha() ? heif_chroma_interleaved_RGBA
                                 : heif_chroma_interleaved_RGB;
    }

    // --- use standard HEVC encoder

    qDebug() << "HEVC encoder setup...";

    struct heif_context* const ctx = heif_context_alloc();

    if (!ctx)
    {
        qWarning() << "Cannot create HEIC context!";
        return false;
    }

    struct heif_encoder* encoder   = nullptr;
    struct heif_error error        = heif_context_get_encoder_for_format(ctx,
                                                                         heif_compression_HEVC,
                                                                         &encoder);

    if (!isHeifSuccess(&error))
    {
        heif_context_free(ctx);
        return false;
    }

    heif_encoder_set_lossy_quality(encoder, quality);
    heif_encoder_set_lossless(encoder, lossless);


    struct heif_image* image = nullptr;
    error                    = heif_image_create(imageWidth(),
                                                 imageHeight(),
                                                 heif_colorspace_RGB,
                                                 chroma,
                                                 &image);

    if (!isHeifSuccess(&error))
    {
        heif_encoder_release(encoder);
        heif_context_free(ctx);
        return false;
    }

    // --- Save color profile before to create image data, as converting to color space can be processed at this stage.

    qDebug() << "HEIC set color profile...";

    saveHEICColorProfile(image);

    // --- Add image data

    qDebug() << "HEIC setup data plane...";

    error = heif_image_add_plane(image,
                                 heif_channel_interleaved,
                                 imageWidth(),
                                 imageHeight(),
                                 maxOutputBitsDepth);

    if (!isHeifSuccess(&error))
    {
        heif_encoder_release(encoder);
        heif_context_free(ctx);
        return false;
    }

    int stride          = 0;
    uint8_t* const data = heif_image_get_plane(image,
                                               heif_channel_interleaved,
                                               &stride);

    if (!data)
    {
        qWarning() << "Cannot get HEIC RGB plane!";
        heif_encoder_release(encoder);
        heif_context_free(ctx);
        return false;
    }

    qDebug() << "HEIC plane details: ptr=" << data << " stride=" << stride;

    uint checkpoint           = 0;
    unsigned short r          = 0;
    unsigned short g          = 0;
    unsigned short b          = 0;
    unsigned short a          = 0;
    unsigned char* pixel      = nullptr;
    unsigned short* pixel16   = nullptr;
    unsigned char* ptr        = nullptr;
    unsigned short* ptr16     = nullptr;
    float div16               = (16 - maxOutputBitsDepth) > 0 ? 16.0 - maxOutputBitsDepth : 1.0;
    float mul8                = (maxOutputBitsDepth - 8)  > 0 ? maxOutputBitsDepth - 8.0  : 1.0;
    int nbOutputBytesPerColor = (maxOutputBitsDepth > 8) ? (imageHasAlpha() ? 4 * 2 : 3 * 2)  // output data stored on 16 bits
                                                         : (imageHasAlpha() ? 4     : 3    ); // output data stored on 8 bits

    qDebug() << "HEIC output bytes per color:" << nbOutputBytesPerColor;
    qDebug() << "HEIC 16 to 8 bits coeff.   :" << div16;
    qDebug() << "HEIC 8 to 16 bits coeff.   :" << mul8;

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
                pixel16 = reinterpret_cast<unsigned short*>(pixel);

                b = pixel16[0];
                g = pixel16[1];
                r = pixel16[2];

                if (imageHasAlpha())
                {
                    a = pixel16[3];
                }

                if (maxOutputBitsDepth > 8) // From 16 bits to 10 bits or more.
                {
                    ptr16    = reinterpret_cast<unsigned short*>(&data[((y * imageWidth()) + x) * nbOutputBytesPerColor]);
                    ptr16[0] = (unsigned short)floor(r / div16);
                    ptr16[1] = (unsigned short)floor(g / div16);
                    ptr16[2] = (unsigned short)floor(b / div16);

                    if (imageHasAlpha())
                    {
                        ptr16[3] = (unsigned short)floor(a / div16);
                    }
                }
                else                        // From 16 bits to 8 bits.
                {
                    ptr    = reinterpret_cast<unsigned char*>(&data[((y * imageWidth()) + x) * nbOutputBytesPerColor]);
                    ptr[0] = (unsigned char)floor(r / div16);
                    ptr[1] = (unsigned char)floor(g / div16);
                    ptr[2] = (unsigned char)floor(b / div16);

                    if (imageHasAlpha())
                    {
                        ptr[3] = (unsigned char)floor(a / div16);
                    }
                }
            }
            else                            // 8 bits image.
            {
                b = (unsigned short)pixel[0];
                g = (unsigned short)pixel[1];
                r = (unsigned short)pixel[2];

                if (imageHasAlpha())
                {
                    a = (unsigned short)(pixel[3]);
                }

                if (maxOutputBitsDepth > 8) // From 8 bits to 10 bits or more.
                {
                    ptr16    = reinterpret_cast<unsigned short*>(&data[((y * imageWidth()) + x) * nbOutputBytesPerColor]);
                    ptr16[0] = (unsigned short)floor(r * mul8);
                    ptr16[1] = (unsigned short)floor(g * mul8);
                    ptr16[2] = (unsigned short)floor(b * mul8);

                    if (imageHasAlpha())
                    {
                        ptr16[3] = (unsigned short)floor(a * mul8);
                    }
                }
                else                        // From 8 bits to 8 bits.
                {
                    ptr    = reinterpret_cast<unsigned char*>(&data[((y * imageWidth()) + x) * nbOutputBytesPerColor]);
                    ptr[0] = (unsigned char)r;
                    ptr[1] = (unsigned char)g;
                    ptr[2] = (unsigned char)b;

                    if (imageHasAlpha())
                    {
                        ptr[3] = (unsigned char)a;
                    }
                }
            }
        }
    }

    qDebug() << "HEIC image encoding...";

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

    qDebug() << "HEIC metadata embeding...";

    saveHEICMetadata(ctx, hdl);

    heif_image_handle_release(hdl);

    // --- TODO: Add thumnail image.

    // --- write HEIF file

    qDebug() << "HEIC flush to file...";

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
