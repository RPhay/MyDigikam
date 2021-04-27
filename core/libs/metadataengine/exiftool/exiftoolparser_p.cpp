/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-11-28
 * Description : ExifTool process stream parser - private container.
 *
 * Copyright (C) 2020-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "exiftoolparser_p.h"

namespace Digikam
{

ExifToolParser::Private::Private()
    : proc     (nullptr),
      loopLoad (nullptr),
      loopChunk(nullptr),
      loopApply(nullptr)
{
}

bool ExifToolParser::Private::prepareProcess()
{
    currentPath.clear();
    exifToolData.clear();

    // Start ExifToolProcess if necessary

    proc->start();

    if (!proc->waitForStarted(500))
    {
        proc->kill();
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifTool process cannot be started ("
                                          << proc->program()
                                          << ")";

        return false;
    }

    return true;
}

QByteArray ExifToolParser::Private::filePathEncoding(const QFileInfo& fi) const
{

#ifdef Q_OS_WIN

    return (QDir::toNativeSeparators(fi.filePath()).toLocal8Bit());

#else

    return (QDir::toNativeSeparators(fi.filePath()).toUtf8());

#endif

}

QString ExifToolParser::Private::actionString(int cmdAction) const
{
    switch (cmdAction)
    {
        case ExifToolProcess::LOAD_METADATA:
        {
            return QLatin1String("Load Metadata");
        }

        case ExifToolProcess::LOAD_CHUNKS:
        {
            return QLatin1String("Load Chunks");
        }

        case ExifToolProcess::APPLY_CHANGES:
        {
            return QLatin1String("Apply Changes");
        }

        default: // ExifToolProcess::NO_ACTION
        {
            break;
        }
    }

    return QString();
}

void ExifToolParser::Private::manageEventLoop(int cmdAction)
{
    switch (cmdAction)
    {
        case ExifToolProcess::LOAD_METADATA:
        {
            loopLoad->quit();
            break;
        }

        case ExifToolProcess::LOAD_CHUNKS:
        {
            loopChunk->quit();
            break;
        }

        case ExifToolProcess::APPLY_CHANGES:
        {
            loopApply->quit();
            break;
        }

        default: // ExifToolProcess::NO_ACTION
        {
            break;
        }
    }
}

} // namespace Digikam
