/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-02-23
 * Description : item metadata interface - Exiftool helpers.
 *
 * SPDX-FileCopyrightText: 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2011      by Leif Huhn <leif at dkstat dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dmetadata.h"

// Qt includes

#include <QString>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QScopedPointer>

// Local includes

#include "digikam_config.h"
#include "digikam_debug.h"
#include "exiftoolparser.h"

namespace Digikam
{

bool DMetadata::loadUsingExifTool(const QString& filePath, bool merge)
{
    QMimeDatabase mimeDB;
    QFileInfo info(filePath);

    QString mimeType  = mimeDB.mimeTypeForFile(info).name();
    bool    copyToAll = (mimeType.startsWith(QLatin1String("video/"))     ||
                         (info.suffix().toUpper() == QLatin1String("FITS")));

    QScopedPointer<ExifToolParser> const parser(new ExifToolParser(nullptr));

    if (!parser->exifToolAvailable())
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifTool is not available to load metadata...";

        return false;
    }

    if (!parser->loadChunk(filePath, copyToAll))
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

    if (merge)
    {
        loadFromDataAndMerge(exv);
    }
    else
    {
        loadFromData(exv);
    }

    // Restore file path.

    setFilePath(filePath);
    loadFromSidecarAndMerge(filePath);

    return true;
}

} // namespace Digikam
