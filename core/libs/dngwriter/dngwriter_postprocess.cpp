/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-25
 * Description : a tool to convert RAW file to DNG - Post process with ExifTool.
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

int DNGWriter::Private::exifToolPostProcess()
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Post-process metadata with ExifTool";

    ExifToolParser etParser;

    if (etParser.exifToolAvailable())
    {
        bool ret = etParser.copyTags(
                                     inputInfo.filePath(),
                                     dngFilePath,
                                     ExifToolProcess::COPY_MAKERNOTES |
                                     ExifToolProcess::COPY_IPTC       |
                                     ExifToolProcess::COPY_XMP
                                    );

        if (!ret)
        {
            qCCritical(DIGIKAM_GENERAL_LOG) << "DNGWriter: Copy tags with ExifTool failed. Aborted...";

            return PROCESS_FAILED;
        }
    }
    else
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "DNGWriter: ExifTool is not available to post-process metadata...";
    }

    return PROCESS_CONTINUE;
}

} // namespace Digikam
