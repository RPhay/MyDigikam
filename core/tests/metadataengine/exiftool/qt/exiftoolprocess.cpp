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
 * Copyright (c) 2021 by Philippe Vianney-Liaud <https://github.com/philvl>
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

#include "exiftoolprocess.h"

// Qt includes

#include <QFile>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

// Init static variables

QMutex       ExifToolProcess::_cmdIdMutex;
const int    ExifToolProcess::CMD_ID_MIN   = 1;
const int    ExifToolProcess::CMD_ID_MAX   = 2000000000;
int          ExifToolProcess::_nextCmdId   = CMD_ID_MIN;

ExifToolProcess::ExifToolProcess(QObject* const parent)
    : QObject              (parent),
      _cmdRunning          (0),
      _writeChannelIsClosed(true),
      _processError        (QProcess::UnknownError)
{
    _outAwait[0]          = false;
    _outAwait[1]          = false;
    _outReady[0]          = false;
    _outReady[1]          = false;
    _process              = new QProcess(this);

    connect(_process, &QProcess::started,
            this, &ExifToolProcess::slotStarted);

#if QT_VERSION >= 0x060000

    connect(_process, &QProcess::finished,
            this, &ExifToolProcess::slotFinished);

#else

    connect(_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &ExifToolProcess::slotFinished);

#endif

    connect(_process, &QProcess::stateChanged,
            this, &ExifToolProcess::slotStateChanged);

    connect(_process, &QProcess::errorOccurred,
            this, &ExifToolProcess::slotErrorOccurred);

    connect(_process, &QProcess::readyReadStandardOutput,
            this, &ExifToolProcess::slotReadyReadStandardOutput);

    connect(_process, &QProcess::readyReadStandardError,
            this, &ExifToolProcess::slotReadyReadStandardError);
}

ExifToolProcess::~ExifToolProcess()
{
}

void ExifToolProcess::setProgram(const QString& etExePath, const QString& perlExePath)
{
    // Check if ExifTool is starting or running

    if (_process->state() != QProcess::NotRunning)
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::setProgram(): ExifTool is already running";
        return;
    }

    _etExePath   = etExePath;
    _perlExePath = perlExePath;
}

void ExifToolProcess::start()
{
    // Check if ExifTool is starting or running

    if (_process->state() != QProcess::NotRunning)
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::start(): ExifTool is already running";
        return;
    }

    // Check if Exiftool program exists and have execution permissions

    if (!QFile::exists(_etExePath) || !(QFile::permissions(_etExePath) & QFile::ExeUser))
    {
        setProcessErrorAndEmit(QProcess::FailedToStart,
                               QString::fromLatin1("ExifTool does not exists or exec permission is missing"));
        return;
    }

    // If perl path is defined, check if Perl program exists and have execution permissions

    if (!_perlExePath.isEmpty() && (!QFile::exists(_perlExePath) || !(QFile::permissions(_perlExePath) & QFile::ExeUser)))
    {
        setProcessErrorAndEmit(QProcess::FailedToStart,
                               QString::fromLatin1("Perl does not exists or exec permission is missing"));
        return;
    }

    // Prepare command for ExifTool

    QString program = _etExePath;
    QStringList args;

    if (!_perlExePath.isEmpty())
    {
        program = _perlExePath;
        args << _etExePath;
    }

    //-- Advanced options

    args << QLatin1String("-stay_open");
    args << QLatin1String("true");

    //-- Other options

    args << QLatin1String("-@");
    args << QLatin1String("-");

    // Clear queue before start

    _cmdQueue.clear();
    _cmdRunning           = 0;

    // Clear errors

    _processError         = QProcess::UnknownError;
    _errorString.clear();

    // Start ExifTool process

    _writeChannelIsClosed = false;

    _process->start(program, args, QProcess::ReadWrite);
}

void ExifToolProcess::terminate()
{
    if (_process->state() == QProcess::Running)
    {
        // If process is in running state, close ExifTool normally

        _cmdQueue.clear();
        _process->write("-stay_open\nfalse\n");
        _process->closeWriteChannel();
        _writeChannelIsClosed= true;
    }
    else
    {
        // Otherwise, close ExifTool using OS system call
        // (WM_CLOSE [Windows] or SIGTERM [Unix])

        _process->terminate();
    }
}

void ExifToolProcess::kill()
{
    _process->kill();
}

bool ExifToolProcess::isRunning() const
{
    return (_process->state() == QProcess::Running);
}

bool ExifToolProcess::isBusy() const
{
    return (_cmdRunning ? true : false);
}

qint64 ExifToolProcess::processId() const
{
    return _process->processId();
}

QProcess::ProcessState ExifToolProcess::state() const
{
    return _process->state();
}

QProcess::ProcessError ExifToolProcess::error() const
{
    return _processError;
}

QString ExifToolProcess::errorString() const
{
    return _errorString;
}

QProcess::ExitStatus ExifToolProcess::exitStatus() const
{
    return _process->exitStatus();
}

bool ExifToolProcess::waitForStarted(int msecs)
{
    return _process->waitForStarted(msecs);
}

bool ExifToolProcess::waitForFinished(int msecs)
{
    return _process->waitForFinished(msecs);
}

int ExifToolProcess::command(const QByteArrayList& args)
{
    if ((_process->state() != QProcess::Running) || _writeChannelIsClosed || args.isEmpty())
    {
        return 0;
    }

    // ThreadSafe incrementation of _nextCmdId

    _cmdIdMutex.lock();
    const int cmdId = _nextCmdId;

    if (_nextCmdId++ >= CMD_ID_MAX)
    {
        _nextCmdId = CMD_ID_MIN;
    }

    _cmdIdMutex.unlock();

    // String representation of _cmdId with leading zero -> constant size: 10 char

    const QByteArray cmdIdStr = QByteArray::number(cmdId).rightJustified(10, '0');

    // Build command string from args

    QByteArray cmdStr;

    for (const QByteArray& arg : args)
    {
        cmdStr.append(arg + '\n');
    }

    //-- Advanced options

    cmdStr.append("-echo1\n{await" + cmdIdStr + "}\n");     // Echo text to stdout before processing is complete
    cmdStr.append("-echo2\n{await" + cmdIdStr + "}\n");     // Echo text to stderr before processing is complete

    if (cmdStr.contains("-q")               ||
        cmdStr.toLower().contains("-quiet") ||
        cmdStr.contains("-T")               ||
        cmdStr.toLower().contains("-table"))
    {
        cmdStr.append("-echo3\n{ready}\n");                 // Echo text to stdout after processing is complete
    }

    cmdStr.append("-echo4\n{ready}\n");                     // Echo text to stderr after processing is complete
    cmdStr.append("-execute\n");                            // Execute command and echo {ready} to stdout after processing is complete

    // TODO: if -binary user, {ready} can not be present in the new line

    // Add command to queue

    Command command;
    command.id      = cmdId;
    command.argsStr = cmdStr;
    _cmdQueue.append(command);

    // Exec cmd queue

    execNextCmd();

    return cmdId;
}

void ExifToolProcess::execNextCmd()
{
    if ((_process->state() != QProcess::Running) || _writeChannelIsClosed)
    {
        return;
    }

    if (_cmdRunning || _cmdQueue.isEmpty())
    {
        return;
    }

    // Clear QProcess buffers

    _process->readAllStandardOutput();
    _process->readAllStandardError();

    // Clear internal buffers

    _outBuff[0]  = QByteArray();
    _outBuff[1]  = QByteArray();
    _outAwait[0] = false;
    _outAwait[1] = false;
    _outReady[0] = false;
    _outReady[1] = false;

    // Exec Command

    _execTimer.start();

    Command command = _cmdQueue.takeFirst();
    _cmdRunning     = command.id;

    _process->write(command.argsStr);
}

void ExifToolProcess::slotStarted()
{
    emit signalStarted();
}

void ExifToolProcess::slotFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
/*
    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool process finished" << exitCode << exitStatus;
*/
    _cmdRunning = 0;

    emit signalFinished(exitCode, exitStatus);
}

void ExifToolProcess::slotStateChanged(QProcess::ProcessState newState)
{
    emit signalStateChanged(newState);
}

void ExifToolProcess::slotErrorOccurred(QProcess::ProcessError error)
{
    setProcessErrorAndEmit(error, _process->errorString());
}

void ExifToolProcess::slotReadyReadStandardOutput()
{
    readOutput(QProcess::StandardOutput);
}

void ExifToolProcess::slotReadyReadStandardError()
{
    readOutput(QProcess::StandardError);
}

void ExifToolProcess::readOutput(const QProcess::ProcessChannel channel)
{
    _process->setReadChannel(channel);

    while (_process->canReadLine() && !_outReady[channel])
    {
        QByteArray line = _process->readLine();

        if (line.endsWith("\r\n"))
        {
            line.remove(line.size()-2, 1); // Remove '\r' character
        }
/*
        qCDebug(DIGIKAM_METAENGINE_LOG) << channel << line;
*/
        if (!_outAwait[channel])
        {
            if (line.startsWith("{await") && line.endsWith("}\n"))
            {
                _outAwait[channel] = line.mid(6, line.size() - 8).toInt();
            }

            continue;
        }

        _outBuff[channel] += line;

        if (line.endsWith("{ready}\n"))
        {
            _outBuff[channel].chop(8);
            _outReady[channel] = true;

            break;
        }
    }

    // Check if outputChannel and errorChannel are both ready

    if (!(_outReady[QProcess::StandardOutput] && _outReady[QProcess::StandardError]))
    {
        return;
    }

    if (
        (_cmdRunning != _outAwait[QProcess::StandardOutput]) ||
        (_cmdRunning != _outAwait[QProcess::StandardError])
       )
    {
        qCCritical(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::readOutput: Sync error between CmdID("
                                           << _cmdRunning
                                           << "), outChannel("
                                           << _outAwait[0]
                                           << ") and errChannel("
                                           << _outAwait[1]
                                           << ")";
    }
    else
    {
        emit signalCmdCompleted(_cmdRunning,
                                _execTimer.elapsed(),
                                _outBuff[QProcess::StandardOutput],
                                _outBuff[QProcess::StandardError]);
    }

    _cmdRunning = 0; // No command is running

    execNextCmd();   // Exec next command
}

void ExifToolProcess::setProcessErrorAndEmit(QProcess::ProcessError error, const QString& description)
{
    _processError = error;
    _errorString  =  description;

    emit signalErrorOccurred(error);
}

} // namespace Digikam
