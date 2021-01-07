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

#include <QDesktopServices>
#include <QNetworkAccessManager>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_version.h"
#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN OnlineVersionChecker::Private
{
public:

    explicit Private()
      : manager   (nullptr),
        curRequest(nullptr),
        curVersion(QLatin1String(digikam_version_short))
    {
    }

    QNetworkAccessManager* manager;
    QNetworkReply*         curRequest;
    QString                curVersion;
};

OnlineVersionChecker::OnlineVersionChecker(QObject* const parent)
    : QObject(parent),
      d      (new Private)
{
    d->manager = new QNetworkAccessManager(this);

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

void OnlineVersionChecker::checkForNewVersion()
{
    QNetworkRequest request(QUrl(QLatin1String("https://invent.kde.org/websites/digikam-org/-/raw/master/data/release.yml")));
    d->curRequest = d->manager->get(request);

    if (d->curRequest->error())
    {
        emit signalNewVersionCheckError(d->curRequest->errorString());
    }
}

void OnlineVersionChecker::slotDownloadFinished(QNetworkReply* reply)
{
    // mark for deletion

    reply->deleteLater();
    d->curRequest = nullptr;

    QNetworkReply::NetworkError error = reply->error();

    if (error != QNetworkReply::NoError)
    {
        emit signalNewVersionCheckError(reply->errorString());
        return;
    }

    QString data           = QString::fromUtf8(reply->readAll());

    if (data.isEmpty())
    {
        emit signalNewVersionCheckError(i18n("No data returned from the remote connection."));
        return;
    }

    // NOTE: File from digikam.org is a Yaml config file where we will extract version string.

    QString tag            = QLatin1String("version: ");
    int start              = data.indexOf(tag) + tag.size();
    QString rightVer       = data.mid(start);
    int end                = rightVer.indexOf(QLatin1Char('\n'));
    QString onlineVer      = rightVer.mid(0, end);
    QStringList onlineVals = onlineVer.split(QLatin1Char('.'));

    if (onlineVals.size() != 3)
    {
        emit signalNewVersionCheckError(i18n("Invalid format returned from the remote connection."));
        return;
    }

    QStringList currVals   = d->curVersion.split(QLatin1Char('.'));

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

} // namespace Digikam
