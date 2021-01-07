/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-01-05
 * Description : Online version downloader.
 *
 * Copyright (C) 2020 by Maik Qualmann <metzpinguin at gmail dot com>
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
        redirects  (0),
        reply      (nullptr),
        netMngr    (nullptr)
    {
    }

    const QString          downloadUrl;

    int                    redirects;

    QString                error;
    QString                file;
    QString                downloaded;

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
    cancelDownload();
    delete d;
}

void OnlineVersionDwnl::cancelDownload()
{
    if (d->reply)
    {
        d->reply->abort();
        d->reply = nullptr;
    }
}

void OnlineVersionDwnl::startDownload(const QString& version)
{
    QString arch;
    QString bundle;

#ifdef Q_OS_MACOS

    bundle = QLatin1String("pkg");

#   ifdef Q_PROCESSOR_X86_64

    arch   = QLatin1String("x86-64");

#   else

    arch   = QLatin1String("arm-64");

#   endif

#endif

#ifdef Q_OS_WINDOWS

    bundle = QLatin1String("exe");

#   ifdef Q_PROCESSOR_X86_64

    arch   = QLatin1String("x86-64");

#   elif defined Q_PROCESSOR_X86_32

    arch   = QLatin1String("i386");

#   endif

#endif

#ifdef Q_OS_LINUX

    bundle = QLatin1String("appimage");

#   ifdef Q_PROCESSOR_X86_64

    arch   = QLatin1String("x86-64");

#   elif defined Q_PROCESSOR_X86_32

    arch   = QLatin1String("i386");

#   endif

#endif

    if (arch.isEmpty() || bundle.isEmpty())
    {
        emit signalDownloadError(i18n("Unsupported Architecture."));

        qCDebug(DIGIKAM_GENERAL_LOG) << "Unsupported architecture";

        return;
    }

    d->file      = QString::fromLatin1("digikam-%1-%2.%3")
                      .arg(version)
                      .arg(arch)
                      .arg(bundle);

    d->redirects = 0;
    QUrl url     = QUrl(d->downloadUrl + QString::fromLatin1("%1/").arg(version) + d->file);
    download(url);
}

void OnlineVersionDwnl::download(const QUrl& url)
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "Downloading: " << url;

    d->redirects++;
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
        qCDebug(DIGIKAM_GENERAL_LOG) << "Error: " << reply->errorString();
        emit signalDownloadError(reply->errorString());
        reply->deleteLater();

        return;
    }

    QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

    if (redirectUrl.isValid() && (reply->url() != redirectUrl) && (d->redirects < 10))
    {
        download(redirectUrl);

        reply->deleteLater();

        return;
    }

    QByteArray data = reply->readAll();

    if (data.isEmpty())
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Downloaded file is empty";
        emit signalDownloadError(i18n("Downloaded file is empty."));

        return;
    }

    QString path    = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    path            = QDir::toNativeSeparators(path + QLatin1String("/") + d->file);

    QFile file(path);

    if (file.open(QIODevice::WriteOnly))
    {
        file.write(data);
        file.close();

        QFile::setPermissions(path, QFile::permissions(path) | QFileDevice::ExeUser);
        d->downloaded = path;

        qCDebug(DIGIKAM_GENERAL_LOG) << "Download is complete: " << path;

        emit signalDownloadError(QString());  // No error: download is complete.
    }
    else
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Cannot open " << path;
        emit signalDownloadError(i18n("Cannot open target file."));
    }
}

QString OnlineVersionDwnl::downloadedPath() const
{
    return d->downloaded;
}

} // namespace Digikam
