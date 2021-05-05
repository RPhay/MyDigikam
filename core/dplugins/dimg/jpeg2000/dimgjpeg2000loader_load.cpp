/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-06-14
 * Description : A JPEG-2000 IO file for DImg framework- load operations
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "dimgjpeg2000loader.h"

// Qt includes

#include <QFile>
#include <QByteArray>
#include <QTextStream>
#include <QScopedPointer>

// Local includes

#include "dimg.h"
#include "digikam_debug.h"
#include "digikam_config.h"
#include "dimgloaderobserver.h"
#include "dmetadata.h"

// Jasper includes

#ifndef Q_CC_MSVC
extern "C"
{
#endif

#if defined(Q_OS_DARWIN) && defined(Q_CC_CLANG)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wshift-negative-value"
#endif

#include <jasper/jasper.h>

#if defined(Q_OS_DARWIN) && defined(Q_CC_CLANG)
#   pragma clang diagnostic pop
#endif

#ifndef Q_CC_MSVC
}
#endif

namespace DigikamJPEG2000DImgPlugin
{

bool DImgJPEG2000Loader::load(const QString& filePath, DImgLoaderObserver* const observer)
{
    readMetadata(filePath);

#ifdef Q_OS_WIN

    FILE* const file = _wfopen((const wchar_t*)filePath.utf16(), L"rb");

#else

    FILE* const file = fopen(filePath.toUtf8().constData(), "rb");

#endif

    if (!file)
    {
        qCWarning(DIGIKAM_DIMG_LOG_JP2K) << "Unable to open JPEG2000 file";
        loadingFailed();

        return false;
    }

    unsigned char header[9];

    if (fread(&header, 9, 1, file) != 1)
    {
        loadingFailed();
        fclose(file);

        return false;
    }

    rewind(file);

    unsigned char jp2ID[5] = { 0x6A, 0x50, 0x20, 0x20, 0x0D, };
    unsigned char jpcID[2] = { 0xFF, 0x4F };

    if ((memcmp(&header[4], &jp2ID, 5) != 0) &&
        (memcmp(&header,    &jpcID, 2) != 0))
    {
        // not a jpeg2000 file

        loadingFailed();
        fclose(file);

        return false;
    }

    imageSetAttribute(QLatin1String("format"), QLatin1String("JP2"));

    if (!(m_loadFlags & LoadImageData) && !(m_loadFlags & LoadICCData))
    {
        // libjasper will load the full image in memory already when calling jas_image_decode.
        // This is bad when scanning. See bugs 215458 and 195583.
        // FIXME: Use Exiv2 to extract this info

        fclose(file);

        QScopedPointer<DMetadata> metadata(new DMetadata(filePath));
        QSize size = metadata->getItemDimensions();

        if (size.isValid())
        {
            imageWidth()  = size.width();
            imageHeight() = size.height();
        }

        return true;
    }

    // -------------------------------------------------------------------
    // Initialize JPEG 2000 API.

    long  i, x, y;
    int            components[4];
    unsigned int   maximum_component_depth, scale[4], x_step[4], y_step[4];
    unsigned long  number_components;

    jas_image_t*  jp2_image   = nullptr;
    jas_stream_t* jp2_stream  = nullptr;
    jas_matrix_t* pixels[4];

    int init = jas_init();

    if (init != 0)
    {
        qCWarning(DIGIKAM_DIMG_LOG_JP2K) << "Unable to init JPEG2000 decoder";
        loadingFailed();
        fclose(file);

        return false;
    }

    jp2_stream = jas_stream_freopen(filePath.toUtf8().constData(), "rb", file);

    if (jp2_stream == nullptr)
    {
        qCWarning(DIGIKAM_DIMG_LOG_JP2K) << "Unable to open JPEG2000 stream";
        loadingFailed();
        fclose(file);

        return false;
    }

    int fmt   = jas_image_strtofmt(QByteArray("jp2").data());
    jp2_image = jas_image_decode(jp2_stream, fmt, nullptr);

    if (jp2_image == nullptr)
    {
        qCWarning(DIGIKAM_DIMG_LOG_JP2K) << "Unable to decode JPEG2000 image";
        jas_stream_close(jp2_stream);
        loadingFailed();

        return false;
    }

    jas_stream_close(jp2_stream);

    // some pseudo-progress

    if (observer)
    {
        observer->progressInfo(0.1F);
    }

    // -------------------------------------------------------------------
    // Check color space.

    int colorModel = DImg::COLORMODELUNKNOWN;

    switch (jas_clrspc_fam(jas_image_clrspc(jp2_image)))
    {
        case JAS_CLRSPC_FAM_RGB:
        {
            components[0] = jas_image_getcmptbytype(jp2_image, JAS_IMAGE_CT_RGB_R);
            components[1] = jas_image_getcmptbytype(jp2_image, JAS_IMAGE_CT_RGB_G);
            components[2] = jas_image_getcmptbytype(jp2_image, JAS_IMAGE_CT_RGB_B);

            if ((components[0] < 0) || (components[1] < 0) || (components[2] < 0))
            {
                qCWarning(DIGIKAM_DIMG_LOG_JP2K) << "Error parsing JPEG2000 image : Missing Image Channel";
                jas_image_destroy(jp2_image);
                loadingFailed();

                return false;
            }

            number_components = 3;
            components[3]     = jas_image_getcmptbytype(jp2_image, 3);

            if (components[3] > 0)
            {
                m_hasAlpha = true;
                ++number_components;
            }

            colorModel = DImg::RGB;
            break;
        }

        case JAS_CLRSPC_FAM_GRAY:
        {
            components[0] = jas_image_getcmptbytype(jp2_image, JAS_IMAGE_CT_GRAY_Y);

            // cppcheck-suppress knownConditionTrueFalse
            if (components[0] < 0)
            {
                qCWarning(DIGIKAM_DIMG_LOG_JP2K) << "Error parsing JP2000 image : Missing Image Channel";
                jas_image_destroy(jp2_image);
                loadingFailed();

                return false;
            }

            number_components = 1;
            colorModel        = DImg::GRAYSCALE;
            break;
        }

        case JAS_CLRSPC_FAM_YCBCR:
        {
            components[0] = jas_image_getcmptbytype(jp2_image, JAS_IMAGE_CT_YCBCR_Y);
            components[1] = jas_image_getcmptbytype(jp2_image, JAS_IMAGE_CT_YCBCR_CB);
            components[2] = jas_image_getcmptbytype(jp2_image, JAS_IMAGE_CT_YCBCR_CR);

            // cppcheck-suppress knownConditionTrueFalse
            if ((components[0] < 0) || (components[1] < 0) || (components[2] < 0))
            {
                qCWarning(DIGIKAM_DIMG_LOG_JP2K) << "Error parsing JP2000 image : Missing Image Channel";
                jas_image_destroy(jp2_image);
                loadingFailed();

                return false;
            }

            number_components = 3;
            components[3]     = jas_image_getcmptbytype(jp2_image, JAS_IMAGE_CT_UNKNOWN);

            if (components[3] > 0)
            {
                m_hasAlpha = true;
                ++number_components;
            }

            // FIXME: image->colorspace=YCbCrColorspace;

            colorModel = DImg::YCBCR;
            break;
        }

        default:
        {
            qCWarning(DIGIKAM_DIMG_LOG_JP2K) << "Error parsing JP2000 image : Colorspace Model Is Not Supported";
            jas_image_destroy(jp2_image);
            loadingFailed();

            return false;
        }
    }

    // -------------------------------------------------------------------
    // Check image geometry.

    imageWidth()  = jas_image_width(jp2_image);
    imageHeight() = jas_image_height(jp2_image);

    for (i = 0 ; i < (long)number_components ; ++i)
    {
        if ((((jas_image_cmptwidth(jp2_image, components[i])*
               jas_image_cmpthstep(jp2_image, components[i])) != (long)imageWidth()))  ||
            (((jas_image_cmptheight(jp2_image, components[i])*
               jas_image_cmptvstep(jp2_image, components[i])) != (long)imageHeight())) ||
            (jas_image_cmpttlx(jp2_image, components[i])  != 0)                        ||
            (jas_image_cmpttly(jp2_image, components[i])  != 0)                        ||
            (jas_image_cmptsgnd(jp2_image, components[i]) != false))
        {
            qCWarning(DIGIKAM_DIMG_LOG_JP2K) << "Error parsing JPEG2000 image : Irregular Channel Geometry Not Supported";
            jas_image_destroy(jp2_image);
            loadingFailed();

            return false;
        }

        x_step[i] = jas_image_cmpthstep(jp2_image, components[i]);
        y_step[i] = jas_image_cmptvstep(jp2_image, components[i]);
    }

    // -------------------------------------------------------------------
    // Get image format.

    maximum_component_depth = 0;

    for (i = 0 ; i < (long)number_components ; ++i)
    {
        maximum_component_depth = qMax((long)jas_image_cmptprec(jp2_image, components[i]),
                                       (long)maximum_component_depth);

        pixels[i]               = jas_matrix_create(1, ((unsigned int)imageWidth()) / x_step[i]);

        if (!pixels[i])
        {
            qCWarning(DIGIKAM_DIMG_LOG_JP2K) << "Error decoding JPEG2000 image data : Memory Allocation Failed";
            jas_image_destroy(jp2_image);
            loadingFailed();

            return false;
        }
    }

    if (maximum_component_depth > 8)
    {
        m_sixteenBit = true;
    }

    for (i = 0 ; i < (long)number_components ; ++i)
    {
        scale[i] = 1;
        int prec = jas_image_cmptprec(jp2_image, components[i]);

        if (m_sixteenBit && prec < 16)
        {
            scale[i] = (1 << (16 - jas_image_cmptprec(jp2_image, components[i])));
        }
    }

    // -------------------------------------------------------------------
    // Get image data.

    QScopedArrayPointer<uchar> data;

    if (m_loadFlags & LoadImageData)
    {
        if (m_sixteenBit)          // 16 bits image.
        {
            data.reset(new_failureTolerant(imageWidth(), imageHeight(), 8));
        }
        else
        {
            data.reset(new_failureTolerant(imageWidth(), imageHeight(), 4));
        }

        if (!data)
        {
            qCWarning(DIGIKAM_DIMG_LOG_JP2K) << "Error decoding JPEG2000 image data : Memory Allocation Failed";
            jas_image_destroy(jp2_image);

            for (i = 0 ; i < (long)number_components ; ++i)
            {
                jas_matrix_destroy(pixels[i]);
            }

            jas_cleanup();
            loadingFailed();

            return false;
        }

        uint   checkPoint     = 0;
        uchar* dst            = data.data();
        unsigned short* dst16 = reinterpret_cast<unsigned short*>(data.data());

        for (y = 0 ; y < (long)imageHeight() ; ++y)
        {
            for (i = 0 ; i < (long)number_components ; ++i)
            {
                int ret = jas_image_readcmpt(jp2_image, (short)components[i], 0,
                                             ((unsigned int) y)            / y_step[i],
                                             ((unsigned int) imageWidth()) / x_step[i],
                                             1, pixels[i]);

                if (ret != 0)
                {
                    qCWarning(DIGIKAM_DIMG_LOG_JP2K) << "Error decoding JPEG2000 image data";
                    jas_image_destroy(jp2_image);

                    for (i = 0 ; i < (long)number_components ; ++i)
                    {
                        jas_matrix_destroy(pixels[i]);
                    }

                    jas_cleanup();
                    loadingFailed();

                    return false;
                }
            }

            switch (number_components)
            {
                case 1: // Grayscale.
                {
                    if (!m_sixteenBit)   // 8 bits image.
                    {
                        for (x = 0 ; x < (long)imageWidth() ; ++x)
                        {
                            dst[0] = (uchar)(scale[0] * jas_matrix_getv(pixels[0], x / x_step[0]));
                            dst[1] = dst[0];
                            dst[2] = dst[0];
                            dst[3] = 0xFF;

                            dst += 4;
                        }
                    }
                    else                // 16 bits image.
                    {
                        for (x = 0 ; x < (long)imageWidth() ; ++x)
                        {
                            dst16[0] = (unsigned short)(scale[0] * jas_matrix_getv(pixels[0], x / x_step[0]));
                            dst16[1] = dst16[0];
                            dst16[2] = dst16[0];
                            dst16[3] = 0xFFFF;

                            dst16 += 4;
                        }
                    }

                    break;
                }

                case 3: // RGB.
                {
                    if (!m_sixteenBit)   // 8 bits image.
                    {
                        for (x = 0 ; x < (long)imageWidth() ; ++x)
                        {
                            // Blue
                            dst[0] = (uchar)(scale[2] * jas_matrix_getv(pixels[2], x / x_step[2]));
                            // Green
                            dst[1] = (uchar)(scale[1] * jas_matrix_getv(pixels[1], x / x_step[1]));
                            // Red
                            dst[2] = (uchar)(scale[0] * jas_matrix_getv(pixels[0], x / x_step[0]));
                            // Alpha
                            dst[3] = 0xFF;

                            dst += 4;
                        }
                    }
                    else                // 16 bits image.
                    {
                        for (x = 0 ; x < (long)imageWidth() ; ++x)
                        {
                            // Blue
                            dst16[0] = (unsigned short)(scale[2] * jas_matrix_getv(pixels[2], x / x_step[2]));
                            // Green
                            dst16[1] = (unsigned short)(scale[1] * jas_matrix_getv(pixels[1], x / x_step[1]));
                            // Red
                            dst16[2] = (unsigned short)(scale[0] * jas_matrix_getv(pixels[0], x / x_step[0]));
                            // Alpha
                            dst16[3] = 0xFFFF;

                            dst16 += 4;
                        }
                    }

                    break;
                }

                case 4: // RGBA.
                {
                    if (!m_sixteenBit)   // 8 bits image.
                    {
                        for (x = 0 ; x < (long)imageWidth() ; ++x)
                        {
                            // Blue
                            dst[0] = (uchar)(scale[2] * jas_matrix_getv(pixels[2], x / x_step[2]));
                            // Green
                            dst[1] = (uchar)(scale[1] * jas_matrix_getv(pixels[1], x / x_step[1]));
                            // Red
                            dst[2] = (uchar)(scale[0] * jas_matrix_getv(pixels[0], x / x_step[0]));
                            // Alpha
                            dst[3] = (uchar)(scale[3] * jas_matrix_getv(pixels[3], x / x_step[3]));

                            dst += 4;
                        }
                    }
                    else                // 16 bits image.
                    {
                        for (x = 0 ; x < (long)imageWidth() ; ++x)
                        {
                            // Blue
                            dst16[0] = (unsigned short)(scale[2] * jas_matrix_getv(pixels[2], x / x_step[2]));
                            // Green
                            dst16[1] = (unsigned short)(scale[1] * jas_matrix_getv(pixels[1], x / x_step[1]));
                            // Red
                            dst16[2] = (unsigned short)(scale[0] * jas_matrix_getv(pixels[0], x / x_step[0]));
                            // Alpha
                            dst16[3] = (unsigned short)(scale[3] * jas_matrix_getv(pixels[3], x / x_step[3]));

                            dst16 += 4;
                        }
                    }

                    break;
                }
            }

            // use 0-10% and 90-100% for pseudo-progress

            if (observer && y >= (long)checkPoint)
            {
                checkPoint += granularity(observer, y, 0.8F);

                if (!observer->continueQuery())
                {
                    jas_image_destroy(jp2_image);

                    for (i = 0 ; i < (long)number_components ; ++i)
                    {
                        jas_matrix_destroy(pixels[i]);
                    }

                    jas_cleanup();

                    loadingFailed();

                    return false;
                }

                observer->progressInfo(0.1F + (0.8F * (((float)y) / ((float)imageHeight()))));
            }
        }
    }

    // -------------------------------------------------------------------
    // Get ICC color profile.

    if (m_loadFlags & LoadICCData)
    {
        jas_iccprof_t* icc_profile = nullptr;
        jas_stream_t*  icc_stream  = nullptr;
        jas_cmprof_t*  cm_profile  = nullptr;

        // To prevent cppcheck warnings.
        (void)icc_profile;
        (void)icc_stream;
        (void)cm_profile;

        cm_profile = jas_image_cmprof(jp2_image);

        if (cm_profile != nullptr)
        {
            icc_profile = jas_iccprof_createfromcmprof(cm_profile);
        }

        if (icc_profile != nullptr)
        {
            icc_stream = jas_stream_memopen(nullptr, 0);

            if (icc_stream != nullptr)
            {
                if (jas_iccprof_save(icc_profile, icc_stream) == 0)
                {
                    if (jas_stream_flush(icc_stream) == 0)
                    {
                        jas_stream_memobj_t* const blob = (jas_stream_memobj_t*) icc_stream->obj_;
                        QByteArray profile_rawdata;
                        profile_rawdata.resize(blob->len_);
                        memcpy(profile_rawdata.data(), blob->buf_, blob->len_);
                        imageSetIccProfile(IccProfile(profile_rawdata));
                        jas_stream_close(icc_stream);
                    }
                }
            }
        }
        else
        {
            // If ICC profile is null, check Exif metadata.

            checkExifWorkingColorSpace();
        }
    }

    if (observer)
    {
        observer->progressInfo(1.0F);
    }

    imageData() = data.take();
    imageSetAttribute(QLatin1String("format"),             QLatin1String("JP2"));
    imageSetAttribute(QLatin1String("originalColorModel"), colorModel);
    imageSetAttribute(QLatin1String("originalBitDepth"),   maximum_component_depth);
    imageSetAttribute(QLatin1String("originalSize"),       QSize(imageWidth(), imageHeight()));

    jas_image_destroy(jp2_image);

    for (i = 0 ; i < (long)number_components ; ++i)
    {
        jas_matrix_destroy(pixels[i]);
    }

    jas_cleanup();

    return true;
}

} // namespace DigikamJPEG2000DImgPlugin
