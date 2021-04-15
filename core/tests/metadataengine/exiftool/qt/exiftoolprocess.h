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

#ifndef DIGIKAM_EXIFTOOL_PROCESS_H
#define DIGIKAM_EXIFTOOL_PROCESS_H

// Qt Core

#include <QObject>
#include <QElapsedTimer>
#include <QProcess>
#include <QMutex>
#include <QList>
#include <QString>
#include <QByteArray>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT ExifToolProcess : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructs a ExifToolProcess object with the given parent.
     */
    explicit ExifToolProcess(QObject* const parent = nullptr);

    /**
     * Destructs the ExifToolProcess object, i.e., killing the process.
     * Note that this function will not return until the process is terminated.
     */
    ~ExifToolProcess();

public:

    /**
     * IMPORTANT: This function must be called before start().
     */
    void setProgram(const QString& etExePath, const QString& perlExePath= QString());

    /**
     * Starts exiftool in a new process.
     */
    void start();

public Q_SLOTS:

    /**
     * Attempts to terminate the process.
     */
    void terminate();

    /**
     * Kills the current process, causing it to exit immediately.
     * On Windows, kill() uses TerminateProcess, and on Unix and macOS,
     * the SIGKILL signal is sent to the process.
     */
    void kill();

public:

    /**
     * Return true if ExifToolProcess is running (process state == Running)
     */
    bool isRunning()                        const;

    /**
     * Return true if a command is running
     */
    bool isBusy()                           const;

    /**
     * Returns the native process identifier for the running process, if available.
     * If no process is currently running, 0 is returned.
     */
    qint64                 processId()      const;

    /**
     * Returns the current state of the process.
     */
    QProcess::ProcessState state()          const;

    /**
     * Returns the type of error that occurred last.
     */
    QProcess::ProcessError error()          const;

    /**
     * Returns an error message
     */
    QString                errorString()    const;

    /**
     * Returns the exit status of the last process that finished.
     */
    QProcess::ExitStatus   exitStatus()     const;

    int                    exitCode()       const;

    /**
     * Blocks until the process has started and the started() signal has been emitted,
     * or until msecs milliseconds have passed.
     */
    bool waitForStarted(int msecs = 30000);

    /**
     * Blocks until the process has finished and the finished() signal has been emitted,
     * or until msecs milliseconds have passed.
     */
    bool waitForFinished(int msecs = 30000);

    /**
     * Send a command to exiftool process
     * Return 0: ExitTool not running, write channel is closed or args is empty
     */
    int command(const QByteArrayList& args);

private:

    void execNextCmd();

private Q_SLOTS:

    void slotStarted();
    void slotFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void slotStateChanged(QProcess::ProcessState newState);
    void slotErrorOccurred(QProcess::ProcessError error);
    void slotReadyReadStandardOutput();
    void slotReadyReadStandardError();

private:

    void readOutput(const QProcess::ProcessChannel channel);
    void setProcessErrorAndEmit(QProcess::ProcessError error, const QString& description);

Q_SIGNALS:

    void signalStarted();
    void signalFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void signalStateChanged(QProcess::ProcessState newState);
    void signalErrorOccurred(QProcess::ProcessError error);

    void signalCmdCompleted(int cmdId,
                            int execTime,
                            const QByteArray& cmdOutputChannel,
                            const QByteArray& cmdErrorChannel);

private:

    struct Command
    {
        Command()
            : id(0)
        {
        }

        int        id;
        QByteArray argsStr;
    };

private:

    static       QMutex     _cmdIdMutex;
    static const int        CMD_ID_MIN;
    static const int        CMD_ID_MAX;
    static       int        _nextCmdId;     ///< Unique identifier, even in a multi-instances or multi-thread environment

    QString                 _etExePath;
    QString                 _perlExePath;
    QProcess*               _process;

    QElapsedTimer           _execTimer;
    QList<Command>          _cmdQueue;
    int                     _cmdRunning;

    int                     _outAwait[2];   ///< [0] StandardOutput | [1] ErrorOutput
    bool                    _outReady[2];   ///< [0] StandardOutput | [1] ErrorOutput
    QByteArray              _outBuff[2];    ///< [0] StandardOutput | [1] ErrorOutput

    bool                    _writeChannelIsClosed;

    QProcess::ProcessError  _processError;
    QString                 _errorString;
};

} // namespace Digikam

#endif // DIGIKAM_EXIFTOOL_PROCESS_H
