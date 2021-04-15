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

const int    ExifToolProcess::CMD_ID_MIN    = 1;
const int    ExifToolProcess::CMD_ID_MAX    = 2000000000;

QMutex       ExifToolProcess::m_cmdIdMutex;
int          ExifToolProcess::m_nextCmdId   = CMD_ID_MIN;

ExifToolProcess::ExifToolProcess(QObject* const parent)
    : QObject              (parent),
      m_cmdRunning          (0),
      m_writeChannelIsClosed(true),
      m_processError        (QProcess::UnknownError)
{
    m_outAwait[0]          = false;
    m_outAwait[1]          = false;
    m_outReady[0]          = false;
    m_outReady[1]          = false;
    m_process              = new QProcess(this);

    connect(m_process, &QProcess::started,
            this, &ExifToolProcess::slotStarted);

#if QT_VERSION >= 0x060000

    connect(m_process, &QProcess::finished,
            this, &ExifToolProcess::slotFinished);

#else

    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &ExifToolProcess::slotFinished);

#endif

    connect(m_process, &QProcess::stateChanged,
            this, &ExifToolProcess::slotStateChanged);

    connect(m_process, &QProcess::errorOccurred,
            this, &ExifToolProcess::slotErrorOccurred);

    connect(m_process, &QProcess::readyReadStandardOutput,
            this, &ExifToolProcess::slotReadyReadStandardOutput);

    connect(m_process, &QProcess::readyReadStandardError,
            this, &ExifToolProcess::slotReadyReadStandardError);
}

ExifToolProcess::~ExifToolProcess()
{
}

void ExifToolProcess::setProgram(const QString& etExePath, const QString& perlExePath)
{
    // Check if ExifTool is starting or running

    if (m_process->state() != QProcess::NotRunning)
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::setProgram(): ExifTool is already running";
        return;
    }

    m_etExePath   = etExePath;
    m_perlExePath = perlExePath;
}

void ExifToolProcess::start()
{
    // Check if ExifTool is starting or running

    if (m_process->state() != QProcess::NotRunning)
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::start(): ExifTool is already running";
        return;
    }

    // Check if Exiftool program exists and have execution permissions

    if (!QFile::exists(m_etExePath) || !(QFile::permissions(m_etExePath) & QFile::ExeUser))
    {
        setProcessErrorAndEmit(QProcess::FailedToStart,
                               QString::fromLatin1("ExifTool does not exists or exec permission is missing"));
        return;
    }

    // If perl path is defined, check if Perl program exists and have execution permissions

    if (!m_perlExePath.isEmpty() && (!QFile::exists(m_perlExePath) ||
        !(QFile::permissions(m_perlExePath) & QFile::ExeUser)))
    {
        setProcessErrorAndEmit(QProcess::FailedToStart,
                               QString::fromLatin1("Perl does not exists or exec permission is missing"));
        return;
    }

    // Prepare command for ExifTool

    QString program = m_etExePath;
    QStringList args;

    if (!m_perlExePath.isEmpty())
    {
        program = m_perlExePath;
        args << m_etExePath;
    }

    //-- Advanced options

    args << QLatin1String("-stay_open");
    args << QLatin1String("true");

    //-- Other options

    args << QLatin1String("-@");
    args << QLatin1String("-");

    // Clear queue before start

    m_cmdQueue.clear();
    m_cmdRunning           = 0;

    // Clear errors

    m_processError         = QProcess::UnknownError;
    m_errorString.clear();

    // Start ExifTool process

    m_writeChannelIsClosed = false;

    m_process->start(program, args, QProcess::ReadWrite);
}

void ExifToolProcess::terminate()
{
    if (m_process->state() == QProcess::Running)
    {
        // If process is in running state, close ExifTool normally

        m_cmdQueue.clear();
        m_process->write("-stay_open\nfalse\n");
        m_process->closeWriteChannel();
        m_writeChannelIsClosed= true;
    }
    else
    {
        // Otherwise, close ExifTool using OS system call
        // (WM_CLOSE [Windows] or SIGTERM [Unix])

        m_process->terminate();
    }
}

void ExifToolProcess::kill()
{
    m_process->kill();
}

bool ExifToolProcess::isRunning() const
{
    return (m_process->state() == QProcess::Running);
}

bool ExifToolProcess::isBusy() const
{
    return (m_cmdRunning ? true : false);
}

qint64 ExifToolProcess::processId() const
{
    return m_process->processId();
}

QProcess::ProcessState ExifToolProcess::state() const
{
    return m_process->state();
}

QProcess::ProcessError ExifToolProcess::error() const
{
    return m_processError;
}

QString ExifToolProcess::errorString() const
{
    return m_errorString;
}

QProcess::ExitStatus ExifToolProcess::exitStatus() const
{
    return m_process->exitStatus();
}

bool ExifToolProcess::waitForStarted(int msecs)
{
    return m_process->waitForStarted(msecs);
}

bool ExifToolProcess::waitForFinished(int msecs)
{
    return m_process->waitForFinished(msecs);
}

int ExifToolProcess::command(const QByteArrayList& args)
{
    if ((m_process->state() != QProcess::Running) ||
        m_writeChannelIsClosed                      ||
        args.isEmpty())
    {
        return 0;
    }

    // ThreadSafe incrementation of _nextCmdId

    m_cmdIdMutex.lock();
    const int cmdId = m_nextCmdId;

    if (m_nextCmdId++ >= CMD_ID_MAX)
    {
        m_nextCmdId = CMD_ID_MIN;
    }

    m_cmdIdMutex.unlock();

    // String representation of m_cmdId with leading zero -> constant size: 10 char

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
    m_cmdQueue.append(command);

    // Exec cmd queue

    execNextCmd();

    return cmdId;
}

void ExifToolProcess::execNextCmd()
{
    if ((m_process->state() != QProcess::Running) ||
        m_writeChannelIsClosed)
    {
        return;
    }

    if (m_cmdRunning || m_cmdQueue.isEmpty())
    {
        return;
    }

    // Clear QProcess buffers

    m_process->readAllStandardOutput();
    m_process->readAllStandardError();

    // Clear internal buffers

    m_outBuff[0]  = QByteArray();
    m_outBuff[1]  = QByteArray();
    m_outAwait[0] = false;
    m_outAwait[1] = false;
    m_outReady[0] = false;
    m_outReady[1] = false;

    // Exec Command

    m_execTimer.start();

    Command command = m_cmdQueue.takeFirst();
    m_cmdRunning    = command.id;

    m_process->write(command.argsStr);
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
    m_cmdRunning = 0;

    emit signalFinished(exitCode, exitStatus);
}

void ExifToolProcess::slotStateChanged(QProcess::ProcessState newState)
{
    emit signalStateChanged(newState);
}

void ExifToolProcess::slotErrorOccurred(QProcess::ProcessError error)
{
    setProcessErrorAndEmit(error, m_process->errorString());
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
    m_process->setReadChannel(channel);

    while (m_process->canReadLine() && !m_outReady[channel])
    {
        QByteArray line = m_process->readLine();

        if (line.endsWith("\r\n"))
        {
            line.remove(line.size()-2, 1); // Remove '\r' character
        }
/*
        qCDebug(DIGIKAM_METAENGINE_LOG) << channel << line;
*/
        if (!m_outAwait[channel])
        {
            if (line.startsWith("{await") && line.endsWith("}\n"))
            {
                m_outAwait[channel] = line.mid(6, line.size() - 8).toInt();
            }

            continue;
        }

        m_outBuff[channel] += line;

        if (line.endsWith("{ready}\n"))
        {
            m_outBuff[channel].chop(8);
            m_outReady[channel] = true;

            break;
        }
    }

    // Check if outputChannel and errorChannel are both ready

    if (!(m_outReady[QProcess::StandardOutput] &&
        m_outReady[QProcess::StandardError]))
    {
        return;
    }

    if (
        (m_cmdRunning != m_outAwait[QProcess::StandardOutput]) ||
        (m_cmdRunning != m_outAwait[QProcess::StandardError])
       )
    {
        qCCritical(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::readOutput: Sync error between CmdID("
                                           << m_cmdRunning
                                           << "), outChannel("
                                           << m_outAwait[0]
                                           << ") and errChannel("
                                           << m_outAwait[1]
                                           << ")";
    }
    else
    {
        emit signalCmdCompleted(m_cmdRunning,
                                m_execTimer.elapsed(),
                                m_outBuff[QProcess::StandardOutput],
                                m_outBuff[QProcess::StandardError]);
    }

    m_cmdRunning = 0; // No command is running

    execNextCmd();    // Exec next command
}

void ExifToolProcess::setProcessErrorAndEmit(QProcess::ProcessError error, const QString& description)
{
    m_processError = error;
    m_errorString  = description;

    emit signalErrorOccurred(error);
}

} // namespace Digikam
