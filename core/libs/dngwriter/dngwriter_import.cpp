/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-25
 * Description : a tool to convert RAW file to DNG - Raw data import.
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

int DNGWriter::Private::importRaw(DRawInfo* const identify,
                                  DRawInfo* const identifyMake)
{
    if (parent->inputFile().isEmpty())
    {
        qCCritical(DIGIKAM_GENERAL_LOG) << "DNGWriter: No input file to convert. Aborted...";

        return PROCESS_FAILED;
    }

    inputInfo   = QFileInfo(parent->inputFile());
    dngFilePath = parent->outputFile();

    if (dngFilePath.isEmpty())
    {
        dngFilePath = QString(inputInfo.completeBaseName() + QLatin1String(".dng"));
    }

    outputInfo  = QFileInfo(dngFilePath);

    // -----------------------------------------------------------------------------------------

    qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Loading RAW data from " << inputInfo.fileName();

    QPointer<DRawDecoder> rawProcessor(new DRawDecoder);

    if (!rawProcessor->rawFileIdentify(*identifyMake, parent->inputFile()))
    {
        qCCritical(DIGIKAM_GENERAL_LOG) << "DNGWriter: Reading RAW file failed. Aborted...";

        return PROCESS_FAILED;
    }

    // -----------------------------------------------------------------------------------------

    if (identifyMake->make.toUpper() == QLatin1String("FUJIFILM"))
    {
        ExifToolParser* const parser = new ExifToolParser(nullptr);

        if (parser->exifToolAvailable())
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Pre-process metadata with ExifTool";

            bool ret = parser->load(inputInfo.filePath());

            if (!ret)
            {
                qCCritical(DIGIKAM_GENERAL_LOG) << "DNGWriter: Pre-process metadata with ExifTool failed. Aborted...";
                delete parser;

                return PROCESS_FAILED;
            }

            ExifToolParser::ExifToolData parsed       = parser->currentData();
            ExifToolParser::ExifToolData::iterator it = parsed.find(QLatin1String("RAF.RAF2.Image.RawImageCropTopLeft"));

            if (it != parsed.end())
            {
                QString data             = it.value()[0].toString();
                identifyMake->topMargin  = data.section(QLatin1Char(' '), 0, 0).toInt();
                identifyMake->leftMargin = data.section(QLatin1Char(' '), 1, 1).toInt();
            }

            it = parsed.find(QLatin1String("RAF.RAF2.Image.RawImageFullSize"));

            if (it != parsed.end())
            {
                QString data             = it.value()[0].toString();
                identifyMake->fullSize.setWidth(data.section(QLatin1Char(' '), 0, 0).toInt());
                identifyMake->fullSize.setHeight(data.section(QLatin1Char(' '), 1, 1).toInt());
            }

            it = parsed.find(QLatin1String("RAF.RAF2.Image.RawImageCroppedSize"));

            if (it != parsed.end())
            {
                QString data               = it.value()[0].toString();
                int cWidth                 = data.section(QLatin1Char(' '), 0, 0).toInt();
                int cHeight                = data.section(QLatin1Char(' '), 1, 1).toInt();
                identifyMake->bottomMargin = identifyMake->fullSize.height() - identifyMake->topMargin  - cHeight;
                identifyMake->rightMargin  = identifyMake->fullSize.width()  - identifyMake->leftMargin - cWidth;
            }
        }
        else
        {
            qCWarning(DIGIKAM_GENERAL_LOG) << "DNGWriter: ExifTool is not available to pre-process metadata...";
        }

        delete parser;
    }

    // -----------------------------------------------------------------------------------------

    qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Identification:";
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- Date:          " << identifyMake->dateTime.toString(Qt::ISODate);
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- Make:          " << identifyMake->make;
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- Model:         " << identifyMake->model;
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- ImageSize:     " << identifyMake->imageSize.width()  << "x" << identifyMake->imageSize.height();
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- FullSize:      " << identifyMake->fullSize.width()   << "x" << identifyMake->fullSize.height();
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- OutputSize:    " << identifyMake->outputSize.width() << "x" << identifyMake->outputSize.height();
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- Orientation:   " << identifyMake->orientation;
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- Top margin:    " << identifyMake->topMargin;
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- Left margin:   " << identifyMake->leftMargin;
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- Bottom margin: " << identifyMake->bottomMargin;
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- Right margin:  " << identifyMake->rightMargin;

    if (identifyMake->cropArea == QRect(65535, 65535, 0, 0))
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Crop Area:     " << "none";
    }
    else
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Crop Area:     " << identifyMake->cropArea;
    }

    // TODO: need to get correct default crop size to avoid artifacts at the borders

    if ((identifyMake->orientation == 5) || (identifyMake->orientation == 6))
    {
        outputHeight = identifyMake->outputSize.width();
        outputWidth  = identifyMake->outputSize.height();
    }
    else
    {
        outputHeight = identifyMake->outputSize.height();
        outputWidth  = identifyMake->outputSize.width();
    }

    if (!rawProcessor->extractRAWDataUnprocessed(parent->inputFile(), rawData, *identify, 0))
    {
        qCCritical(DIGIKAM_GENERAL_LOG) << "DNGWriter: Loading RAW data failed. Aborted...";

        return FILE_NOT_SUPPORTED;
    }

    qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Raw data loaded:";
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- Data Size:     " << rawData.size() << "bytes";
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- Filter:        " << identify->filterPattern;
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- Colors:        " << identify->rawColors;
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- Black:         " << identify->blackPoint;
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- White:         " << identify->whitePoint;
    qCDebug(DIGIKAM_GENERAL_LOG) << "--- CAM->XYZ:";

    for (int i = 0 ; i < 4 ; ++i)
    {
        qCDebug(DIGIKAM_GENERAL_LOG)
                 << "                   "
                 << QString().asprintf("%03.4f  %03.4f  %03.4f", identify->cameraXYZMatrix[i][0],
                                                                 identify->cameraXYZMatrix[i][1],
                                                                 identify->cameraXYZMatrix[i][2]);
    }

    if (cancel)
    {
        return PROCESS_CANCELED;
    }

    return PROCESS_CONTINUE;
}

} // namespace Digikam
