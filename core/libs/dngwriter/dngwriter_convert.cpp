/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-25
 * Description : a tool to convert RAW file to DNG - Run Convertion stages.
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010-2011 by Jens Mueller <tschenser at gmx dot de>
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

#include "dngwriter_p.h"

// Local includes

#include "dngwriterhost.h"

namespace Digikam
{

int DNGWriter::convert()
{
    int ret;
    d->cancel = false;

    try
    {
        if (inputFile().isEmpty())
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: No input file to convert. Aborted...";
            return PROCESS_FAILED;
        }

        d->inputInfo    = QFileInfo(inputFile());
        d->dngFilePath  = outputFile();

        if (d->dngFilePath.isEmpty())
        {
            d->dngFilePath = QString(d->inputInfo.completeBaseName() + QLatin1String(".dng"));
        }

        d->outputInfo    = QFileInfo(d->dngFilePath);
        QByteArray rawData;
        QScopedPointer<DRawInfo> identify(new DRawInfo);
        QScopedPointer<DRawInfo> identifyMake(new DRawInfo);

        // -----------------------------------------------------------------------------------------

        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Loading RAW data from " << d->inputInfo.fileName();

        QPointer<DRawDecoder> rawProcessor(new DRawDecoder);

        if (!rawProcessor->rawFileIdentify(*identifyMake.data(), inputFile()))
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Reading RAW file failed. Aborted...";

            return PROCESS_FAILED;
        }

        // TODO: need to get correct default crop size to avoid artifacts at the borders

        if ((identifyMake->orientation == 5) || (identifyMake->orientation == 6))
        {
            d->outputHeight = identifyMake->outputSize.width();
            d->outputWidth  = identifyMake->outputSize.height();
        }
        else
        {
            d->outputHeight = identifyMake->outputSize.height();
            d->outputWidth  = identifyMake->outputSize.width();
        }

        if (!rawProcessor->extractRAWData(inputFile(), rawData, *identify, 0))
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Loading RAW data failed. Aborted..." ;

            return FILE_NOT_SUPPORTED;
        }

        if (d->cancel)
        {
            return PROCESS_CANCELED;
        }

        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Raw data loaded:" ;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Data Size:     " << rawData.size() << " bytes";
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Date:          " << identify->dateTime.toString(Qt::ISODate);
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Make:          " << identify->make;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Model:         " << identify->model;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- ImageSize:     " << identify->imageSize.width()  << "x" << identify->imageSize.height();
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- FullSize:      " << identify->fullSize.width()   << "x" << identify->fullSize.height();
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- OutputSize:    " << identify->outputSize.width() << "x" << identify->outputSize.height();
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Orientation:   " << identify->orientation;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Top margin:    " << identify->topMargin;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Left margin:   " << identify->leftMargin;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Filter:        " << identify->filterPattern;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Colors:        " << identify->rawColors;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Black:         " << identify->blackPoint;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- White:         " << identify->whitePoint;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- CAM->XYZ:" ;

        QString matrixVal;

        for (int i = 0 ; i < 4 ; ++i)
        {
            qCDebug(DIGIKAM_GENERAL_LOG)
                     << "                   "
                     << QString().asprintf("%03.4f  %03.4f  %03.4f", identify->cameraXYZMatrix[i][0],
                                                                     identify->cameraXYZMatrix[i][1],
                                                                     identify->cameraXYZMatrix[i][2]);
        }

        // Check if CFA layout is supported by DNG SDK.

        bool fujiRotate90 = false;

        // Standard bayer layouts

        if      (identify->filterPattern == QLatin1String("GRBGGRBGGRBGGRBG"))
        {
            d->bayerPattern = Private::Standard;
            d->filter       = 0;
        }
        else if (identify->filterPattern == QLatin1String("RGGBRGGBRGGBRGGB"))
        {
            d->bayerPattern = Private::Standard;
            d->filter       = 1;
        }
        else if (identify->filterPattern == QLatin1String("BGGRBGGRBGGRBGGR"))
        {
            d->bayerPattern = Private::Standard;
            d->filter       = 2;
        }
        else if (identify->filterPattern == QLatin1String("GBRGGBRGGBRGGBRG"))
        {
            d->bayerPattern = Private::Standard;
            d->filter       = 3;
        }
        else if ((identify->filterPattern      == QLatin1String("RGBGRGBGRGBGRGBG")) &&
                 (identifyMake->make.toUpper() == QLatin1String("FUJIFILM")))
        {
            // Fuji layouts

            d->bayerPattern = Private::Fuji;
            fujiRotate90 = false;
            d->filter       = 0;
        }
        else if ((identify->filterPattern      == QLatin1String("RBGGBRGGRBGGBRGG")) &&
                 (identifyMake->make.toUpper() == QLatin1String("FUJIFILM")))
        {
            // Fuji layouts

            d->bayerPattern = Private::Fuji;
            fujiRotate90 = true;
            d->filter       = 0;
        }
        else if ((identify->filterPattern      == QLatin1String("GGGGBRGGGGRBGGGG")) &&
                 (identifyMake->make.toUpper() == QLatin1String("FUJIFILM")))
        {
            // Fuji layouts

            d->bayerPattern = Private::Fuji6x6;
            fujiRotate90 = false;
            d->filter       = 1;
        }
        else if ((identify->rawColors == 3)                 &&
                 (identify->filterPattern.isEmpty())        &&
/*
                 (identify->filterPattern == QString(""))   &&
*/
                 ((uint32)rawData.size() == identify->outputSize.width() * identify->outputSize.height() * 3 * sizeof(uint16)))
        {
            d->bayerPattern = Private::LinearRaw;
        }
        else if (identify->rawColors == 4)           // Four color sensors
        {
            d->bayerPattern = Private::FourColor;

            if (identify->filterPattern.length() != 16)
            {
                qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Bayer mosaic not supported. Aborted..." ;

                return FILE_NOT_SUPPORTED;
            }

            for (int i = 0 ; i < 16 ; ++i)
            {
                d->filter = d->filter >> 2;

                if      (identify->filterPattern[i] == QLatin1Char('G'))
                {
                    d->filter |= 0x00000000;
                }
                else if (identify->filterPattern[i] == QLatin1Char('M'))
                {
                    d->filter |= 0x40000000;
                }
                else if (identify->filterPattern[i] == QLatin1Char('C'))
                {
                    d->filter |= 0x80000000;
                }
                else if (identify->filterPattern[i] == QLatin1Char('Y'))
                {
                    d->filter |= 0xC0000000;
                }
                else
                {
                    qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Bayer mosaic not supported. Aborted..." ;

                    return FILE_NOT_SUPPORTED;
                }
            }
        }
        else
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Bayer mosaic not supported. Aborted..." ;

            return FILE_NOT_SUPPORTED;
        }

        if (fujiRotate90)
        {
            if (!d->fujiRotate(rawData, *identify))
            {
                qCDebug(DIGIKAM_GENERAL_LOG) << "Can not rotate fuji image. Aborted...";

                return PROCESS_FAILED;
            }

            int tmp         = d->outputWidth;
            d->outputWidth  = d->outputHeight;
            d->outputHeight = tmp;
        }

        d->activeArea   = dng_rect(identify->outputSize.height(), identify->outputSize.width());
        d->activeWidth  = identify->outputSize.width();
        d->activeHeight = identify->outputSize.height();

        // Check if number of Raw Color components is supported.

        if ((identify->rawColors != 3) && (identify->rawColors != 4))
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Number of Raw color components not supported. Aborted..." ;

            return PROCESS_FAILED;
        }

        d->width  = identify->outputSize.width();
        d->height = identify->outputSize.height();
/*
        debugExtractedRAWData(rawData);
*/
        // -----------------------------------------------------------------------------------------

        if (d->cancel)
        {
            return PROCESS_CANCELED;
        }

        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG memory allocation and initialization" ;

        dng_memory_allocator memalloc(gDefaultDNGMemoryAllocator);

        dng_rect rect(d->height, d->width);
        DNGWriterHost host(d, &memalloc);

        host.SetSaveDNGVersion(dngVersion_SaveDefault);
        host.SetSaveLinearDNG(false);
        host.SetKeepOriginalFile(true);

        AutoPtr<dng_image> image(new dng_simple_image(rect, (d->bayerPattern == Private::LinearRaw) ? 3 : 1, ttShort, memalloc));

        if (d->cancel)
        {
            return PROCESS_CANCELED;
        }

        // -----------------------------------------------------------------------------------------

        dng_pixel_buffer buffer;

        buffer.fArea       = rect;
        buffer.fPlane      = 0;
        buffer.fPlanes     = (d->bayerPattern == Private::LinearRaw) ? 3 : 1;
        buffer.fRowStep    = buffer.fPlanes * d->width;
        buffer.fColStep    = buffer.fPlanes;
        buffer.fPlaneStep  = 1;
        buffer.fPixelType  = ttShort;
        buffer.fPixelSize  = TagTypeSize(ttShort);
        buffer.fData       = rawData.data();
        image->Put(buffer);

        if (d->cancel)
        {
            return PROCESS_CANCELED;
        }

        // -----------------------------------------------------------------------------------------

        AutoPtr<dng_negative> negative(host.Make_dng_negative());

        ret = d->createNegative(negative, identify.get());

        if (ret != PROCESS_CONTINUE)
        {
            return ret;
        }

        // -----------------------------------------------------------------------------------------

        ret = d->storeMetadata(host, negative, identify.get(), identifyMake.get());

        if (ret != PROCESS_CONTINUE)
        {
            return ret;
        }

        // -----------------------------------------------------------------------------------------

        ret = d->backupRaw(host, negative);

        if (ret != PROCESS_CONTINUE)
        {
            return ret;
        }

        // -----------------------------------------------------------------------------------------

        ret = d->exportTarget(host, negative, image);

        if (ret != PROCESS_CONTINUE)
        {
            return ret;
        }
    }

    catch (const dng_exception& exception)
    {
        int ret = exception.ErrorCode();
        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG SDK exception code (" << ret << "):" << d->dngErrorCodeToString(ret);

        return DNG_SDK_INTERNAL_ERROR;
    }

    catch (...)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG SDK exception code unknow";

        return DNG_SDK_INTERNAL_ERROR;
    }

    qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG conversion complete...";

    return PROCESS_COMPLETE;
}

} // namespace Digikam
