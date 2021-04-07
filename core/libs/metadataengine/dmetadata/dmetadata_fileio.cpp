/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-02-23
 * Description : item metadata interface - file I/O helpers.
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <QFile>
#include <QFileInfo>
#include <QLocale>
#include <QUuid>
#include <QMimeDatabase>

// Local includes

#include "filereadwritelock.h"
#include "metaenginesettings.h"
#include "digikam_version.h"
#include "digikam_globals.h"
#include "digikam_debug.h"

namespace Digikam
{

bool DMetadata::load(const QString& filePath)
{
    FileReadLocker lock(filePath);

    bool hasLoaded = false;
    QMimeDatabase mimeDB;

    if (
        !mimeDB.mimeTypeForFile(filePath).name().startsWith(QLatin1String("video/")) &&
        !mimeDB.mimeTypeForFile(filePath).name().startsWith(QLatin1String("audio/"))
       )
    {
        // Process images only with Exiv2 backend first, or libraw in second for RAW files, or with libheif, or at end with ImageMagick.
        // Never process video files with Exiv2, the backend is very unstable.
/*
        qCDebug(DIGIKAM_METAENGINE_LOG) << "Loading metadata with Exiv2 from" << filePath;
*/
        if (!(hasLoaded = MetaEngine::load(filePath)))
        {
/*
            qCDebug(DIGIKAM_METAENGINE_LOG) << "Loading metadata with LibRaw from" << filePath;
*/
            if (!(hasLoaded = loadUsingRawEngine(filePath)))
            {
/*
                qCDebug(DIGIKAM_METAENGINE_LOG) << "Loading metadata with LibHeif from" << filePath;
*/
                if (!(hasLoaded = loadUsingLibheif(filePath)))
                {
/*
                    qCDebug(DIGIKAM_METAENGINE_LOG) << "Loading metadata with ImageMagick from" << filePath;
*/
                    hasLoaded = loadUsingImageMagick(filePath);
                }
            }
        }
    }
    else
    {
        // No image files (aka video or audio), process with ffmpeg backend.

        hasLoaded  = loadUsingFFmpeg(filePath);
        hasLoaded |= loadFromSidecarAndMerge(filePath);
    }

    return hasLoaded;
}

bool DMetadata::save(const QString& filePath, bool setVersion) const
{
    FileWriteLocker lock(filePath);

    return MetaEngine::save(filePath, setVersion);
}

bool DMetadata::applyChanges(bool setVersion) const
{
    FileWriteLocker lock(getFilePath());

    return MetaEngine::applyChanges(setVersion);
}

} // namespace Digikam
