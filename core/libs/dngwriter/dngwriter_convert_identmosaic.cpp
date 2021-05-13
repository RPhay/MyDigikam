/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-25
 * Description : a tool to convert RAW file to DNG
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

namespace Digikam
{

int DNGWriter::mosaicIdentStage()
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Raw data loaded:" ;
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- Data Size:     " << d->rawData.size() << " bytes";
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- Date:          " << d->identify->dateTime.toString(Qt::ISODate);
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- Make:          " << d->identify->make;
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- Model:         " << d->identify->model;
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- ImageSize:     " << d->identify->imageSize.width()  << "x" << d->identify->imageSize.height();
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- FullSize:      " << d->identify->fullSize.width()   << "x" << d->identify->fullSize.height();
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- OutputSize:    " << d->identify->outputSize.width() << "x" << d->identify->outputSize.height();
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- Orientation:   " << d->identify->orientation;
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- Top margin:    " << d->identify->topMargin;
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- Left margin:   " << d->identify->leftMargin;
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- Filter:        " << d->identify->filterPattern;
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- Colors:        " << d->identify->rawColors;
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- Black:         " << d->identify->blackPoint;
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- White:         " << d->identify->whitePoint;
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- CAM->XYZ:" ;

    QString matrixVal;

    for (int i = 0 ; i < 4 ; ++i)
    {
        qCDebug(DIGIKAM_GENERAL_LOG)
                    << "                   "
                    << QString().asprintf("%03.4f  %03.4f  %03.4f", d->identify->cameraXYZMatrix[i][0],
                                                                    d->identify->cameraXYZMatrix[i][1],
                                                                    d->identify->cameraXYZMatrix[i][2]);
    }

    // Check if CFA layout is supported by DNG SDK.

    bool fujiRotate90 = false;

    // Standard bayer layouts

    if      (d->identify->filterPattern == QLatin1String("GRBGGRBGGRBGGRBG"))
    {
        d->bayerPattern = Private::Standard;
        d->filter       = 0;
    }
    else if (d->identify->filterPattern == QLatin1String("RGGBRGGBRGGBRGGB"))
    {
        d->bayerPattern = Private::Standard;
        d->filter       = 1;
    }
    else if (d->identify->filterPattern == QLatin1String("BGGRBGGRBGGRBGGR"))
    {
        d->bayerPattern = Private::Standard;
        d->filter       = 2;
    }
    else if (d->identify->filterPattern == QLatin1String("GBRGGBRGGBRGGBRG"))
    {
        d->bayerPattern = Private::Standard;
        d->filter       = 3;
    }
    else if ((d->identify->filterPattern      == QLatin1String("RGBGRGBGRGBGRGBG")) &&
             (d->identifyMake->make.toUpper() == QLatin1String("FUJIFILM")))
    {
        // Fuji layouts

        d->bayerPattern = Private::Fuji;
        fujiRotate90    = false;
        d->filter       = 0;
    }
    else if ((d->identify->filterPattern      == QLatin1String("RBGGBRGGRBGGBRGG")) &&
             (d->identifyMake->make.toUpper() == QLatin1String("FUJIFILM")))
    {
        // Fuji layouts

        d->bayerPattern = Private::Fuji;
        fujiRotate90    = true;
        d->filter       = 0;
    }
    else if ((d->identify->filterPattern      == QLatin1String("GGGGBRGGGGRBGGGG")) &&
             (d->identifyMake->make.toUpper() == QLatin1String("FUJIFILM")))
    {
        // Fuji layouts

        d->bayerPattern = Private::Fuji6x6;
        fujiRotate90    = false;
        d->filter       = 1;
    }
    else if ((d->identify->rawColors == 3)                 &&
             (d->identify->filterPattern.isEmpty())     &&
/*
             (identify->filterPattern == QString(""))   &&
*/
             ((uint32)d->rawData.size() == d->identify->outputSize.width() * d->identify->outputSize.height() * 3 * sizeof(uint16)))
    {
        d->bayerPattern = Private::LinearRaw;
    }
    else if (d->identify->rawColors == 4)           // Four color sensors
    {
        d->bayerPattern = Private::FourColor;

        if (d->identify->filterPattern.length() != 16)
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Bayer mosaic not supported. Aborted...";
            d->clearMemory();

            return FILE_NOT_SUPPORTED;
        }

        for (int i = 0 ; i < 16 ; ++i)
        {
            d->filter = d->filter >> 2;

            if      (d->identify->filterPattern[i] == QLatin1Char('G'))
            {
                d->filter |= 0x00000000;
            }
            else if (d->identify->filterPattern[i] == QLatin1Char('M'))
            {
                d->filter |= 0x40000000;
            }
            else if (d->identify->filterPattern[i] == QLatin1Char('C'))
            {
                d->filter |= 0x80000000;
            }
            else if (d->identify->filterPattern[i] == QLatin1Char('Y'))
            {
                d->filter |= 0xC0000000;
            }
            else
            {
                qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Bayer mosaic not supported. Aborted...";
                d->clearMemory();

                return FILE_NOT_SUPPORTED;
            }
        }
    }
    else
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Bayer mosaic not supported. Aborted...";
        d->clearMemory();

        return FILE_NOT_SUPPORTED;
    }

    if (fujiRotate90)
    {
        if (!d->fujiRotate())
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "Can not rotate fuji image. Aborted...";
            d->clearMemory();

            return PROCESS_FAILED;
        }

        int tmp         = d->outputWidth;
        d->outputWidth  = d->outputHeight;
        d->outputHeight = tmp;
    }

    d->activeArea   = dng_rect(d->identify->outputSize.height(), d->identify->outputSize.width());
    d->activeWidth  = d->identify->outputSize.width();
    d->activeHeight = d->identify->outputSize.height();

    // Check if number of Raw Color components is supported.

    if ((d->identify->rawColors != 3) && (d->identify->rawColors != 4))
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Number of Raw color components not supported. Aborted...";
        d->clearMemory();

        return PROCESS_FAILED;
    }

    d->width  = d->identify->outputSize.width();
    d->height = d->identify->outputSize.height();

/*
    // NOTE: code to hack RAW data extraction

    QString   rawdataFilePath(d->inputInfo.completeBaseName() + QString(".dat"));
    QFileInfo rawdataInfo(rawdataFilePath);

    QFile rawdataFile(rawdataFilePath);

    if (!rawdataFile.open(QIODevice::WriteOnly))
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Cannot open file to write RAW data. Aborted...";
        d->clearMemory();
        return PROCESS_FAILED;
    }

    QDataStream rawdataStream(&rawdataFile);
    rawdataStream.writeRawData(d->rawData.data(), d->rawData.size());
    rawdataFile.close();
*/
    // -----------------------------------------------------------------------------------------

    if (d->cancel)
    {
        d->clearMemory();
        return PROCESS_CANCELED;
    }

    return PROCESS_CONTINUE;
}

} // namespace Digikam
