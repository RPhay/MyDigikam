/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-02-18
 * Description : Qt5 and Qt6 interface for exiftool - private container.
 *               Based on ZExifTool Qt interface published at 18 Feb 2021
 *               https://github.com/philvl/ZExifTool
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (c) 2021 by Philippe Vianney Liaud <philvl dot dev at gmail dot com>
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

#include "exiftoolprocess_p.h"

namespace Digikam
{

QMutex ExifToolProcess::Private::s_cmdIdMutex;
int    ExifToolProcess::Private::s_nextCmdId = ExifToolProcess::Private::CMD_ID_MIN;

ExifToolProcess::Private::Private(ExifToolProcess* const q)
    : parent              (q),
      process             (nullptr),
      cmdRunning          (0),
      cmdAction           (ExifToolProcess::LOAD_METADATA),
      writeChannelIsClosed(true),
      exifToolHasFinished (false),
      processError        (QProcess::UnknownError)
{
    outAwait[0] = false;
    outAwait[1] = false;
    outReady[0] = false;
    outReady[1] = false;
}

void ExifToolProcess::Private::execNextCmd()
{
    if ((process->state() != QProcess::Running) ||
        writeChannelIsClosed)
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::execNextCmd(): ExifTool is not running";
        return;
    }

    if (cmdRunning || cmdQueue.isEmpty())
    {
        return;
    }

    // Clear QProcess buffers

    process->readAllStandardOutput();
    process->readAllStandardError();

    // Clear internal buffers

    outBuff[0]      = QByteArray();
    outBuff[1]      = QByteArray();
    outAwait[0]     = false;
    outAwait[1]     = false;
    outReady[0]     = false;
    outReady[1]     = false;

    // Exec Command

    execTimer.start();

    Command command = cmdQueue.takeFirst();
    cmdRunning      = command.id;
    cmdAction       = command.ac;

    process->write(command.argsStr);
}

void ExifToolProcess::Private::readOutput(const QProcess::ProcessChannel channel)
{
    process->setReadChannel(channel);

    while (process->canReadLine() && !outReady[channel])
    {
        QByteArray line = process->readLine();

        if (line.endsWith(QByteArray("\r\n")))
        {
            line.remove(line.size() - 2, 1); // Remove '\r' character
        }
/*
        qCDebug(DIGIKAM_METAENGINE_LOG) << channel << line;
*/
        if (!outAwait[channel])
        {
            if (line.startsWith(QByteArray("{await")) && line.endsWith(QByteArray("}\n")))
            {
                outAwait[channel] = line.mid(6, line.size() - 8).toInt();
            }

            continue;
        }

        outBuff[channel] += line;

        if (line.endsWith(QByteArray("{ready}\n")))
        {
            outBuff[channel].chop(8);
            outReady[channel] = true;

            break;
        }
    }

    // Check if outputChannel and errorChannel are both ready

    if (!(outReady[QProcess::StandardOutput] &&
        outReady[QProcess::StandardError]))
    {
/*
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::readOutput(): ExifTool read channels are not ready";
*/
        return;
    }

    if (
        (cmdRunning != outAwait[QProcess::StandardOutput]) ||
        (cmdRunning != outAwait[QProcess::StandardError])
       )
    {
        qCCritical(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::readOutput: Sync error between CmdID("
                                           << cmdRunning
                                           << "), outChannel("
                                           << outAwait[0]
                                           << ") and errChannel("
                                           << outAwait[1]
                                           << ")";
    }
    else
    {
        qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::readOutput(): ExifTool command completed";

        parent->emit signalCmdCompleted(cmdAction,
                                        execTimer.elapsed(),
                                        outBuff[QProcess::StandardOutput],
                                        outBuff[QProcess::StandardError]);
    }

    cmdRunning = 0; // No command is running

    execNextCmd();  // Exec next command
}

void ExifToolProcess::Private::setProcessErrorAndEmit(QProcess::ProcessError error, const QString& description)
{
    processError = error;
    errorString  = description;

    parent->emit signalErrorOccurred(cmdAction, error);
}

} // namespace Digikam
