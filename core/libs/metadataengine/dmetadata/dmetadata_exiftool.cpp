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
 * SPDX-License-Identifier: GPL-2.0-or-later
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

bool DMetadata::loadUsingExifTool(const QString& filePath, bool fromVideo)
{
    QScopedPointer<ExifToolParser> const parser(new ExifToolParser(nullptr));

    if (!parser->exifToolAvailable())
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifTool is not available to load metadata...";

        return false;
    }

    bool ret = parser->loadChunk(filePath, fromVideo);

    if (!ret)
    {
        qCCritical(DIGIKAM_METAENGINE_LOG) << "Load metadata using ExifTool failed...";

        return false;
    }

    ExifToolParser::ExifToolData chunk = parser->currentData();

    qCDebug(DIGIKAM_METAENGINE_LOG) << "Metadata chunk loaded with ExifTool";

    ExifToolParser::ExifToolData::iterator it = chunk.find(QLatin1String("EXV"));

    if (it == chunk.end())
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "Metadata chunk loaded with ExifTool is empty";

        return false;
    }

    QVariantList varLst = it.value();
    QByteArray exv      = varLst[0].toByteArray();

    if (exv.isEmpty())
    {
        qCDebug(DIGIKAM_METAENGINE_LOG) << "Metadata chunk loaded with ExifTool has no data";

        return false;
    }

    loadFromData(exv);

    // Restore file path.

    setFilePath(filePath);
    loadFromSidecarAndMerge(filePath);

    return true;
}

} // namespace Digikam
