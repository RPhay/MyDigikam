/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-02-23
 * Description : item metadata interface - Exiftool helpers.
 *
 * Copyright (C) 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2011      by Leif Huhn <leif at dkstat dot com>
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

#include <QString>
#include <QScopedPointer>
#include <QFileInfo>

// Local includes

#include "digikam_config.h"
#include "digikam_debug.h"
#include "drawfiles.h"
#include "exiftoolparser.h"

namespace Digikam
{

bool DMetadata::loadUsingExifTool(const QString& filePath)
{
    ExifToolParser* const parser = new ExifToolParser(nullptr);

    if (parser->exifToolAvailable())
    {
        bool ret = parser->loadChunk(filePath);

        if (!ret)
        {
            qCCritical(DIGIKAM_METAENGINE_LOG) << "Load metadata using ExifTool failed...";
            delete parser;

            return false;
        }

        ExifToolParser::ExifToolData chunk = parser->currentData();

        qCDebug(DIGIKAM_METAENGINE_LOG) << "Metadata chunk loaded with ExifTool";

        ExifToolParser::ExifToolData::iterator it = chunk.find(QLatin1String("EXV"));

        if (it == chunk.end())
        {
            qCWarning(DIGIKAM_METAENGINE_LOG) << "Metadata chunk loaded with ExifTool is empty";
            delete parser;

            return false;
        }

        QVariantList varLst = it.value();
        QByteArray exv      = varLst[0].toByteArray();

        if (!exv.isEmpty())
        {
            qCDebug(DIGIKAM_METAENGINE_LOG) << "EXV chunk size loaded with ExifTool:" << exv.size();
            loadFromData(exv);
            setFilePath(filePath);
            loadFromSidecarAndMerge(filePath);
        }
    }
    else
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifTool is not available to load metadata...";
        delete parser;

        return false;
    }

    delete parser;

    return true;
}

} // namespace Digikam
