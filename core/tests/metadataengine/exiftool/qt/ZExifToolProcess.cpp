/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-03-18
 * Description : ZExifTool - Qt5 and Qt6 interface for exiftool.
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (c) 2020 by Philippe Vianney-Liaud <https://github.com/philvl>
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ============================================================ */

#include "ZExifToolProcess.h"

// Qt includes

#include <QFile>
#include <QDebug>

// Init static variables

QMutex       ZExifToolProcess::_cmdIdMutex;
const int    ZExifToolProcess::CMD_ID_MIN   = 1;
const int    ZExifToolProcess::CMD_ID_MAX   = 2000000000;
int          ZExifToolProcess::_nextCmdId   = CMD_ID_MIN;

ZExifToolProcess::ZExifToolProcess(QObject* const parent)
    : QObject(parent)
{
    _cmdRunning   = 0;
    _outAwait[0]  = _outAwait[1]= false;
    _outReady[0]  = _outReady[1]= false;
    _writeChannelIsClosed= true;
    _processError = QProcess::UnknownError;
    _process      = new QProcess(this);

    connect(_process, &QProcess::started, this, &ZExifToolProcess::onStarted);

#if QT_VERSION >= 0x060000

    connect(_process, &QProcess::finished, this, &ZExifToolProcess::onFinished);

#else

    connect(_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &ZExifToolProcess::onFinished);

#endif

    connect(_process, &QProcess::stateChanged,  this, &ZExifToolProcess::onStateChanged);
    connect(_process, &QProcess::errorOccurred, this, &ZExifToolProcess::onErrorOccurred);
    connect(_process, &QProcess::readyReadStandardOutput, this, &ZExifToolProcess::onReadyReadStandardOutput);
    connect(_process, &QProcess::readyReadStandardError,  this, &ZExifToolProcess::onReadyReadStandardError);
}

ZExifToolProcess::~ZExifToolProcess()
{
}

void ZExifToolProcess::setProgram(const QString& etExePath, const QString& perlExePath)
{
    // Check if ExifTool is starting or running

    if (_process->state() != QProcess::NotRunning)
    {
        qWarning("ZExifToolProcess::setProgram: ExifTool is already running");
        return;
    }

    _etExePath   = etExePath;
    _perlExePath = perlExePath;
}

void ZExifToolProcess::start()
{
    // Check if ExifTool is starting or running

    if (_process->state() != QProcess::NotRunning)
    {
        qWarning("ZExifToolProcess::start: ExifTool is already running");
        return;
    }

    // Check if Exiftool program exists and have execution permissions

    if (!QFile::exists(_etExePath) || !(QFile::permissions(_etExePath) & QFile::ExeUser))
    {
        setProcessErrorAndEmit(QProcess::FailedToStart, QString::fromLatin1("ExifTool does not exists or exec permission is missing"));
        return;
    }

    // If perl path is defined, check if Perl program exists and have execution permissions

    if (!_perlExePath.isEmpty() && (!QFile::exists(_perlExePath) || !(QFile::permissions(_perlExePath) & QFile::ExeUser)))
    {
        setProcessErrorAndEmit(QProcess::FailedToStart, QString::fromLatin1("Perl does not exists or exec permission is missing"));
        return;
    }

    // Prepare command for ExifTool

    QString program = _etExePath;
    QStringList args;

    if (!_perlExePath.isEmpty())
    {
        program= _perlExePath;
        args <<  _etExePath;
    }

    //-- Advanced options

    args << QLatin1String("-stay_open");
    args << QLatin1String("true");

    //-- Other options

    args << QLatin1String("-@");
    args << QLatin1String("-");

    // Clear queue before start

    _cmdQueue.clear();
    _cmdRunning = 0;

    // Clear errors

    _processError = QProcess::UnknownError;
    _errorString.clear();

    // Start ExifTool process

    _writeChannelIsClosed = false;
    _process->start(program, args, QProcess::ReadWrite);
}

void ZExifToolProcess::terminate()
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

void ZExifToolProcess::kill()
{
    _process->kill();
}

bool ZExifToolProcess::isRunning() const
{
    return (_process->state() == QProcess::Running);
}

bool ZExifToolProcess::isBusy() const
{
    return (_cmdRunning ? true : false);
}

qint64 ZExifToolProcess::processId() const
{
    return _process->processId();
}

QProcess::ProcessState ZExifToolProcess::state() const
{
    return _process->state();
}

QProcess::ProcessError ZExifToolProcess::error() const
{
    return _processError;
}

QString ZExifToolProcess::errorString() const
{
    return _errorString;
}

QProcess::ExitStatus ZExifToolProcess::exitStatus() const
{
    return _process->exitStatus();
}

bool ZExifToolProcess::waitForStarted(int msecs)
{
    return _process->waitForStarted(msecs);
}

bool ZExifToolProcess::waitForFinished(int msecs)
{
    return _process->waitForFinished(msecs);
}

/**
 * Send a command to exiftool process
 * Return 0: ExitTool not running, write channel is closed or args is empty
 */
int ZExifToolProcess::command(const QByteArrayList& args)
{
    if(_process->state() != QProcess::Running || _writeChannelIsClosed || args.isEmpty())
    {
        return 0;
    }

    // ThreadSafe incrementation of _nextCmdId

    _cmdIdMutex.lock();
    const int cmdId= _nextCmdId;

    if (_nextCmdId++ >= CMD_ID_MAX)
    {
        _nextCmdId= CMD_ID_MIN;
    }

    _cmdIdMutex.unlock();

    // String representation of _cmdId with leading zero -> constant size: 10 char

    const QByteArray cmdIdStr = QByteArray::number(cmdId).rightJustified(10, '0');

    // Build command string from args

    QByteArray cmdStr;

    for (const QByteArray& arg : args)
    {
        cmdStr.append(arg+'\n');
    }

    //-- Advanced options

    cmdStr.append("-echo1\n{await"+cmdIdStr+"}\n"); // Echo text to stdout before processing is complete
    cmdStr.append("-echo2\n{await"+cmdIdStr+"}\n"); // Echo text to stderr before processing is complete

    if (cmdStr.contains("-q") || cmdStr.toLower().contains("-quiet") || cmdStr.contains("-T") || cmdStr.toLower().contains("-table"))
    {
        cmdStr.append("-echo3\n{ready}\n");         // Echo text to stdout after processing is complete
    }

    cmdStr.append("-echo4\n{ready}\n");             // Echo text to stderr after processing is complete
    cmdStr.append("-execute\n");                    // Execute command and echo {ready} to stdout after processing is complete

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

void ZExifToolProcess::execNextCmd()
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

    _outBuff[0]  =  _outBuff[1] =  QByteArray();
    _outAwait[0] = _outAwait[1] = false;
    _outReady[0] = _outReady[1] = false;

    // Exec Command

    _execTimer.start();
    Command command = _cmdQueue.takeFirst();
    _cmdRunning     = command.id;
    _process->write(command.argsStr);
}

void ZExifToolProcess::onStarted()
{
    emit started();
}

void ZExifToolProcess::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
/*
    qDebug() << "ExifTool process finished" << exitCode << exitStatus;
*/
    _cmdRunning = 0;
    emit finished(exitCode, exitStatus);
}

void ZExifToolProcess::onStateChanged(QProcess::ProcessState newState)
{
    emit stateChanged(newState);
}

void ZExifToolProcess::onErrorOccurred(QProcess::ProcessError error)
{
    setProcessErrorAndEmit(error, _process->errorString());
}

void ZExifToolProcess::onReadyReadStandardOutput()
{
    readOutput(QProcess::StandardOutput);
}

void ZExifToolProcess::onReadyReadStandardError()
{
    readOutput(QProcess::StandardError);
}

void ZExifToolProcess::readOutput(const QProcess::ProcessChannel channel)
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
        qDebug() << channel << line;
*/
        if (!_outAwait[channel])
        {
            if (line.startsWith("{await") && line.endsWith("}\n"))
            {
                _outAwait[channel] = line.mid(6, line.size()-8).toInt();
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

    if ((_cmdRunning != _outAwait[QProcess::StandardOutput]) || (_cmdRunning != _outAwait[QProcess::StandardError]))
    {
        qCritical().nospace() << "ZExifToolProcess::readOutput: Sync error between CmdID("
                              << _cmdRunning
                              << "), outChannel("
                              << _outAwait[0]
                              << ") and errChannel("
                              << _outAwait[1]
                              << ")";
    }
    else
    {
        emit cmdCompleted(_cmdRunning, _execTimer.elapsed(), _outBuff[QProcess::StandardOutput], _outBuff[QProcess::StandardError]);
    }

    _cmdRunning = 0; // No command is running

    execNextCmd();   // Exec next command
}

void ZExifToolProcess::setProcessErrorAndEmit(QProcess::ProcessError error, const QString& description)
{
    _processError = error;
    _errorString  =  description;

    emit errorOccurred(error);
}
