/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-01-05
 * Description : Online version downloader.
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

#include "onlineversiondwnl.h"

// Qt includes

#include <QDir>
#include <QByteArray>
#include <QApplication>
#include <QStandardPaths>
#include <QNetworkRequest>
#include <QCryptographicHash>
#include <QNetworkAccessManager>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN OnlineVersionDwnl::Private
{
public:

    explicit Private()
      : downloadUrl(QLatin1String("https://download.kde.org/stable/digikam/")),
        reply      (nullptr),
        netMngr    (nullptr)
    {
    }

    const QString          downloadUrl;

    QString                error;
    QString                file;

    QNetworkReply*         reply;
    QNetworkAccessManager* netMngr;
};

OnlineVersionDwnl::OnlineVersionDwnl(QObject* const parent)
    : QObject(parent),
      d      (new Private)
{
    d->netMngr = new QNetworkAccessManager(this);
    d->netMngr->setRedirectPolicy(QNetworkRequest::ManualRedirectPolicy);

    connect(d->netMngr, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slotDownloaded(QNetworkReply*)));
}

OnlineVersionDwnl::~OnlineVersionDwnl()
{
    if (d->reply)
    {
        d->reply->abort();
        d->reply = nullptr;
    }

    delete d;
}

void OnlineVersionDwnl::startDownload(const QString& file)
{
    d->file  = file;
    QUrl url = QUrl(d->downloadUrl + d->file);
    d->reply = d->netMngr->get(QNetworkRequest(url));

    connect(d->reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SIGNAL(signalDownloadProgress(qint64,qint64)));

    connect(d->reply, SIGNAL(sslErrors(QList<QSslError>)),
            d->reply, SLOT(ignoreSslErrors()));
}

void OnlineVersionDwnl::slotDownloaded(QNetworkReply* reply)
{
    if (reply != d->reply)
    {
        return;
    }

    d->reply = nullptr;

    if ((reply->error() != QNetworkReply::NoError)             &&
        (reply->error() != QNetworkReply::InsecureRedirectError))
    {
        emit signalDownloadError(reply->errorString());

        reply->deleteLater();

        return;
    }

    QByteArray data = reply->readAll();
    QString path    = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    path            = QDir::toNativeSeparators(path + QLatin1String("/") + d->file);

    QFile file(path);

    if (file.open(QIODevice::WriteOnly))
    {
        file.write(data);
        file.close();

        emit signalDownloadError(QString());  // No error: download is complete.
    }
    else
    {
        emit signalDownloadError(i18n("Cannot open target file."));
    }
}

} // namespace Digikam
