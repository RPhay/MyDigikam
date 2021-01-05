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

#include <QFile>
#include <QTextStream>
#include <QDesktopServices>

// Local includes

#include "digikam_version.h"
#include "digikam_debug.h"

namespace Digikam
{

OnlineVersionChecker::OnlineVersionChecker(QObject* const parent)
    : QObject   (parent),
      curRequest(nullptr)
{
    connect(&manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slotDownloadFinished(QNetworkReply*)));
}

OnlineVersionChecker::~OnlineVersionChecker()
{
}

void OnlineVersionChecker::checkForNewVersion()
{
    QNetworkRequest request(QUrl(QLatin1String("https://invent.kde.org/websites/digikam-org/-/raw/master/data/release.yml")));
    curRequest = manager.get(request);

    if (curRequest->error())
    {
        emit signalNewVersionCheckError(curRequest->error());
    }
}

void OnlineVersionChecker::cancelCheck()
{
    if (curRequest)
    {
        curRequest->abort();
    }
}

void OnlineVersionChecker::slotDownloadFinished(QNetworkReply* reply)
{
    // mark for deletion

    reply->deleteLater();
    curRequest = nullptr;

    QNetworkReply::NetworkError error = reply->error();

    if (error)
    {
        emit signalNewVersionCheckError(error);
        return;
    }

    QString data           = QString::fromUtf8(reply->readAll());

    if (data.isEmpty())
    {
        emit signalNewVersionCheckError(QNetworkReply::ProtocolFailure);
        return;
    }

    QString tag            = QLatin1String("version: ");
    int start              = data.indexOf(tag) + tag.size();
    QString rightVer       = data.mid(start);
    int end                = rightVer.indexOf(QLatin1Char('\n'));
    QString onlineVer      = rightVer.mid(0, end);
    QStringList onlineVals = onlineVer.split(QLatin1Char('.'));

    if (onlineVals.size() != 3)
    {
        emit signalNewVersionCheckError(QNetworkReply::ProtocolFailure);
        return;
    }

    QStringList currVals   = QString::fromLatin1(digikam_version_short).split(QLatin1Char('.'));

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
        emit signalNewVersionCheckError(QNetworkReply::NoError);
    }
}

} // namespace Digikam
