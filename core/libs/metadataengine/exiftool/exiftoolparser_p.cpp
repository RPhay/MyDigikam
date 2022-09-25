/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-11-28
 * Description : ExifTool process stream parser - private container.
 *
 * SPDX-FileCopyrightText: 2020-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "exiftoolparser_p.h"

namespace Digikam
{

ExifToolParser::Private::Private(ExifToolParser* const q)
    : pp          (q),
      proc        (nullptr),
      async       (false),
      cmdRunning  (0)
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

void ExifToolParser::Private::prepareProcess()
{
    currentPath.clear();
    errorString.clear();
    exifToolData.clear();
}

bool ExifToolParser::Private::startProcess(const QByteArrayList& cmdArgs, ExifToolProcess::Action cmdAction)
{
    // Send command to ExifToolProcess

    cmdRunning = proc->command(cmdArgs, cmdAction);

    if (cmdRunning == 0)
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifTool cannot be sent:" << actionString(cmdAction);

        return false;
    }

    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool" << actionString(cmdAction) << cmdArgs.join(QByteArray(" "));

    if (!async)
    {
        while ((proc->cmdRunning() != cmdRunning) && (proc->cmdState() != ExifToolProcess::EXIT_RESULT))
        {
            if (!proc->waitForExifToolResult())
            {
                qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifTool timed out:" << actionString(cmdAction);

                return false;
            }
        }

        switch (proc->cmdState())
        {
            case ExifToolProcess::COMMAND_RESULT:
            {
                pp->slotCmdCompleted(proc->cmdRunning(),
                                     proc->cmdAction(),
                                     proc->elapsedTime(),
                                     proc->outputBuffer(),
                                     QByteArray());
                break;
            }

            case ExifToolProcess::FINISH_RESULT:
            {
                pp->slotFinished(proc->cmdRunning());
                break;
            }

            case ExifToolProcess::ERROR_RESULT:
            {
                pp->slotErrorOccurred(proc->cmdRunning(),
                                      proc->cmdAction(),
                                      proc->exifToolError(),
                                      proc->exifToolErrorString());
                break;
            }

            case ExifToolProcess::EXIT_RESULT:
            {
                return false;
            }
        }

        if (currentPath.isEmpty())
        {
            qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool complete" << actionString(cmdAction);
        }
        else
        {
            qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool complete" << actionString(cmdAction) << "for" << currentPath;
        }
    }

    return true;
}

QByteArray ExifToolParser::Private::filePathEncoding(const QFileInfo& fi) const
{
    return (QDir::toNativeSeparators(fi.filePath()).toUtf8());
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

        case ExifToolProcess::APPLY_CHANGES_EXV:
        {
            return QLatin1String("Apply Changes EXV");
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

        case ExifToolProcess::TAGS_DATABASE:
        {
            return QLatin1String("Tags Database");
        }

        case ExifToolProcess::VERSION_STRING:
        {
            return QLatin1String("Version String");
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

} // namespace Digikam
