/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-26
 * Description : item metadata interface - libheif helpers.
 *
 * Copyright (C) 2020-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "dmetadata.h"

// Qt includes

#include <QFile>
#include <QVariant>
#include <QByteArray>
#include <QTextStream>
#include <QDataStream>
#include <QFileInfo>
#include <qplatformdefs.h>

// Local includes

#include "digikam_config.h"
#include "digikam_debug.h"
#include "heif.h"

namespace Digikam
{

bool s_isHeifSuccess(struct heif_error* const error)
{
    if (error->code == 0)
    {
        return true;
    }

    qCWarning(DIGIKAM_METAENGINE_LOG) << "Error while processing HEIF image:" << error->message;

    return false;
}

void s_readHEICMetadata(struct heif_context* const heif_context, heif_item_id image_id,
                        QByteArray& exif, QByteArray& iptc, QByteArray& xmp)
{
    struct heif_image_handle* image_handle = nullptr;
    struct heif_error error1               = heif_context_get_image_handle(heif_context,
                                                                           image_id,
                                                                           &image_handle);

    if (!s_isHeifSuccess(&error1))
    {
        heif_image_handle_release(image_handle);

        return;
    }

    heif_item_id dataIds[10];

    int count = heif_image_handle_get_list_of_metadata_block_IDs(image_handle,
                                                                 nullptr,
                                                                 dataIds,
                                                                 10);

    qDebug(DIGIKAM_METAENGINE_LOG) << "Found" << count << "HEIF metadata chunck";

    if (count > 0)
    {
        for (int i = 0 ; i < count ; ++i)
        {
            qDebug(DIGIKAM_METAENGINE_LOG) << "Parsing HEIF metadata chunck:" << heif_image_handle_get_metadata_type(image_handle, dataIds[i]);

            if (QLatin1String(heif_image_handle_get_metadata_type(image_handle, dataIds[i])) == QLatin1String("Exif"))
            {
                // Read Exif chunk.

                size_t length = heif_image_handle_get_metadata_size(image_handle, dataIds[i]);

                QByteArray exifChunk;
                exifChunk.resize((int)length);

                struct heif_error error2 = heif_image_handle_get_metadata(image_handle,
                                                                          dataIds[i],
                                                                          exifChunk.data());

                if ((error2.code == 0) && (length > 4))
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

                        qDebug(DIGIKAM_METAENGINE_LOG) << "HEIF exif container found with size:" << length - skip;
                        exif.append((char*)(exifChunk.data() + skip), exifChunk.size() - skip);
                    }
                }
            }

            if (QLatin1String(heif_image_handle_get_metadata_type(image_handle, dataIds[i])) == QLatin1String("iptc"))
            {
                // Read Iptc chunk.

                size_t length = heif_image_handle_get_metadata_size(image_handle, dataIds[i]);

                iptc.resize((int)length);

                struct heif_error error3 = heif_image_handle_get_metadata(image_handle,
                                                                          dataIds[i],
                                                                          iptc.data());

                if (error3.code == 0)
                {
                    qDebug(DIGIKAM_METAENGINE_LOG) << "HEIF iptc container found with size:" << length;
                }
                else
                {
                    iptc = QByteArray();
                }
            }

            if (
                (QLatin1String(heif_image_handle_get_metadata_type(image_handle, dataIds[i]))         == QLatin1String("mime")) &&
                (QLatin1String(heif_image_handle_get_metadata_content_type(image_handle, dataIds[i])) == QLatin1String("application/rdf+xml"))
               )
            {
                // Read Xmp chunk.

                size_t length = heif_image_handle_get_metadata_size(image_handle, dataIds[i]);

                xmp.resize((int)length);

                struct heif_error error4 = heif_image_handle_get_metadata(image_handle,
                                                                          dataIds[i],
                                                                          xmp.data());

                if (error4.code == 0)
                {
                    qDebug(DIGIKAM_METAENGINE_LOG) << "HEIF xmp container found with size:" << length;
                }
                else
                {
                    xmp = QByteArray();
                }
            }
        }
    }

    heif_image_handle_release(image_handle);
}

bool DMetadata::loadUsingLibheif(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    QString ext = fileInfo.suffix().toUpper();

    if (
        !fileInfo.exists() || ext.isEmpty() ||
        (ext != QLatin1String("HEIF"))      ||
        (ext != QLatin1String("HEIC"))
       )
    {
        return false;
    }

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
        qCWarning(DIGIKAM_METAENGINE_LOG) << "Error: Could not open source file.";

        return false;
    }

    const int headerLen = 12;

    unsigned char header[headerLen];

    if (fread(&header, headerLen, 1, file) != 1)
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "Error: Could not parse magic identifier.";
        fclose(file);

        return false;
    }

    if ((memcmp(&header[4], "ftyp", 4) != 0) &&
        (memcmp(&header[8], "heic", 4) != 0) &&
        (memcmp(&header[8], "heix", 4) != 0) &&
        (memcmp(&header[8], "mif1", 4) != 0))
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "Error: source file is not HEIF image.";
        fclose(file);

        return false;
    }

    fclose(file);

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
            qCWarning(DIGIKAM_METAENGINE_LOG) << "Error: Could not load into memory.";
            heif_context_free(heif_context);

            return false;
        }

        buffer = memFile.readAll();
        memFile.close();

        qDebug(DIGIKAM_METAENGINE_LOG) << "HEIF loading file to memory buffer";

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

    if (!s_isHeifSuccess(&error))
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "Error: Could not read source file.";
        heif_context_free(heif_context);

        return false;
    }

    error = heif_context_get_primary_image_ID(heif_context, &primary_image_id);

    if (!s_isHeifSuccess(&error))
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "Error: Could not load image data.";
        heif_context_free(heif_context);

        return false;
    }

    QByteArray exif;
    QByteArray iptc;
    QByteArray xmp;
    bool ret = false;

    s_readHEICMetadata(heif_context, primary_image_id, exif, iptc, xmp);

    if (!exif.isEmpty() || !iptc.isEmpty() || !xmp.isEmpty())
    {
        if (!exif.isEmpty())
        {
            setExif(exif);
        }

        if (!iptc.isEmpty())
        {
            setIptc(iptc);
        }

        if (!xmp.isEmpty())
        {
            setXmp(xmp);
        }

        ret = true;
    }

    heif_context_free(heif_context);

    return ret;
}

} // namespace Digikam
