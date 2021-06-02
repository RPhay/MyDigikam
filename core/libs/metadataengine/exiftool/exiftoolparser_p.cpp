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
    : proc(nullptr)
{
    argsFile.setAutoRemove(false);
}

ExifToolParser::Private::~Private()
{
    if (argsFile.exists())
    {
        argsFile.remove();
    }
}

bool ExifToolParser::Private::prepareProcess()
{
    currentPath.clear();
    exifToolData.clear();

    // Start ExifToolProcess if necessary

    if (!proc->start())
    {
        proc->kill();
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifTool process cannot be started ("
                                          << proc->program()
                                          << ")";

        return false;
    }

    return true;
}

bool ExifToolParser::Private::startProcess(const QByteArrayList& cmdArgs, ExifToolProcess::Action cmdAction)
{
    // Send command to ExifToolProcess

    int ret = proc->command(cmdArgs, cmdAction);

    if (ret == 0)
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifTool cannot be sent:" << actionString(cmdAction);

        return false;
    }

    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool" << actionString(cmdAction) << cmdArgs.join(QByteArray(" "));

    evLoops[cmdAction]->exec(QEventLoop::ExcludeUserInputEvents);

    if (currentPath.isEmpty())
    {
        qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool complete" << actionString(cmdAction);
    }
    else
    {
        qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool complete" << actionString(cmdAction) << "for" << currentPath;
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

        case ExifToolProcess::READ_FORMATS:
        {
            return QLatin1String("Readable Formats");
        }

        case ExifToolProcess::WRITE_FORMATS:
        {
            return QLatin1String("Writable Formats");
        }

        case ExifToolProcess::TRANSLATIONS_LIST:
        {
            return QLatin1String("Translations List");
        }

        case ExifToolProcess::COPY_TAGS:
        {
            return QLatin1String("Copy Tags");
        }

        case ExifToolProcess::TRANS_TAGS:
        {
            return QLatin1String("Translate Tags");
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
    if ((cmdAction >= ExifToolProcess::LOAD_METADATA) &&
        (cmdAction <  ExifToolProcess::NO_ACTION))
    {
        if (evLoops[cmdAction])
        {
            evLoops[cmdAction]->quit();
        }
    }
}

} // namespace Digikam
