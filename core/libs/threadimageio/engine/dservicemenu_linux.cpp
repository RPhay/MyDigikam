/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-05-08
 * Description : Service menu operation methods
 *
 * SPDX-FileCopyrightText: 2014-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dservicemenu.h"

// Qt includes

#include <QProcess>
#include <QMimeType>
#include <QMimeDatabase>
#include <QStandardPaths>
#include <QRegularExpression>

// KDE includes

#include <kservice_version.h>

#if KSERVICE_VERSION > QT_VERSION_CHECK(5, 81, 0)
#   include <kapplicationtrader.h>
#else
#   include <kmimetypetrader.h>
#endif

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"
#include "digikam_globals.h"

namespace Digikam
{

bool DServiceMenu::runFiles(const KService::Ptr& service,
                            const QList<QUrl>& urls)
{
    return (runFiles(service->exec(), urls, service));
}

bool DServiceMenu::runFiles(const QString& appCmd,
                            const QList<QUrl>& urls,
                            const KService::Ptr& service)
{
    QRegularExpression split(QLatin1String(" +(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)"));
    QStringList cmdList = appCmd.split(split, QT_SKIP_EMPTY_PARTS);
    QList<QUrl> urlList = urls;

    if (cmdList.isEmpty() || urlList.isEmpty())
    {
        return false;
    }

    if (!appCmd.contains(QLatin1String("%f"), Qt::CaseInsensitive) &&
        !appCmd.contains(QLatin1String("%u"), Qt::CaseInsensitive) &&
        !appCmd.contains(QLatin1String("%d"), Qt::CaseInsensitive))
    {
        cmdList << QLatin1String("%f");
    }

    QString exec;
    QString name;
    QString icon;
    QString term;

    QStringList dirs;
    QStringList files;
    QStringList cmdArgs;
    QStringList termOpts;

    bool useTerminal = false;
    bool openNewRun  = false;

    if (service)
    {
        name = service->desktopEntryName();
        icon = service->icon();

#ifdef Q_OS_LINUX

        if (service->terminal())
        {
            termOpts = service->terminalOptions().split(split, QT_SKIP_EMPTY_PARTS);
            term     = QStandardPaths::findExecutable(QLatin1String("konsole"));

            if (term.isEmpty())
            {
                term = QStandardPaths::findExecutable(QLatin1String("xterm"));
                termOpts.replaceInStrings(QLatin1String("--noclose"),
                                          QLatin1String("-hold"));
            }

            useTerminal = !term.isEmpty();
        }

#endif // Q_OS_LINUX

    }

    QProcess* const process = new QProcess();
    QProcessEnvironment env = adjustedEnvironmentForAppImage();

    Q_FOREACH (const QUrl& url, urlList)
    {
        dirs  << url.adjusted(QUrl::RemoveFilename).toLocalFile();
        files << url.toLocalFile();
    }

    Q_FOREACH (const QString& cmdString, cmdList)
    {
        QString cmd = cmdString;

        if (cmd.startsWith(QLatin1Char('"')) && cmd.endsWith(QLatin1Char('"')))
        {
            cmd.remove(0, 1).chop(1);
        }

        if (exec.isEmpty() && cmd.contains(QLatin1Char('=')))
        {
            QStringList envList = cmd.split(QLatin1Char('='), QT_SKIP_EMPTY_PARTS);

            if (envList.count() == 2)
            {
                env.insert(envList[0], envList[1]);
            }

            continue;
        }
        else if (exec.isEmpty())
        {
            exec = cmd;
            continue;
        }

        if      (cmd == QLatin1String("%c"))
        {
            cmdArgs << name;
        }
        else if (cmd == QLatin1String("%i"))
        {
            cmdArgs << icon;
        }
        else if (cmd == QLatin1String("%f"))
        {
            cmdArgs << files.first();
            openNewRun = true;
        }
        else if (cmd == QLatin1String("%F"))
        {
            cmdArgs << files;
        }
        else if (cmd == QLatin1String("%u"))
        {
            cmdArgs << files.first();
            openNewRun = true;
        }
        else if (cmd == QLatin1String("%U"))
        {
            cmdArgs << files;
        }
        else if (cmd == QLatin1String("%d"))
        {
            cmdArgs << dirs.first();
            openNewRun = true;
        }
        else if (cmd == QLatin1String("%D"))
        {
            cmdArgs << dirs;
        }
        else
        {
            cmdArgs << cmd;
        }
    }

    process->setProcessEnvironment(env);

    if (useTerminal)
    {
        termOpts << QLatin1String("-e") << exec << cmdArgs;
        process->start(term, termOpts);
    }
    else
    {
        process->start(exec, cmdArgs);
    }

    bool ret = true;
    ret     &= process->waitForStarted();

    if (openNewRun)
    {
        urlList.removeFirst();

        if (!urlList.isEmpty())
        {
            ret &= runFiles(appCmd, urlList, service);
        }
    }

    return ret;
}

KService::List DServiceMenu::servicesForOpenWith(const QList<QUrl>& urls)
{
    // This code is inspired by KonqMenuActions:
    // kdebase/apps/lib/konq/konq_menuactions.cpp

    QStringList    mimeTypes;
    KService::List offers;

    Q_FOREACH (const QUrl& item, urls)
    {
        const QString mimeType = QMimeDatabase().mimeTypeForFile(item.toLocalFile(),
                                                                 QMimeDatabase::MatchExtension).name();

        if (!mimeTypes.contains(mimeType))
        {
            mimeTypes << mimeType;
        }
    }

    if (!mimeTypes.isEmpty())
    {
        // Query trader

        const QString firstMimeType      = mimeTypes.takeFirst();
        const QString constraintTemplate = QLatin1String("'%1' in ServiceTypes");
        QStringList constraints;

        Q_FOREACH (const QString& mimeType, mimeTypes)
        {
            constraints << constraintTemplate.arg(mimeType);
        }

#if KSERVICE_VERSION > QT_VERSION_CHECK(5, 81, 0)
        offers = KApplicationTrader::queryByMimeType(firstMimeType);
#else
        offers = KMimeTypeTrader::self()->query(firstMimeType,
                                                QLatin1String("Application"),
                                                constraints.join(QLatin1String(" and ")));
#endif

        // remove duplicate service entries

        QSet<QString> seenApps;

        for (KService::List::iterator it = offers.begin() ; it != offers.end() ; )
        {
            const QString appName((*it)->name());

            if (!seenApps.contains(appName))
            {
                seenApps.insert(appName);
                ++it;
            }
            else
            {
                it = offers.erase(it);
            }
        }
    }

    return offers;
}

} // namespace Digikam
