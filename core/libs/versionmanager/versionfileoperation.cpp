/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-12-20
 * Description : description of actions when saving a file with versioning
 *
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010      by Martin Klapetek <martin dot klapetek at gmail dot com>
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

#include "versionfileoperation.h"

// Qt includes

#include <QDir>
#include <QUrl>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

bool VersionFileInfo::isNull() const
{
    return fileName.isNull();
}

QString VersionFileInfo::filePath() const
{
    return path + QLatin1Char('/') + fileName;
}

QUrl VersionFileInfo::fileUrl() const
{
    QUrl url = QUrl::fromLocalFile(path);
    url      = url.adjusted(QUrl::StripTrailingSlash);
    url.setPath(url.path() + QLatin1Char('/') + fileName);

    return url;
}

QStringList VersionFileOperation::allFilePaths() const
{
    QStringList paths;

    if (!saveFile.isNull())
    {
        paths << saveFile.filePath();
    }

    if (!intermediateForLoadedFile.isNull())
    {
        paths << intermediateForLoadedFile.filePath();
    }

    foreach (const VersionFileInfo& intermediate, intermediates)
    {
        paths << intermediate.filePath();
    }

    return paths;
}

} // namespace Digikam
