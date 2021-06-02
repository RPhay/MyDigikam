/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-02-18
 * Description : Qt5 and Qt6 interface for exiftool.
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

ExifToolProcess::ExifToolProcess(QObject* const parent)
    : QObject(parent),
      d      (new Private(this))
{
    d->process = new QProcess(this);
    d->process->setProcessEnvironment(adjustedEnvironmentForAppImage());

    connect(d->process, &QProcess::started,
            this, &ExifToolProcess::slotStarted);

#if QT_VERSION >= 0x060000

    connect(d->process, &QProcess::finished,
            this, &ExifToolProcess::slotFinished);

#else

    connect(d->process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &ExifToolProcess::slotFinished);

#endif

    connect(d->process, &QProcess::stateChanged,
            this, &ExifToolProcess::slotStateChanged);

    connect(d->process, &QProcess::errorOccurred,
            this, &ExifToolProcess::slotErrorOccurred);

    connect(d->process, &QProcess::readyReadStandardOutput,
            this, &ExifToolProcess::slotReadyReadStandardOutput);

    connect(d->process, &QProcess::readyReadStandardError,
            this, &ExifToolProcess::slotReadyReadStandardError);
}

ExifToolProcess::~ExifToolProcess()
{
    terminate();

    delete d;
}

void ExifToolProcess::setProgram(const QString& etExePath, const QString& perlExePath)
{
    // Check if ExifTool is starting or running

    if (d->process->state() != QProcess::NotRunning)
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::setProgram(): ExifTool is already running";

        return;
    }

    d->etExePath   = etExePath;
    d->perlExePath = perlExePath;

    if      (d->etExePath.isEmpty())
    {
        d->etExePath = exifToolBin();
    }
    else if (QFileInfo(d->etExePath).isDir())
    {
        d->etExePath.append(QLatin1Char('/'));
        d->etExePath.append(exifToolBin());
    }
}

QString ExifToolProcess::program() const
{
    return d->etExePath;
}

bool ExifToolProcess::start()
{
    // Check if ExifTool is starting or running

    if (d->exifToolHasFinished                    ||
        d->process->state() != QProcess::NotRunning)
    {
        return true;
    }

    if (!checkExifToolProgram())
    {
        return false;
    }

    // Prepare command for ExifTool

    QString program = d->etExePath;
    QStringList args;

    if (!d->perlExePath.isEmpty())
    {
        program = d->perlExePath;
        args << d->etExePath;
    }

    //-- Advanced options

    args << QLatin1String("-stay_open");
    args << QLatin1String("true");

    //-- Other options

    args << QLatin1String("-@");
    args << QLatin1String("-");

    // Clear queue before start

    d->cmdQueue.clear();
    d->cmdRunning           = 0;
    d->cmdAction            = NO_ACTION;

    // Clear errors

    d->processError         = QProcess::UnknownError;
    d->errorString.clear();

    // Start ExifTool process

    d->writeChannelIsClosed = false;

    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::start(): create new ExifTool instance:" << program << args;

    d->process->start(program, args, QProcess::ReadWrite);

    return d->process->waitForStarted(1000);
}

void ExifToolProcess::terminate()
{
    if      (d->process->state() == QProcess::Running)
    {
        // If process is in running state, close ExifTool normally

        qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::terminate(): send ExifTool shutdown command...";

        d->cmdQueue.clear();
        d->process->write(QByteArray("-stay_open\nfalse\n"));
        d->process->closeWriteChannel();
        d->writeChannelIsClosed = true;
        d->exifToolHasFinished  = true;

        if (!d->process->waitForFinished(5000))
        {
            // Otherwise, close ExifTool using OS system call
            // (WM_CLOSE [Windows] or SIGTERM [Unix])

            // Console applications on Windows that do not run an event loop,
            // or whose event loop does not handle the WM_CLOSE message,
            // can only be terminated by calling kill().

#ifdef Q_OS_WIN

            kill();

#else

            qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::terminate(): closing ExifTool instance...";

            d->process->terminate();

#endif

        }
    }
}

void ExifToolProcess::kill()
{
    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::kill(): shutdown ExifTool instance...";

    d->process->kill();
}

bool ExifToolProcess::isRunning() const
{
    return (d->process->state() == QProcess::Running);
}

bool ExifToolProcess::isBusy() const
{
    return (d->cmdRunning ? true : false);
}

qint64 ExifToolProcess::processId() const
{
    return d->process->processId();
}

QProcess::ProcessState ExifToolProcess::state() const
{
    return d->process->state();
}

QProcess::ProcessError ExifToolProcess::error() const
{
    return d->processError;
}

QString ExifToolProcess::errorString() const
{
    return d->errorString;
}

QProcess::ExitStatus ExifToolProcess::exitStatus() const
{
    return d->process->exitStatus();
}

bool ExifToolProcess::waitForStarted(int msecs) const
{
    return d->process->waitForStarted(msecs);
}

bool ExifToolProcess::waitForFinished(int msecs) const
{
    return d->process->waitForFinished(msecs);
}

int ExifToolProcess::command(const QByteArrayList& args, Action ac)
{
    if ((d->process->state() != QProcess::Running) ||
        d->writeChannelIsClosed                    ||
        args.isEmpty())
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::command(): cannot process command with ExifTool" << args;

        return 0;
    }

    // ThreadSafe incrementation of d->nextCmdId

    Private::s_cmdIdMutex.lock();
    const int cmdId = Private::s_nextCmdId;

    if (Private::s_nextCmdId++ >= Private::CMD_ID_MAX)
    {
        Private::s_nextCmdId = Private::CMD_ID_MIN;
    }

    Private::s_cmdIdMutex.unlock();

    // String representation of d->cmdId with leading zero -> constant size: 10 char

    const QByteArray cmdIdStr = QByteArray::number(cmdId).rightJustified(10, '0');

    // Build command string from args

    QByteArray cmdStr;

    for (const QByteArray& arg : args)
    {
        cmdStr.append(arg + '\n');
    }

    //-- Advanced options

    cmdStr.append(QByteArray("-echo1\n{await") + cmdIdStr + QByteArray("}\n"));     // Echo text to stdout before processing is complete
    cmdStr.append(QByteArray("-echo2\n{await") + cmdIdStr + QByteArray("}\n"));     // Echo text to stderr before processing is complete

    if (cmdStr.contains(QByteArray("-q"))               ||
        cmdStr.toLower().contains(QByteArray("-quiet")) ||
        cmdStr.contains(QByteArray("-T"))               ||
        cmdStr.toLower().contains(QByteArray("-table")))
    {
        cmdStr.append(QByteArray("-echo3\n{ready}\n"));                 // Echo text to stdout after processing is complete
    }

    cmdStr.append(QByteArray("-echo4\n{ready}\n"));                     // Echo text to stderr after processing is complete
    cmdStr.append(QByteArray("-execute\n"));                            // Execute command and echo {ready} to stdout after processing is complete

    // TODO: if -binary user, {ready} can not be present in the new line

    // Add command to queue

    Private::Command command;
    command.id      = cmdId;
    command.argsStr = cmdStr;
    command.ac      = ac;
    d->cmdQueue.append(command);

    // Exec cmd queue

    d->execNextCmd();

    return cmdId;
}

void ExifToolProcess::slotStarted()
{
    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool process started";

    emit signalStarted(d->cmdAction);
}

void ExifToolProcess::slotFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool process finished" << exitCode << exitStatus;

    emit signalFinished(d->cmdAction, exitCode, exitStatus);

    d->cmdRunning = 0;
    d->cmdAction  = NO_ACTION;
}

void ExifToolProcess::slotStateChanged(QProcess::ProcessState newState)
{
    emit signalStateChanged(d->cmdAction, newState);
}

void ExifToolProcess::slotErrorOccurred(QProcess::ProcessError error)
{
    d->setProcessErrorAndEmit(error, d->process->errorString());
}

void ExifToolProcess::slotReadyReadStandardOutput()
{
    d->readOutput(QProcess::StandardOutput);
}

void ExifToolProcess::slotReadyReadStandardError()
{
    d->readOutput(QProcess::StandardError);
}

QString ExifToolProcess::exifToolBin() const
{

#ifdef Q_OS_WIN

    return QLatin1String("exiftool.exe");

#else

    return QLatin1String("exiftool");

#endif

}

bool ExifToolProcess::checkExifToolProgram()
{
    // Check if Exiftool program exists and have execution permissions

    qCDebug(DIGIKAM_METAENGINE_LOG) << "Path to ExifTool:" << d->etExePath;

    if (
        (d->etExePath != exifToolBin())                    &&
        (!QFile::exists(d->etExePath)                      ||
        !(QFile::permissions(d->etExePath) & QFile::ExeUser))
       )
    {
        d->setProcessErrorAndEmit(QProcess::FailedToStart,
                                  QString::fromLatin1("ExifTool does not exists or exec permission is missing"));
        return false;
    }

    // If perl path is defined, check if Perl program exists and have execution permissions

    if (!d->perlExePath.isEmpty() && (!QFile::exists(d->perlExePath) ||
        !(QFile::permissions(d->perlExePath) & QFile::ExeUser)))
    {
        d->setProcessErrorAndEmit(QProcess::FailedToStart,
                                  QString::fromLatin1("Perl does not exists or exec permission is missing"));
        return false;
    }

    return true;
}

} // namespace Digikam
