/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-01-04
 * Description : Online version checker
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <QTextStream>
#include <QDateTime>
#include <QNetworkAccessManager>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_version.h"
#include "digikam_debug.h"
#include "dbuilddate.h"

namespace Digikam
{

class Q_DECL_HIDDEN OnlineVersionChecker::Private
{
public:

    explicit Private()
      : preRelease(false),
        redirects (0),
        manager   (nullptr),
        curRequest(nullptr),
        curVersion(QLatin1String(digikam_version_short))
    {
    }

    bool                   preRelease;
    int                    redirects;

    QNetworkAccessManager* manager;
    QNetworkReply*         curRequest;
    QString                curVersion;
    QString                preReleaseFileName;
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
    if (d->curRequest)
    {
        d->curRequest->abort();
    }
}

void OnlineVersionChecker::setCurrentVersion(const QString& version)
{
    d->curVersion = version;
}

QString OnlineVersionChecker::preReleaseFileName() const
{
    return d->preReleaseFileName;
}

void OnlineVersionChecker::checkForNewVersion()
{
    QUrl rUrl;

    if (d->preRelease)
    {
        rUrl = QUrl(QLatin1String("https://files.kde.org/digikam/FILES"));
    }
    else
    {
        rUrl = QUrl(QLatin1String("https://invent.kde.org/websites/digikam-org/-/raw/master/data/release.yml"));
    }

    d->redirects = 0;
    download(rUrl);
}

void OnlineVersionChecker::download(const QUrl& url)
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "Downloading: " << url;

    d->redirects++;
    d->curRequest = d->manager->get(QNetworkRequest(url));

    connect(d->curRequest, SIGNAL(sslErrors(QList<QSslError>)),
            d->curRequest, SLOT(ignoreSslErrors()));

    if (d->curRequest->error())
    {
        emit signalNewVersionCheckError(d->curRequest->errorString());
    }
}

void OnlineVersionChecker::slotDownloadFinished(QNetworkReply* reply)
{
    if (reply != d->curRequest)
    {
        return;
    }

    // mark for deletion

    reply->deleteLater();
    d->curRequest = nullptr;

    if ((reply->error() != QNetworkReply::NoError)             &&
        (reply->error() != QNetworkReply::InsecureRedirectError))
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Error: " << reply->errorString();
        emit signalNewVersionCheckError(reply->errorString());

        return;
    }

    QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

    if (redirectUrl.isValid() && (reply->url() != redirectUrl) && (d->redirects < 10))
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

    if (d->preRelease)
    {
        // NOTE: pre-release files from files.kde.org is a simple list of remote directory contents where we will extract build date string.

        QString arch;
        QString ext;
        bundleProperties(arch, ext);

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
            qCWarning(DIGIKAM_GENERAL_LOG) << "Invalid format returned from the remote connection.";
            emit signalNewVersionCheckError(i18n("Invalid format returned from the remote connection."));

            return;
        }

        QDateTime onlineDt   = QDateTime::fromString(sections[3], QLatin1String("yyyyMMddTHHmmss"));
        qCDebug(DIGIKAM_GENERAL_LOG) << "Pre-release build date:" << onlineDt << "(" << sections[3] << ")";

        if (!onlineDt.isValid())
        {
            qCWarning(DIGIKAM_GENERAL_LOG) << "Invalid pre-release date.";
            emit signalNewVersionCheckError(i18n("Invalid pre-release date."));

            return;
        }

        qCDebug(DIGIKAM_GENERAL_LOG) << "Pre-release file Name :" << preReleaseFileName();
        qCDebug(DIGIKAM_GENERAL_LOG) << "Pre-release build date:" << onlineDt;

        if (onlineDt > digiKamBuildDate())
        {
            emit signalNewVersionAvailable(sections[3]);
        }
        else
        {
            emit signalNewVersionCheckError(QString());
        }
    }
    else
    {
        // NOTE: stable files from digikam.org is a Yaml config file where we will extract version string.

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

#ifdef Q_OS_MACOS

    ext  = QLatin1String("pkg");

#   ifdef Q_PROCESSOR_X86_64

    arch = QLatin1String("x86-64");

#   else

    arch = QLatin1String("arm-64");

#   endif

#endif

#ifdef Q_OS_WINDOWS

    ext  = QLatin1String("exe");

#   ifdef Q_PROCESSOR_X86_64

    arch = QLatin1String("x86-64");

#   elif defined Q_PROCESSOR_X86_32

    arch = QLatin1String("i386");

#   endif

#endif

#ifdef Q_OS_LINUX

    ext  = QLatin1String("appimage");

#   ifdef Q_PROCESSOR_X86_64

    arch = QLatin1String("x86-64");

#   elif defined Q_PROCESSOR_X86_32

    arch = QLatin1String("i386");

#   endif

#endif

    return (!ext.isEmpty() && !arch.isEmpty());
}

} // namespace Digikam
