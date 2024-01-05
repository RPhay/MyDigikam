/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-04-13
 * Description : Generic process launcher with a capture of console output
 *
 * SPDX-FileCopyrightText: 2011-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "processlauncher.h"

// Qt includes

#include <QByteArray>
#include <QProcess>
#include <QElapsedTimer>
#include <QProcessEnvironment>

// Local includes

#include "digikam_globals.h"
#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN ProcessLauncher::Private
{
public:

    Private() = default;

public:

    QProcess*   proc     = nullptr;
    QStringList args;
    QString     prog;
    QString     dir;
    int         exitCode = 0;
    int         timeOut  = 30000;           ///< in milli-seconds;
    qint64      elapsed  = 0;
};

ProcessLauncher::ProcessLauncher(QObject* const parent)
    : QObject(parent),
      d      (new Private)
{
}

ProcessLauncher::~ProcessLauncher()
{
    d->proc->kill();
    delete d;
}

void ProcessLauncher::setProgram(const QString& prog)
{
    d->prog = prog;
}

void ProcessLauncher::setArguments(const QStringList& args)
{
    d->args = args;
}

void ProcessLauncher::setWorkingDirectory(const QString& dir)
{
    d->dir = dir;
}

void ProcessLauncher::setTimeOut(int msecs)
{
    d->timeOut = msecs;
}

int ProcessLauncher::exitCode() const
{
    return d->exitCode;
}

qint64 ProcessLauncher::elapsedTime() const
{
    return d->elapsed;
}

bool ProcessLauncher::startProcess() const
{
    QString     prog;
    QStringList args;

    d->proc = new QProcess(parent());
    d->proc->setProcessChannelMode(QProcess::MergedChannels);
    d->proc->setWorkingDirectory(d->dir);

    QProcessEnvironment env = adjustedEnvironmentForAppImage();
    d->proc->setProcessEnvironment(env);

    connect(d->proc, SIGNAL(readyRead()),
            this, SLOT(slotReadyRead()));

    QElapsedTimer etimer;
    etimer.start();

#ifdef Q_OS_WIN

    prog = QLatin1String("cmd");
    args = QStringList() << QLatin1String("/c") << d->prog << d->args;

#else   // Linux && MacOS

    prog = d->prog;
    args = d->args;

#endif

    qCInfo(DIGIKAM_GENERAL_LOG) << "=== Starting process:" << prog << args;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

    d->proc->start(prog, args);

#else

    d->proc->setProgram(prog);
    d->proc->setArguments(args);
    d->proc->start();

#endif

    if (d->proc->waitForStarted(d->timeOut))
    {
        bool timedOut = !d->proc->waitForFinished(d->timeOut);
        d->exitCode   = d->proc->exitCode();
        d->elapsed    = etimer.elapsed();

        qCInfo(DIGIKAM_GENERAL_LOG) << "=== Process execution is complete!";
        qCInfo(DIGIKAM_GENERAL_LOG) << "> Process timed-out        :" << timedOut;
        qCInfo(DIGIKAM_GENERAL_LOG) << "> Process exit code        :" << d->exitCode;
        qCInfo(DIGIKAM_GENERAL_LOG) << "> Process elasped time (ms):" << d->elapsed;

        if (timedOut)
        {
            return false;
        }
    }
    else
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "=== Process execution failed!";

        return false;
    }

    return true;
}

void ProcessLauncher::slotReadyRead()
{
    QByteArray data = d->proc->readAll();

    if (!data.isEmpty())
    {
        QString txt       = QString::fromLocal8Bit(data.data(), data.size());
        QStringList lines = txt.split(QLatin1Char('\n'));

        Q_FOREACH (const QString& str, lines)
        {
            if (!str.isEmpty())
            {
                qCDebug(DIGIKAM_GENERAL_LOG)

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

                   .noquote()

#endif

                   << str;
            }
        }
    }
}

} // namespace Digikam

#include "moc_processlauncher.cpp"
