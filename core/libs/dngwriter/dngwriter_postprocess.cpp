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
    ExifToolParser* const parser = new ExifToolParser(nullptr);

    if (parser->exifToolAvailable())
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Post-process Iptc and Xmp with ExifTool";

        bool ret = parser->copyTags(
                                     inputInfo.filePath(),
                                     dngFilePath,
                                     ExifToolProcess::COPY_IPTC         |
                                     ExifToolProcess::COPY_XMP,
                                     ExifToolProcess::CREATE_NEW_TAGS   |
                                     ExifToolProcess::CREATE_NEW_GROUPS
                                   );

        if (!ret)
        {
            qCCritical(DIGIKAM_GENERAL_LOG) << "DNGWriter: Copy Iptc and Xmp tags with ExifTool failed. Aborted...";
            delete parser;

            return PROCESS_FAILED;
        }

        // ---

        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Post-process markernotes with ExifTool";

        ret     = parser->copyTags(
                                     inputInfo.filePath(),
                                     dngFilePath,
                                     ExifToolProcess::COPY_MAKERNOTES
                                   );

        if (!ret)
        {
            qCCritical(DIGIKAM_GENERAL_LOG) << "DNGWriter: Copy markernotes with ExifTool failed. Aborted...";
            delete parser;

            return PROCESS_FAILED;
        }

        // ---

        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Populate Xmp with ExifTool";

        ret      = parser->translateTags(
                                          dngFilePath,
                                          ExifToolProcess::TRANS_ALL_XMP
                                        );

        if (!ret)
        {
            qCCritical(DIGIKAM_GENERAL_LOG) << "DNGWriter: Populate Xmp tags with ExifTool failed. Aborted...";
            delete parser;

            return PROCESS_FAILED;
        }

        // ---

        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Populate Iptc with ExifTool";

        ret      = parser->translateTags(
                                          dngFilePath,
                                          ExifToolProcess::TRANS_ALL_IPTC
                                        );

        if (!ret)
        {
            qCCritical(DIGIKAM_GENERAL_LOG) << "DNGWriter: Populate Iptc tags with ExifTool failed. Aborted...";
            delete parser;

            return PROCESS_FAILED;
        }
    }
    else
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "DNGWriter: ExifTool is not available to post-process metadata...";
    }

    delete parser;

    return PROCESS_CONTINUE;
}

} // namespace Digikam
