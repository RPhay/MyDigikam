/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-12-31
 * Description : Online version checker
 *
 * Copyright (C) 2020-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "onlineversionchecker.h"

// Qt includes

#include <QLocale>
#include <QSysInfo>
#include <QTextStream>
#include <QNetworkAccessManager>

// KDE includes

#include <klocalizedstring.h>
#include <kconfiggroup.h>
#include <ksharedconfig.h>

// Local includes

#include "digikam_version.h"
#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN OnlineVersionChecker::Private
{
public:

    explicit Private()
      : preRelease  (false),
        releaseNotes(false),
        redirects   (0),
        curVersion  (QLatin1String(digikam_version_short)),
        curBuildDt  (digiKamBuildDate()),
        reply       (nullptr),
        manager     (nullptr)
    {
    }

    bool                   preRelease;          ///< Flag to check pre-releases
    bool                   releaseNotes;        ///< Flag to indicate release notes downloading
    int                    redirects;           ///< Count of redirected url

    QString                curVersion;          ///< Current application version string
    QString                preReleaseFileName;  ///< Pre-release file name get from remote server

    QDateTime              curBuildDt;          ///< Current application build date

    QNetworkReply*         reply;               ///< Current network request reply
    QNetworkAccessManager* manager;             ///< Network manager instance
};

OnlineVersionChecker::OnlineVersionChecker(QObject* const parent, bool checkPreRelease)
    : QObject(parent),
      d      (new Private)
{
    d->preRelease = checkPreRelease;
    d->manager    = new QNetworkAccessManager(this);
    d->manager->setRedirectPolicy(QNetworkRequest::ManualRedirectPolicy);

    connect(d->manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slotDownloadFinished(QNetworkReply*)));
}

OnlineVersionChecker::~OnlineVersionChecker()
{
    cancelCheck();
    delete d;
}

void OnlineVersionChecker::cancelCheck()
{
    if (d->reply)
    {
        d->reply->abort();
    }
}

void OnlineVersionChecker::setCurrentVersion(const QString& version)
{
    d->curVersion = version;
}

void OnlineVersionChecker::setCurrentBuildDate(const QDateTime& dt)
{
    d->curBuildDt = dt;
}

QString OnlineVersionChecker::preReleaseFileName() const
{
    return d->preReleaseFileName;
}

QString OnlineVersionChecker::lastCheckDate()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("Updates"));
    QDateTime dt              = group.readEntry(QLatin1String("Last Check For New Version"), QDateTime());
    QString dts               = QLocale().toString(dt, QLocale::ShortFormat);

    return (!dts.isEmpty() ? dts : i18nc("@info: check of new online version never done yet", "never"));
}

void OnlineVersionChecker::checkForNewVersion()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("Updates"));
    group.writeEntry(QLatin1String("Last Check For New Version"), QDateTime::currentDateTime());
    config->sync();

    QUrl rUrl;

    if (d->preRelease)
    {
        rUrl = QUrl(QLatin1String("https://files.kde.org/digikam/FILES"));
    }
    else
    {
        rUrl = QUrl(QLatin1String("https://invent.kde.org/websites/digikam-org/-/raw/master/data/release.yml"));
    }

    d->redirects    = 0;
    d->releaseNotes = false;
    download(rUrl);
}

void OnlineVersionChecker::downloadReleaseNotes(const QString& version)
{
    QUrl rUrl;

    if (version.isEmpty())
    {
        rUrl = QUrl(QLatin1String("https://invent.kde.org/graphics/digikam/-/raw/master/NEWS"));
    }
    else
    {
        rUrl = QUrl(QString::fromLatin1("https://invent.kde.org/graphics/digikam/-/raw/master/project/NEWS.%1").arg(version));
    }

    d->redirects    = 0;
    d->releaseNotes = true;
    download(rUrl);
}

void OnlineVersionChecker::download(const QUrl& url)
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "Downloading: " << url;

    d->redirects++;
    d->reply = d->manager->get(QNetworkRequest(url));

    connect(d->reply, SIGNAL(sslErrors(QList<QSslError>)),
            d->reply, SLOT(ignoreSslErrors()));

    if (d->reply->error())
    {
        emit signalNewVersionCheckError(d->reply->errorString());
    }
}

void OnlineVersionChecker::slotDownloadFinished(QNetworkReply* reply)
{
    if (reply != d->reply)
    {
        return;
    }

    // mark for deletion

    reply->deleteLater();
    d->reply = nullptr;

    if ((reply->error() != QNetworkReply::NoError)             &&
        (reply->error() != QNetworkReply::InsecureRedirectError))
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Error: " << reply->errorString();
        emit signalNewVersionCheckError(reply->errorString());

        return;
    }

    QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

    if (redirectUrl.isValid()         &&
        (reply->url() != redirectUrl) &&
        (d->redirects < 10))
    {
        download(redirectUrl);

        return;
    }

    QString data     = QString::fromUtf8(reply->readAll());

    if (data.isEmpty())
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "No data returned from the remote connection.";
        emit signalNewVersionCheckError(i18n("No data returned from the remote connection."));

        return;
    }

    if (d->releaseNotes)
    {
        emit signalReleaseNotesData(data);
        d->releaseNotes = false;

        return;
    }

    if (d->preRelease)
    {
        // NOTE: pre-release files list from files.kde.org is a simple text
        // file of remote directory contents where we will extract build date string.

        QString arch;
        QString ext;

        if (!OnlineVersionChecker::bundleProperties(arch, ext))
        {
            emit signalNewVersionCheckError(i18n("Unsupported Architecture: %1", QSysInfo::buildAbi()));

            qCDebug(DIGIKAM_GENERAL_LOG) << "Unsupported architecture";

            return;
        }

        QTextStream in(&data);
        QString line;

        do
        {
            line = in.readLine();

            if (line.contains(ext) && line.contains(arch))
            {
                d->preReleaseFileName = line.simplified();
                break;
            }
        }
        while (!line.isNull());

        QStringList sections = d->preReleaseFileName.split(QLatin1Char('-'));

        if (sections.size() < 4)
        {
            qCWarning(DIGIKAM_GENERAL_LOG) << "Invalid file name format returned from the remote connection.";
            emit signalNewVersionCheckError(i18n("Invalid file name format returned from the remote connection."));

            return;
        }

        // Tow possibles places exists where to find the date in file name.

        // Check 1 - the fila name include a pre release suffix as -beta or -rc 

        QString dtStr      = sections[3];
        QDateTime onlineDt = QDateTime::fromString(dtStr, QLatin1String("yyyyMMddTHHmmss"));
        onlineDt.setTimeSpec(Qt::UTC);

        if (!onlineDt.isValid())
        {
            // Check 2 - the file name do not include a pre release suffix

            dtStr    = sections[2];
            onlineDt = QDateTime::fromString(dtStr, QLatin1String("yyyyMMddTHHmmss"));
            onlineDt.setTimeSpec(Qt::UTC);
        }

        if (!onlineDt.isValid())
        {
            qCWarning(DIGIKAM_GENERAL_LOG) << "Invalid pre-release date from the remote list.";
            emit signalNewVersionCheckError(i18n("Invalid pre-release date from the remote list."));

            return;
        }

        qCDebug(DIGIKAM_GENERAL_LOG) << "Pre-release file Name :" << preReleaseFileName();
        qCDebug(DIGIKAM_GENERAL_LOG) << "Pre-release build date:" << onlineDt;
        qCDebug(DIGIKAM_GENERAL_LOG) << "Current build date:"     << d->curBuildDt;

        if (onlineDt > d->curBuildDt)
        {
            emit signalNewVersionAvailable(dtStr);            // Forward pre-release build date from remote file.
        }
        else
        {
            emit signalNewVersionCheckError(QString());             // Report error to GUI
        }
    }
    else
    {
        // NOTE: stable files list from digikam.org is a Yaml config file where we will extract version string.

        QString tag            = QLatin1String("version: ");
        int start              = data.indexOf(tag) + tag.size();
        QString rightVer       = data.mid(start);
        int end                = rightVer.indexOf(QLatin1Char('\n'));
        QString onlineVer      = rightVer.mid(0, end);
        QStringList onlineVals = onlineVer.split(QLatin1Char('.'));

        if (onlineVals.size() != 3)
        {
            qCWarning(DIGIKAM_GENERAL_LOG) << "Invalid format returned from the remote connection.";
            emit signalNewVersionCheckError(i18n("Invalid format returned from the remote connection."));

            return;
        }

        QStringList currVals = d->curVersion.split(QLatin1Char('.'));

        qCDebug(DIGIKAM_GENERAL_LOG) << "Online Version:" << onlineVer;

        if (digiKamMakeIntegerVersion(onlineVals[0].toInt(),
                                      onlineVals[1].toInt(),
                                      onlineVals[2].toInt()) >
            digiKamMakeIntegerVersion(currVals[0].toInt(),
                                      currVals[1].toInt(),
                                      currVals[2].toInt()))
        {
            emit signalNewVersionAvailable(onlineVer);
        }
        else
        {
            emit signalNewVersionCheckError(QString());
        }
    }
}

bool OnlineVersionChecker::bundleProperties(QString& arch, QString& ext)
{

#if defined Q_OS_MACOS

    ext  = QLatin1String("pkg");

#   if defined Q_PROCESSOR_X86_64

    arch = QLatin1String("x86-64");

#   elif defined Q_PROCESSOR_ARM

/*  Native Apple silicon is not yet supported
    arch = QLatin1String("arm-64");
*/

    // NOTE: Intel 64 version work fine with Apple Rosetta 2 emulator.
    arch = QLatin1String("x86-64");

#   endif

#endif

#if defined Q_OS_WIN

    ext  = QLatin1String("exe");

#   if defined Q_PROCESSOR_X86_64

    arch = QLatin1String("Win64");

#   elif defined Q_PROCESSOR_ARM

/*  Windows Arm is not yet supported
    arch = QLatin1String("arm-64");
*/

#   elif defined Q_PROCESSOR_X86_32

    // 32 bits is not supported

#   endif

#endif

#if defined Q_OS_LINUX

    ext  = QLatin1String("appimage");

#   ifdef Q_PROCESSOR_X86_64

    arch = QLatin1String("x86-64");

#   elif defined Q_PROCESSOR_ARM

/*  Linux Arm is not yet supported
    arch = QLatin1String("arm-64");
*/

#   elif defined Q_PROCESSOR_X86_32

    // 32 bits is not supported

#   endif

#endif

    return (!ext.isEmpty() && !arch.isEmpty());
}

} // namespace Digikam
