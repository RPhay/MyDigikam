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

#include "exiftoolprocess.h"

// Qt includes

#include <QFile>
#include <QElapsedTimer>
#include <QList>
#include <QByteArray>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"

namespace Digikam
{

class Q_DECL_HIDDEN ExifToolProcess::Private
{
public:

    struct Command
    {
        Command()
          : id(0)
        {
        }

        int        id;
        QByteArray argsStr;
    };

public:

    explicit Private()
      : process             (nullptr),
        cmdRunning          (0),
        writeChannelIsClosed(true),
        processError        (QProcess::UnknownError)
    {
        outAwait[0] = false;
        outAwait[1] = false;
        outReady[0] = false;
        outReady[1] = false;
    }

public:

    QString                etExePath;
    QString                perlExePath;
    QProcess*              process;

    QElapsedTimer          execTimer;
    QList<Command>         cmdQueue;
    int                    cmdRunning;

    int                    outAwait[2];             ///< [0] StandardOutput | [1] ErrorOutput
    bool                   outReady[2];             ///< [0] StandardOutput | [1] ErrorOutput
    QByteArray             outBuff[2];              ///< [0] StandardOutput | [1] ErrorOutput

    bool                   writeChannelIsClosed;

    QProcess::ProcessError processError;
    QString                errorString;

public:

    static const int       CMD_ID_MIN  = 1;
    static const int       CMD_ID_MAX  = 2000000000;

    static int             s_nextCmdId;               ///< Unique identifier, even in a multi-instances or multi-thread environment
    static QMutex          s_cmdIdMutex;
};

QMutex ExifToolProcess::Private::s_cmdIdMutex;
int    ExifToolProcess::Private::s_nextCmdId = ExifToolProcess::Private::CMD_ID_MIN;

ExifToolProcess::ExifToolProcess(QObject* const parent)
    : QObject(parent),
      d      (new Private)
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
}

QString ExifToolProcess::program() const
{
    return d->etExePath;
}

void ExifToolProcess::start()
{
    // Check if ExifTool is starting or running

    if (d->process->state() != QProcess::NotRunning)
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::start(): ExifTool is already running";
        return;
    }

    // Check if Exiftool program exists and have execution permissions

    if (!QFile::exists(d->etExePath) ||
        !(QFile::permissions(d->etExePath) & QFile::ExeUser))
    {
        setProcessErrorAndEmit(QProcess::FailedToStart,
                               QString::fromLatin1("ExifTool does not exists or exec permission is missing"));
        return;
    }

    // If perl path is defined, check if Perl program exists and have execution permissions

    if (!d->perlExePath.isEmpty() && (!QFile::exists(d->perlExePath) ||
        !(QFile::permissions(d->perlExePath) & QFile::ExeUser)))
    {
        setProcessErrorAndEmit(QProcess::FailedToStart,
                               QString::fromLatin1("Perl does not exists or exec permission is missing"));
        return;
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

    args << QLatin1String("-charset filename=utf8");
    args << QLatin1String("-stay_open");
    args << QLatin1String("true");

    //-- Other options

    args << QLatin1String("-@");
    args << QLatin1String("-");

    // Clear queue before start

    d->cmdQueue.clear();
    d->cmdRunning           = 0;

    // Clear errors

    d->processError         = QProcess::UnknownError;
    d->errorString.clear();

    // Start ExifTool process

    d->writeChannelIsClosed = false;

    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::start(): create new ExifTool instance...";

    d->process->start(program, args, QProcess::ReadWrite);
}

void ExifToolProcess::terminate()
{
    if (d->process->state() == QProcess::Running)
    {
        // If process is in running state, close ExifTool normally

        qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::terminate(): send ExifTool shutdown command...";

        d->cmdQueue.clear();
        d->process->write(QByteArray("-stay_open\nfalse\n"));
        d->process->closeWriteChannel();
        d->writeChannelIsClosed = true;
    }
    else
    {
        // Otherwise, close ExifTool using OS system call
        // (WM_CLOSE [Windows] or SIGTERM [Unix])

        qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::terminate(): closing ExifTool instance...";

        d->process->terminate();
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

int ExifToolProcess::command(const QByteArrayList& args)
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
    d->cmdQueue.append(command);

    // Exec cmd queue

    execNextCmd();

    return cmdId;
}

void ExifToolProcess::execNextCmd()
{
    if ((d->process->state() != QProcess::Running) ||
        d->writeChannelIsClosed)
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::execNextCmd(): ExifTool is not running";
        return;
    }

    if (d->cmdRunning || d->cmdQueue.isEmpty())
    {
        return;
    }

    // Clear QProcess buffers

    d->process->readAllStandardOutput();
    d->process->readAllStandardError();

    // Clear internal buffers

    d->outBuff[0]            = QByteArray();
    d->outBuff[1]            = QByteArray();
    d->outAwait[0]           = false;
    d->outAwait[1]           = false;
    d->outReady[0]           = false;
    d->outReady[1]           = false;

    // Exec Command

    d->execTimer.start();

    Private::Command command = d->cmdQueue.takeFirst();
    d->cmdRunning            = command.id;

    d->process->write(command.argsStr);
}

void ExifToolProcess::slotStarted()
{
    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool process started";
    emit signalStarted();
}

void ExifToolProcess::slotFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool process finished" << exitCode << exitStatus;
    d->cmdRunning = 0;

    emit signalFinished(exitCode, exitStatus);
}

void ExifToolProcess::slotStateChanged(QProcess::ProcessState newState)
{
    emit signalStateChanged(newState);
}

void ExifToolProcess::slotErrorOccurred(QProcess::ProcessError error)
{
    setProcessErrorAndEmit(error, d->process->errorString());
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
    d->process->setReadChannel(channel);

    while (d->process->canReadLine() && !d->outReady[channel])
    {
        QByteArray line = d->process->readLine();

        if (line.endsWith(QByteArray("\r\n")))
        {
            line.remove(line.size() - 2, 1); // Remove '\r' character
        }
/*
        qCDebug(DIGIKAM_METAENGINE_LOG) << channel << line;
*/
        if (!d->outAwait[channel])
        {
            if (line.startsWith(QByteArray("{await")) && line.endsWith(QByteArray("}\n")))
            {
                d->outAwait[channel] = line.mid(6, line.size() - 8).toInt();
            }

            continue;
        }

        d->outBuff[channel] += line;

        if (line.endsWith(QByteArray("{ready}\n")))
        {
            d->outBuff[channel].chop(8);
            d->outReady[channel] = true;

            break;
        }
    }

    // Check if outputChannel and errorChannel are both ready

    if (!(d->outReady[QProcess::StandardOutput] &&
        d->outReady[QProcess::StandardError]))
    {
/*
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::readOutput(): ExifTool read channels are not ready";
*/
        return;
    }

    if (
        (d->cmdRunning != d->outAwait[QProcess::StandardOutput]) ||
        (d->cmdRunning != d->outAwait[QProcess::StandardError])
       )
    {
        qCCritical(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::readOutput: Sync error between CmdID("
                                           << d->cmdRunning
                                           << "), outChannel("
                                           << d->outAwait[0]
                                           << ") and errChannel("
                                           << d->outAwait[1]
                                           << ")";
    }
    else
    {
        qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::readOutput(): ExifTool command completed with elapsed time:"
                                        << d->execTimer.elapsed();
        emit signalCmdCompleted(d->cmdRunning,
                                d->execTimer.elapsed(),
                                d->outBuff[QProcess::StandardOutput],
                                d->outBuff[QProcess::StandardError]);
    }

    d->cmdRunning = 0; // No command is running

    execNextCmd();     // Exec next command
}

void ExifToolProcess::setProcessErrorAndEmit(QProcess::ProcessError error, const QString& description)
{
    d->processError = error;
    d->errorString  = description;

    emit signalErrorOccurred(error);
}

} // namespace Digikam
