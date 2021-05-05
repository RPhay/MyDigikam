/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-26
 * Description : A HEIF IO file for DImg framework - load operations
 *
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include "digikam_config.h"
#include "dimg.h"
#include "dimgloaderobserver.h"
#include "metaengine.h"

namespace Digikam
{

bool DImgHEIFLoader::load(const QString& filePath, DImgLoaderObserver* const observer)
{
    m_observer = observer;

    readMetadata(filePath);

    FILE* file = fopen(QFile::encodeName(filePath).constData(), "rb");

#ifdef Q_OS_WIN

    bool loadToMemory = false;

    if (!file)
    {
        file          = _wfopen((const wchar_t*)filePath.utf16(), L"rb");
        loadToMemory  = true;
    }

#endif

    if (!file)
    {
        qCWarning(DIGIKAM_DIMG_LOG_HEIF) << "Error: Could not open source file.";
        loadingFailed();

        return false;
    }

    const int headerLen = 12;

    unsigned char header[headerLen];

    if (fread(&header, headerLen, 1, file) != 1)
    {
        qCWarning(DIGIKAM_DIMG_LOG_HEIF) << "Error: Could not parse magic identifier.";
        fclose(file);
        loadingFailed();

        return false;
    }

    if ((memcmp(&header[4], "ftyp", 4) != 0) &&
        (memcmp(&header[8], "heic", 4) != 0) &&
        (memcmp(&header[8], "heix", 4) != 0) &&
        (memcmp(&header[8], "mif1", 4) != 0))
    {
        qCWarning(DIGIKAM_DIMG_LOG_HEIF) << "Error: source file is not HEIF image.";
        fclose(file);
        loadingFailed();

        return false;
    }

    fclose(file);

    if (observer)
    {
        observer->progressInfo(0.1F);
    }

    // -------------------------------------------------------------------
    // Initialize HEIF API.

    heif_item_id primary_image_id;

    struct heif_context* const heif_context = heif_context_alloc();
    struct heif_error error;

#ifdef Q_OS_WIN

    if (loadToMemory)
    {
        QByteArray buffer;
        QFile memFile(filePath);

        if (!memFile.open(QIODevice::ReadOnly))
        {
            qCWarning(DIGIKAM_DIMG_LOG_HEIF) << "Error: Could not load into memory.";
            heif_context_free(heif_context);
            loadingFailed();

            return false;
        }

        buffer = memFile.readAll();
        memFile.close();

        qCDebug(DIGIKAM_DIMG_LOG_HEIF) << "HEIF loading file to memory buffer";

        error = heif_context_read_from_memory(heif_context,
                                              (void*)buffer.data(),
                                              buffer.size(),
                                              nullptr);
    }
    else

#endif

    {

        error = heif_context_read_from_file(heif_context,
                                            QFile::encodeName(filePath).constData(),
                                            nullptr);
    }

    if (!isHeifSuccess(&error))
    {
        qCWarning(DIGIKAM_DIMG_LOG_HEIF) << "Error: Could not read source file.";
        heif_context_free(heif_context);
        loadingFailed();

        return false;
    }

    error = heif_context_get_primary_image_ID(heif_context, &primary_image_id);

    if (!isHeifSuccess(&error))
    {
        qCWarning(DIGIKAM_DIMG_LOG_HEIF) << "Error: Could not load image data.";
        heif_context_free(heif_context);
        loadingFailed();

        return false;
    }

    bool ret = readHEICImageByID(heif_context, primary_image_id);
    heif_context_free(heif_context);

    return ret;
}

bool DImgHEIFLoader::readHEICColorProfile(struct heif_image_handle* const image_handle)
{

#if LIBHEIF_NUMERIC_VERSION >= 0x01040000

    switch (heif_image_handle_get_color_profile_type(image_handle))
    {
        case heif_color_profile_type_not_present:
            break;

        case heif_color_profile_type_rICC:
        case heif_color_profile_type_prof:
        {
            size_t length = heif_image_handle_get_raw_color_profile_size(image_handle);

            if (length > 0)
            {
                // Read color profile.

                QByteArray profile;
                profile.resize((int)length);

                struct heif_error error = heif_image_handle_get_raw_color_profile(image_handle,
                                                                                  profile.data());

                if (error.code == 0)
                {
                    qCDebug(DIGIKAM_DIMG_LOG_HEIF) << "HEIF color profile found with size:" << length;
                    imageSetIccProfile(IccProfile(profile));

                    return true;
                }
            }

            break;
        }

        default: // heif_color_profile_type_nclx
            qCWarning(DIGIKAM_DIMG_LOG_HEIF) << "Unknown HEIF color profile type discarded";
            break;
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
        loadingFailed();

        return false;
    }

    // NOTE: An HEIC image without ICC color profile or without metadata still valid.

    if (m_loadFlags & LoadICCData)
    {
        readHEICColorProfile(image_handle);
    }

    if (m_observer)
    {
        m_observer->progressInfo(0.2F);
    }

    if (m_loadFlags & LoadPreview)
    {
        heif_item_id thumbnail_ID = 0;
        int nThumbnails           = heif_image_handle_get_list_of_thumbnail_IDs(image_handle, &thumbnail_ID, 1);

        if (nThumbnails > 0)
        {
            struct heif_image_handle* thumbnail_handle = nullptr;
            error = heif_image_handle_get_thumbnail(image_handle, thumbnail_ID, &thumbnail_handle);

            if (!isHeifSuccess(&error))
            {
                loadingFailed();
                heif_image_handle_release(image_handle);

                return false;
            }

            // Save the original size, the size
            // may differ from the decoded image size.

            QSize originalSize(heif_image_handle_get_width(image_handle),
                               heif_image_handle_get_height(image_handle));

            heif_image_handle_release(image_handle);

            qCDebug(DIGIKAM_DIMG_LOG_HEIF) << "HEIF preview found in thumbnail chunk";

            bool ret = readHEICImageByHandle(thumbnail_handle, heif_image, true);

            // Restore original size.

            imageSetAttribute(QLatin1String("originalSize"), originalSize);

            return ret;
        }

        // Image has no preview, load image normally

        return readHEICImageByHandle(image_handle, heif_image, true);
    }

    // Load image data or only image metadata

    return readHEICImageByHandle(image_handle, heif_image, (m_loadFlags & LoadImageData));
}

bool DImgHEIFLoader::readHEICImageByHandle(struct heif_image_handle* image_handle,
                                           struct heif_image* heif_image, bool loadImageData)
{
    int lumaBits   = heif_image_handle_get_luma_bits_per_pixel(image_handle);
    int chromaBits = heif_image_handle_get_chroma_bits_per_pixel(image_handle);

    if ((lumaBits == -1) || (chromaBits == -1))
    {
        qCWarning(DIGIKAM_DIMG_LOG_HEIF) << "HEIC luma or chroma bits information not valid!";
        loadingFailed();
        heif_image_release(heif_image);
        heif_image_handle_release(image_handle);

        return false;
    }

    // Copy HEIF image into data structures.

    struct heif_error error;
    struct heif_decoding_options* const decode_options = heif_decoding_options_alloc();
    decode_options->ignore_transformations             = 1;
    m_hasAlpha                                         = heif_image_handle_has_alpha_channel(image_handle);
    heif_chroma chroma                                 = m_hasAlpha ? heif_chroma_interleaved_RGBA
                                                                    : heif_chroma_interleaved_RGB;

    // Trace to check image size properties before decoding, as these values can be different.

    qCDebug(DIGIKAM_DIMG_LOG_HEIF) << "HEIF image size: ("
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
        loadingFailed();
        heif_image_release(heif_image);
        heif_image_handle_release(image_handle);
        heif_decoding_options_free(decode_options);

        return false;
    }

    if (m_observer)
    {
        m_observer->progressInfo(0.3F);
    }

    heif_decoding_options_free(decode_options);

    int colorDepth = heif_image_get_bits_per_pixel_range(heif_image, heif_channel_interleaved);
    imageWidth()   = heif_image_get_width(heif_image, heif_channel_interleaved);
    imageHeight()  = heif_image_get_height(heif_image, heif_channel_interleaved);

    qCDebug(DIGIKAM_DIMG_LOG_HEIF) << "Decoded HEIF image properties: size("
                << imageWidth() << "x" << imageHeight()
                << "), Alpha:" << m_hasAlpha
                << ", Color depth :" << colorDepth;

    if (!QSize(imageWidth(), imageHeight()).isValid())
    {
        loadingFailed();
        heif_image_release(heif_image);
        heif_image_handle_release(image_handle);

        return false;
    }

    int stride         = 0;
    uint8_t* const ptr = heif_image_get_plane(heif_image, heif_channel_interleaved, &stride);

    qCDebug(DIGIKAM_DIMG_LOG_HEIF) << "HEIF data container:" << ptr;
    qCDebug(DIGIKAM_DIMG_LOG_HEIF) << "HEIC bytes per line:" << stride;

    if (!ptr || stride <= 0)
    {
        qCWarning(DIGIKAM_DIMG_LOG_HEIF) << "HEIC data pixels information not valid!";
        loadingFailed();
        heif_image_release(heif_image);
        heif_image_handle_release(image_handle);

        return false;
    }

    uchar* data    = nullptr;
    int colorMul   = 1;       // color multiplier
    int colorModel = DImg::RGB;

    if      (colorDepth == 8)
    {
        qCDebug(DIGIKAM_DIMG_LOG_HEIF) << "Color bytes depth: 8";
        m_sixteenBit = false;
    }
    else if ((colorDepth > 8) && (colorDepth <= 16))
    {
        qCDebug(DIGIKAM_DIMG_LOG_HEIF) << "Color bytes depth: 16";
        m_sixteenBit = true;
        colorMul     = 16 - colorDepth;
    }
    else
    {
        qCWarning(DIGIKAM_DIMG_LOG_HEIF) << "Color bits depth: " << colorDepth << ": not supported!";
        loadingFailed();
        heif_image_release(heif_image);
        heif_image_handle_release(image_handle);

        return false;
    }

    if (loadImageData)
    {
        qCDebug(DIGIKAM_DIMG_LOG_HEIF) << "Color multiplier:" << colorMul;

        if (m_sixteenBit)
        {
            data = new_failureTolerant(imageWidth(), imageHeight(), 8); // 16 bits/color/pixel
        }
        else
        {
            data = new_failureTolerant(imageWidth(), imageHeight(), 4); // 8 bits/color/pixel
        }

        if (!data)
        {
            qCWarning(DIGIKAM_DIMG_LOG_HEIF) << "Cannot allocate memory!";
            loadingFailed();
            heif_image_release(heif_image);
            heif_image_handle_release(image_handle);

            return false;
        }

        if (m_observer)
        {
            m_observer->progressInfo(0.4F);
        }

        uchar* dst              = data;
        unsigned short* dst16   = reinterpret_cast<unsigned short*>(data);
        uchar* src              = nullptr;
        unsigned short* src16   = nullptr;
        unsigned int checkPoint = 0;

        for (unsigned int y = 0 ; y < imageHeight() ; ++y)
        {
            src   = reinterpret_cast<unsigned char*>(ptr + (y * stride));
            src16 = reinterpret_cast<unsigned short*>(src);

            for (unsigned int x = 0 ; x < imageWidth() ; ++x)
            {
                if (!m_sixteenBit)   // 8 bits image.
                {
                    // Blue
                    dst[0] = src[2];
                    // Green
                    dst[1] = src[1];
                    // Red
                    dst[2] = src[0];

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
                    dst16[0] = (unsigned short)(src16[2] << colorMul);
                    // Green
                    dst16[1] = (unsigned short)(src16[1] << colorMul);
                    // Red
                    dst16[2] = (unsigned short)(src16[0] << colorMul);

                    // Alpha

                    if (m_hasAlpha)
                    {
                        dst16[3] = (unsigned short)(src16[3] << colorMul);
                        src16   += 4;
                    }
                    else
                    {
                        dst16[3] = 0xFFFF;
                        src16   += 3;
                    }

                        dst16 += 4;
            }
            }

            if (m_observer && y >= checkPoint)
            {
                checkPoint += granularity(m_observer, y, 0.8F);

                if (!m_observer->continueQuery())
                {
                    loadingFailed();
                    heif_image_release(heif_image);
                    heif_image_handle_release(image_handle);

                    return false;
                }

                m_observer->progressInfo(0.4F + (0.8F * (((float)y) / ((float)imageHeight()))));
            }
        }
    }

    imageData() = data;
    imageSetAttribute(QLatin1String("format"),             QLatin1String("HEIF"));
    imageSetAttribute(QLatin1String("originalColorModel"), colorModel);
    imageSetAttribute(QLatin1String("originalBitDepth"),   m_sixteenBit ? 16 : 8);
    imageSetAttribute(QLatin1String("originalSize"),       QSize(imageWidth(), imageHeight()));

    if (m_observer)
    {
        m_observer->progressInfo(0.9F);
    }

    heif_image_release(heif_image);
    heif_image_handle_release(image_handle);

    return true;
}

} // namespace Digikam
