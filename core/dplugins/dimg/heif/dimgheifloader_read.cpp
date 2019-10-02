/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-26
 * Description : A HEIF IO file for DImg framework - read operations
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

bool DImgHEIFLoader::load(const QString& filePath, DImgLoaderObserver* const observer)
{
    m_observer = observer;

//    readMetadata(filePath, DImg::QIMAGE); NOTE: Exiv2 do not support HEIC yet

    FILE* const file = fopen(QFile::encodeName(filePath).constData(), "rb");

    if (!file)
    {
        qWarning() << "Error: Could not open source file.";
        loadingFailed();
        return false;
    }

    const int headerLen = 12;

    unsigned char header[headerLen];

    if (fread(&header, headerLen, 1, file) != 1)
    {
        qWarning() << "Error: Could not parse magic identifier.";
        fclose(file);
        loadingFailed();
        return false;
    }

    if ((memcmp(&header[4], "ftyp", 4) != 0) &&
        (memcmp(&header[8], "heic", 4) != 0) &&
        (memcmp(&header[8], "heix", 4) != 0) &&
        (memcmp(&header[8], "mif1", 4) != 0))
    {
        qWarning() << "Error: source file is not HEIC image.";
        fclose(file);
        loadingFailed();
        return false;
    }

    fclose(file);

    if (observer)
    {
        observer->progressInfo(m_image, 0.1F);
    }

    // -------------------------------------------------------------------
    // Initialize HEIF API.

    heif_item_id primary_image_id;

    struct heif_context* const heif_context = heif_context_alloc();
    struct heif_error error                 = heif_context_read_from_file(heif_context,
                                              QFile::encodeName(filePath).constData(), NULL);

    if (!isHeifSuccess(&error))
    {
        qWarning() << "Error: Could not read source file.";
        loadingFailed();
        heif_context_free(heif_context);
        return false;
    }

    error = heif_context_get_primary_image_ID(heif_context, &primary_image_id);

    if (!isHeifSuccess(&error))
    {
        qWarning() << "Error: Could not load image data.";
        loadingFailed();
        heif_context_free(heif_context);
        return false;
    }

    return (readHEICImageByID(heif_context, primary_image_id));
}

bool DImgHEIFLoader::readHEICColorProfile(struct heif_image_handle* const image_handle)
{
#if LIBHEIF_NUMERIC_VERSION >= 0x01040000

    size_t length = heif_image_handle_get_raw_color_profile_size(image_handle);

    if (length > 0)
    {
        // Read color profile.

        QByteArray profile;
        profile.resize(length);

        struct heif_error error = heif_image_handle_get_raw_color_profile(image_handle,
                                                                          profile.data());

        if (error.code == 0)
        {
            qDebug() << "HEIC color profile found with size:" << length;
            imageSetIccProfile(IccProfile(profile));
            return true;
        }
    }

#else
    Q_UNUSED(image_handle);
#endif

    // If ICC profile is null, check Exif metadata.

    if (checkExifWorkingColorSpace())
    {
        return true;
    }

    return false;
}

bool DImgHEIFLoader::readHEICMetadata(struct heif_image_handle* const image_handle)
{
    heif_item_id dataIds[10];
    QByteArray exif;
    QByteArray xmp;

    int count = heif_image_handle_get_list_of_metadata_block_IDs(image_handle,
                                                                 nullptr,
                                                                 dataIds,
                                                                 10);

    if (count > 0)
    {
        for (int i = 0 ; i < count ; ++i)
        {
            if (QLatin1String(heif_image_handle_get_metadata_type(image_handle, dataIds[i])) == QLatin1String("Exif"))
            {
                // Read Exif chunk.

                size_t length = heif_image_handle_get_metadata_size(image_handle, dataIds[i]);

                QByteArray exifChunk;
                exifChunk.resize(length);

                struct heif_error error = heif_image_handle_get_metadata(image_handle,
                                                                         dataIds[i],
                                                                         exifChunk.data());

                if ((error.code == 0) && (length > 4))
                {
                    // The first 4 bytes indicate the
                    // offset to the start of the TIFF header of the Exif data.

                    int skip = ((exifChunk.constData()[0] << 24) |
                                (exifChunk.constData()[1] << 16) |
                                (exifChunk.constData()[2] << 8)  |
                                 exifChunk.constData()[3]) + 4;

                    if (exifChunk.size() > skip)
                    {
                        // Copy the real exif data into the byte array

                        qDebug() << "HEIC exif container found with size:" << length - skip;
                        exif.append((char*)(exifChunk.data() + skip), exifChunk.size() - skip);
                    }
                }
            }

            if (
                (QLatin1String(heif_image_handle_get_metadata_type(image_handle, dataIds[i]))         == QLatin1String("mime")) &&
                (QLatin1String(heif_image_handle_get_metadata_content_type(image_handle, dataIds[i])) == QLatin1String("application/rdf+xml"))
               )
            {
                // Read Xmp chunk.

                size_t length = heif_image_handle_get_metadata_size(image_handle, dataIds[i]);

                xmp.resize(length);

                struct heif_error error = heif_image_handle_get_metadata(image_handle,
                                                                         dataIds[i],
                                                                         xmp.data());

                if ((error.code == 0))
                {
                    qDebug() << "HEIC xmp container found with size:" << length;
                }
                else
                {
                    xmp = QByteArray();
                }
            }
        }
    }

    if (!exif.isEmpty() || !xmp.isEmpty())
    {
        MetaEngine meta;

        if (!exif.isEmpty())
            meta.setExif(exif);

        if (!xmp.isEmpty())
            meta.setXmp(xmp);

        m_image->setMetadata(meta.data());
        return true;
    }

    return false;
}

bool DImgHEIFLoader::readHEICImageByID(struct heif_context* const heif_context,
                                       heif_item_id image_id)
{
    struct heif_image*        heif_image   = nullptr;
    struct heif_image_handle* image_handle = nullptr;
    struct heif_error error                = heif_context_get_image_handle(heif_context,
                                                                           image_id,
                                                                           &image_handle);

    if (!isHeifSuccess(&error))
    {
        return false;
    }

    // NOTE: An HEIC image without ICC color profile or without metadata still valid.

    if (m_loadFlags & LoadICCData)
    {
        readHEICMetadata(image_handle);
        readHEICColorProfile(image_handle);
    }

    if (m_observer)
    {
        m_observer->progressInfo(m_image, 0.2F);
    }

    if (m_loadFlags & LoadImageData)
    {
        // Copy HEIF image into data structures.

        struct heif_decoding_options* const decode_options = heif_decoding_options_alloc();
        decode_options->ignore_transformations             = 1;
        m_hasAlpha                                         = heif_image_handle_has_alpha_channel(image_handle);
        heif_chroma chroma                                 = m_hasAlpha ? heif_chroma_interleaved_RGBA
                                                                        : heif_chroma_interleaved_RGB;

        // Trace to check image size properties before decoding, as these values can be different.
        qDebug() << "HEIC image size: ("
                 << heif_image_handle_get_width(image_handle)
                 << "x"
                 << heif_image_handle_get_height(image_handle)
                 << ")";

        error = heif_decode_image(image_handle,
                                  &heif_image,
                                  heif_colorspace_RGB,
                                  chroma,
                                  decode_options);

        if (!isHeifSuccess(&error))
        {
            heif_image_handle_release(image_handle);
            return false;
        }

        if (m_observer)
        {
            m_observer->progressInfo(m_image, 0.3F);
        }

        heif_decoding_options_free(decode_options);

        int colorDepth = heif_image_get_bits_per_pixel(heif_image, heif_channel_interleaved);
        imageWidth()   = heif_image_get_width(heif_image, heif_channel_interleaved);
        imageHeight()  = heif_image_get_height(heif_image, heif_channel_interleaved);

        qDebug() << "Decoded HEIC image properties: size("
                 << imageWidth() << "x" << imageHeight()
                 << "), Alpha:" << m_hasAlpha
                 << ", Color depth :" << colorDepth;

        if (!QSize(imageWidth(), imageHeight()).isValid())
        {
            heif_image_release(heif_image);
            heif_image_handle_release(image_handle);
            return false;
        }

        int stride         = 0;
        uint8_t* const ptr = heif_image_get_plane(heif_image, heif_channel_interleaved, &stride);

        qDebug() << "HEIC data  :" << ptr;
        qDebug() << "HEIC stride:" << stride;

        if (!ptr || stride <= 0)
        {
            qWarning() << "Invalid data pixel information!";
            heif_image_release(heif_image);
            heif_image_handle_release(image_handle);
            return false;
        }

        uchar* data = nullptr;

        if (colorDepth == 24 ||       // RGB
            colorDepth == 32)         // RGBA
        {
            qDebug() << "Color bytes depth: 8";
            m_sixteenBit = false;
        }
        else if (colorDepth == 48 ||  // RGB
                 colorDepth == 64)    // RGBA
        {
            qDebug() << "Color bytes depth: 16";
            m_sixteenBit = true;
        }
        else
        {
            qWarning() << "Color bits depth: " << colorDepth << ": not supported!";
            heif_image_release(heif_image);
            heif_image_handle_release(image_handle);
            return false;
        }

        if (m_sixteenBit)
        {
            data = new_failureTolerant(imageWidth(), imageHeight(), 8); // 16 bits/color/pixel
        }
        else
        {
            data = new_failureTolerant(imageWidth(), imageHeight(), 4); // 8 bits/color/pixel
        }

        if (m_observer)
        {
            m_observer->progressInfo(m_image, 0.4F);
        }

        uchar* dst              = data;
        unsigned short* dst16   = reinterpret_cast<unsigned short*>(data);
        uchar* src              = reinterpret_cast<unsigned char*>(ptr);
        unsigned short* src16   = reinterpret_cast<unsigned short*>(ptr);
        unsigned int checkPoint = 0;
        unsigned int size       = imageHeight()*imageWidth();

        for (unsigned int i = 0 ; i < size ; ++i)
        {
            if (!m_sixteenBit)   // 8 bits image.
            {
                // Blue
                dst[0] = src[2];
                // Green
                dst[1] = src[1];
                // Red
                dst[2] = src[3];

                // Alpha

                if (m_hasAlpha)
                {
                    dst[3] = src[3];
                    src   += 4;
                }
                else
                {
                    dst[3] = 0xFF;
                    src   += 3;
                }

                dst += 4;
            }
            else                // 16 bits image.
            {
                // Blue
                dst16[0] = src16[2];
                // Green
                dst16[1] = src16[1];
                // Red
                dst16[2] = src16[0];

                // Alpha

                if (m_hasAlpha)
                {
                    dst16[3] = src16[3];
                    src16   += 4;
                }
                else
                {
                    dst16[3] = 0xFFFF;
                    src16   += 3;
                }

                dst16 += 4;
            }


            if (m_observer && i >= checkPoint)
            {
                checkPoint += granularity(m_observer, i, 0.8F);

                if (!m_observer->continueQuery(m_image))
                {
                    heif_image_release(heif_image);
                    heif_image_handle_release(image_handle);

                    loadingFailed();
                    return false;
                }

                m_observer->progressInfo(m_image, 0.4 + (0.8 * (((float)i) / ((float)size))));
            }
        }

        imageData() = data;
        imageSetAttribute(QLatin1String("format"),             QLatin1String("HEIF"));
        imageSetAttribute(QLatin1String("originalColorModel"), DImg::RGB);
        imageSetAttribute(QLatin1String("originalBitDepth"),   m_sixteenBit ? 16 : 8);
        imageSetAttribute(QLatin1String("originalSize"),       QSize(imageWidth(), imageHeight()));
    }

    if (m_observer)
    {
        m_observer->progressInfo(m_image, 0.9F);
    }

    heif_image_release(heif_image);
    heif_image_handle_release(image_handle);

    return true;
}

} // namespace Digikam
