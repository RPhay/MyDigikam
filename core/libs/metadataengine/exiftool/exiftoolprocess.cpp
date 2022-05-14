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
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

QPointer<ExifToolProcess> ExifToolProcess::internalPtr = QPointer<ExifToolProcess>();

ExifToolProcess::ExifToolProcess()
    : QProcess(nullptr),
      d       (new Private(this))
{
    setProcessEnvironment(adjustedEnvironmentForAppImage());
}

ExifToolProcess::~ExifToolProcess()
{
    terminateExifTool();

    delete d;
}

bool ExifToolProcess::isCreated()
{
    return (!internalPtr.isNull());
}

ExifToolProcess* ExifToolProcess::instance()
{
    if (ExifToolProcess::internalPtr.isNull())
    {
        ExifToolProcess::internalPtr = new ExifToolProcess();
    }

    return ExifToolProcess::internalPtr;
}

void ExifToolProcess::changeProgram(const QString& etExePath)
{
    Q_EMIT signalChangeProgram(etExePath);

    qApp->processEvents();

    waitForStarted(1000);
}

QString ExifToolProcess::exifToolProgram() const
{
    return d->etExePath;
}

bool ExifToolProcess::startExifTool()
{
    // Check if ExifTool is starting or running

    if (state() != QProcess::NotRunning)
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

    //-- Define common arguments

    args << QLatin1String("-common_args");

    //-- Use UTF-8 for file names

    args << QLatin1String("-charset");
    args << QLatin1String("filename=UTF8");

    args << QLatin1String("-charset");
    args << QLatin1String("iptc=UTF8");

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

    start(program, args, QProcess::ReadWrite);

    return waitForStarted(1000);
}

void ExifToolProcess::terminateExifTool()
{
    if (state() == QProcess::Running)
    {
        // If process is in running state, close ExifTool normally

        qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::terminate(): send ExifTool shutdown command...";

        d->cmdQueue.clear();
        write(QByteArray("-stay_open\nfalse\n"));
        d->writeChannelIsClosed = true;
        closeWriteChannel();

        if (!waitForFinished(5000))
        {
            // Otherwise, close ExifTool using OS system call
            // (WM_CLOSE [Windows] or SIGTERM [Unix])

            // Console applications on Windows that do not run an event loop,
            // or whose event loop does not handle the WM_CLOSE message,
            // can only be terminated by calling kill().

#ifdef Q_OS_WIN

            killExifTool();

#else

            qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::terminate(): closing ExifTool instance...";

            terminate();

#endif

        }
    }
}

void ExifToolProcess::killExifTool()
{
    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::kill(): shutdown ExifTool instance...";

    kill();
}

bool ExifToolProcess::isRunning() const
{
    return (state() == QProcess::Running);
}

bool ExifToolProcess::isBusy() const
{
    return (d->cmdRunning ? true : false);
}

QProcess::ProcessError ExifToolProcess::error() const
{
    return d->processError;
}

QString ExifToolProcess::errorString() const
{
    return d->errorString;
}

int ExifToolProcess::command(const QByteArrayList& args, Action ac)
{
    if (
        (state() != QProcess::Running) ||
        d->writeChannelIsClosed        ||
        args.isEmpty()
       )
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::command(): cannot process command with ExifTool" << args;

        return 0;
    }

    // ThreadSafe incrementation of d->nextCmdId

    QMutexLocker locker(&d->mutex);

    const int cmdId = d->nextCmdId;

    if (d->nextCmdId++ >= CMD_ID_MAX)
    {
        d->nextCmdId = CMD_ID_MIN;
    }

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

    if (
        cmdStr.contains(QByteArray("-q"))               ||
        cmdStr.toLower().contains(QByteArray("-quiet")) ||
        cmdStr.contains(QByteArray("-T"))               ||
        cmdStr.toLower().contains(QByteArray("-table"))
       )
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

    Q_EMIT signalExecNextCmd();

    return cmdId;
}

void ExifToolProcess::slotExecNextCmd()
{
    d->execNextCmd();
}

void ExifToolProcess::slotStarted()
{
    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool process started";

    emit signalStarted(d->cmdRunning, d->cmdAction);
}

void ExifToolProcess::slotFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool process finished" << exitCode << exitStatus;

    if (d->cmdRunning)
    {
        emit signalFinished(d->cmdRunning, d->cmdAction, exitCode, exitStatus);
    }

    d->cmdRunning = 0;
    d->cmdAction  = NO_ACTION;
}

void ExifToolProcess::slotStateChanged(QProcess::ProcessState newState)
{
    emit signalStateChanged(d->cmdRunning, d->cmdAction, newState);
}

void ExifToolProcess::slotErrorOccurred(QProcess::ProcessError error)
{
    d->setProcessErrorAndEmit(error, errorString());
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

    if (
        !d->perlExePath.isEmpty()                            &&
        (!QFile::exists(d->perlExePath)                      ||
        !(QFile::permissions(d->perlExePath) & QFile::ExeUser))
       )
    {
        d->setProcessErrorAndEmit(QProcess::FailedToStart,
                                  QString::fromLatin1("Perl does not exists or exec permission is missing"));
        return false;
    }

    return true;
}

void ExifToolProcess::setupConnections()
{
    connect(this, &QProcess::started,
            this, &ExifToolProcess::slotStarted);

#if QT_VERSION >= 0x060000

    connect(this, &QProcess::finished,
            this, &ExifToolProcess::slotFinished);

#else

    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &ExifToolProcess::slotFinished);

#endif

    connect(this, &QProcess::stateChanged,
            this, &ExifToolProcess::slotStateChanged);

    connect(this, &QProcess::errorOccurred,
            this, &ExifToolProcess::slotErrorOccurred);

    connect(this, &QProcess::readyReadStandardOutput,
            this, &ExifToolProcess::slotReadyReadStandardOutput);

    connect(this, &QProcess::readyReadStandardError,
            this, &ExifToolProcess::slotReadyReadStandardError);

    connect(this, &ExifToolProcess::signalExecNextCmd,
            this, &ExifToolProcess::slotExecNextCmd,
            Qt::QueuedConnection);

    connect(this, &ExifToolProcess::signalChangeProgram,
            this, &ExifToolProcess::slotChangeProgram,
            Qt::QueuedConnection);

    connect(MetaEngineSettings::instance(), SIGNAL(signalSettingsChanged()),
            this, SLOT(slotApplySettingsAndStart()),
            Qt::QueuedConnection);
}

void ExifToolProcess::setExifToolProgram(const QString& etExePath)
{
    d->etExePath = etExePath;

    if      (d->etExePath.isEmpty())
    {
        d->etExePath = exifToolBin();
    }
    else if (QFileInfo(d->etExePath).isDir())
    {
        if (!d->etExePath.endsWith(QLatin1Char('/')))
        {
            d->etExePath.append(QLatin1Char('/'));
        }

        d->etExePath.append(exifToolBin());
    }
}

void ExifToolProcess::slotChangeProgram(const QString& etExePath)
{
    QString et(exifToolProgram());

    setExifToolProgram(etExePath);

    if (isRunning()             &&
        (et == exifToolProgram()))
    {
        return;
    }

    terminateExifTool();

    if (!startExifTool())
    {
        killExifTool();

        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifTool process cannot be started ("
                                          << exifToolProgram() << ")";
    }
}

void ExifToolProcess::slotApplySettingsAndStart()
{
    setExifToolProgram(MetaEngineSettings::instance()->settings().exifToolPath);

    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool path:" << exifToolProgram();

    if (state() != QProcess::NotRunning)
    {
        return;
    }

    terminateExifTool();

    if (!startExifTool())
    {
        killExifTool();

        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifTool process cannot be started ("
                                          << exifToolProgram() << ")";
    }
}

} // namespace Digikam
