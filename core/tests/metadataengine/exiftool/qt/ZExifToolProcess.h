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

#ifndef DIGIKAM_Z_EXIFTOOL_PROCESS_H
#define DIGIKAM_Z_EXIFTOOL_PROCESS_H

// Qt Core

#include <QObject>
#include <QElapsedTimer>
#include <QProcess>
#include <QMutex>

class ZExifToolProcess : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructs a ZExifToolProcess object with the given parent.
     */
    explicit ZExifToolProcess(QObject* const parent = nullptr);

    /**
     * Destructs the ZExifToolProcess object, i.e., killing the process.
     * Note that this function will not return until the process is terminated.
     */
    ~ZExifToolProcess();

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
     * On Windows, kill() uses TerminateProcess, and on Unix and macOS, the SIGKILL signal is sent to the process.
     */
    void kill();

public:

    /**
     * Return true if ZExifToolProcess is running (process state == Running)
     */
    bool isRunning()                        const;

    /**
     * Return true if a command is running
     */
    bool isBusy()                           const;

    /**
     * Returns the native process identifier for the running process, if available. If no process is currently running, 0 is returned.
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
     * Blocks until the process has started and the started() signal has been emitted, or until msecs milliseconds have passed.
     */
    bool waitForStarted(int msecs = 30000);

    /**
     * Blocks until the process has finished and the finished() signal has been emitted, or until msecs milliseconds have passed.
     */
    bool waitForFinished(int msecs = 30000);

/*
    bool waitForCompleted(int msecs = 30000); // TODO: Need to be implemented
*/
    int command(const QByteArrayList& args);

private:

    void execNextCmd();

private Q_SLOTS:

    void onStarted();
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onStateChanged(QProcess::ProcessState newState);
    void onErrorOccurred(QProcess::ProcessError error);
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();

private:

    void readOutput(const QProcess::ProcessChannel channel);
    void setProcessErrorAndEmit(QProcess::ProcessError error, const QString& description);

Q_SIGNALS:

    void started();
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void stateChanged(QProcess::ProcessState newState);
    void errorOccurred(QProcess::ProcessError error);

    void cmdCompleted(int cmdId,
                      int execTime,
                      const QByteArray& cmdOutputChannel,
                      const QByteArray& cmdErrorChannel);

private:

    struct Command
    {
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

#endif // DIGIKAM_Z_EXIFTOOL_PROCESS_H
