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

int DNGWriter::extractRawStage()
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

    d->outputInfo   = QFileInfo(d->dngFilePath);
    d->identify     = new DRawInfo;
    d->identifyMake = new DRawInfo;

    // -----------------------------------------------------------------------------------------

    qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Loading RAW data from " << d->inputInfo.fileName();

    QPointer<DRawDecoder> rawProcessor(new DRawDecoder);

    if (!rawProcessor->rawFileIdentify(*d->identifyMake, inputFile()))
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Reading RAW file failed. Aborted...";
        d->clearMemory();

        return PROCESS_FAILED;
    }

    // TODO: need to get correct default crop size to avoid artifacts at the borders

    if ((d->identifyMake->orientation == 5) || (d->identifyMake->orientation == 6))
    {
        d->outputHeight = d->identifyMake->outputSize.width();
        d->outputWidth  = d->identifyMake->outputSize.height();
    }
    else
    {
        d->outputHeight = d->identifyMake->outputSize.height();
        d->outputWidth  = d->identifyMake->outputSize.width();
    }

    if (!rawProcessor->extractRAWData(inputFile(), d->rawData, *d->identify, 0))
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Loading RAW data failed. Aborted...";
        d->clearMemory();

        return FILE_NOT_SUPPORTED;
    }

    if (d->cancel)
    {
        d->clearMemory();
        return PROCESS_CANCELED;
    }

    return PROCESS_CONTINUE;
}

} // namespace Digikam
