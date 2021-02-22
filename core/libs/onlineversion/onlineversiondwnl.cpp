/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-12-31
 * Description : Online version downloader.
 *
 * Copyright (C) 2020-2021 by Maik Qualmann <metzpinguin at gmail dot com>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <QSysInfo>
#include <QDialog>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>    // krazy:exclude=includes
#include <QByteArray>
#include <QPointer>
#include <QEventLoop>
#include <QApplication>
#include <QStandardPaths>
#include <QNetworkRequest>
#include <QCryptographicHash>
#include <QNetworkAccessManager>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "onlineversionchecker.h"

namespace Digikam
{

class Q_DECL_HIDDEN OnlineVersionDwnl::Private
{
public:

    explicit Private()
      : preRelease      (false),
        updateWithDebug (false),
        redirects       (0),
        reply           (nullptr),
        manager         (nullptr)
    {
    }

    bool                   preRelease;      ///< Flag to check pre-releases
    bool                   updateWithDebug; ///< Flag to use version with debug symbols
    int                    redirects;       ///< Count of redirected url

    QString                downloadUrl;     ///< Root url for current download
    QString                checksums;       ///< Current download sha256 sums
    QString                currentUrl;      ///< Full url of current file to download
    QString                error;           ///< Error string about current download
    QString                file;            ///< Info about file to download (version string, or filename)
    QString                downloaded;      ///< Local file path to downloaded data

    QNetworkReply*         reply;           ///< Current network request reply
    QNetworkAccessManager* manager;         ///< Network manager instance
};

OnlineVersionDwnl::OnlineVersionDwnl(QObject* const parent,
                                     bool checkPreRelease,
                                     bool updateWithDebug)
    : QObject(parent),
      d      (new Private)
{
    d->preRelease      = checkPreRelease;
    d->updateWithDebug = updateWithDebug;

    if (d->preRelease)
    {
        d->downloadUrl = QLatin1String("https://files.kde.org/digikam/");
    }
    else
    {
        d->downloadUrl = QLatin1String("https://download.kde.org/stable/digikam/");
    }

    d->manager         = new QNetworkAccessManager(this);
    d->manager->setRedirectPolicy(QNetworkRequest::ManualRedirectPolicy);

    connect(d->manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slotDownloaded(QNetworkReply*)));
}

OnlineVersionDwnl::~OnlineVersionDwnl()
{
    cancelDownload();
    delete d;
}

QString OnlineVersionDwnl::downloadUrl() const
{
    return d->downloadUrl;
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
    QUrl url;

    if (d->preRelease)
    {
        if (d->updateWithDebug)
        {
            QString base = version.section(QLatin1Char('.'), 0, -2);
            QString suf  = version.section(QLatin1Char('.'), -1);
            d->file      = base + QLatin1String("-debug.") + suf;
        }
        else
        {
            d->file = version;
        }

        d->currentUrl = d->downloadUrl + d->file + QLatin1String(".sha256");
        url           = QUrl(d->currentUrl);
    }
    else
    {
        QString arch;
        QString bundle;
        QString debug = d->updateWithDebug ? QLatin1String("-debug") : QString();

        if (!OnlineVersionChecker::bundleProperties(arch, bundle))
        {
            emit signalDownloadError(i18n("Unsupported Architecture: %1", QSysInfo::buildAbi()));

            qCDebug(DIGIKAM_GENERAL_LOG) << "Unsupported architecture";

            return;
        }

        QString os    =

#ifdef Q_OS_MACOS

                        QLatin1String("MacOS-");

#else

                        QString();

#endif

        d->file       = QString::fromLatin1("digikam-%1-%2%3%4.%5")
                            .arg(version)
                            .arg(os)
                            .arg(arch)
                            .arg(debug)
                            .arg(bundle);

        d->currentUrl = d->downloadUrl + QString::fromLatin1("%1/").arg(version) + d->file + QLatin1String(".sha256");
        url           = QUrl(d->currentUrl);
    }

    d->redirects = 0;
    download(url);
}

void OnlineVersionDwnl::download(const QUrl& url)
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "Downloading: " << url;

    d->redirects++;
    d->reply = d->manager->get(QNetworkRequest(url));

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

    // mark for deletion

    reply->deleteLater();
    d->reply = nullptr;

    if ((reply->error() != QNetworkReply::NoError)             &&
        (reply->error() != QNetworkReply::InsecureRedirectError))
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Error: " << reply->errorString();
        emit signalDownloadError(reply->errorString());

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

    // Check if checksum arrive in first

    if (reply->url().url().endsWith(QLatin1String(".sha256")))
    {
        QByteArray data = reply->readAll();

        if (data.isEmpty())
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "Checksum file is empty";
            emit signalDownloadError(i18n("Checksum file is empty."));

            return;
        }

        QTextStream in(&data);
        QString line    = in.readLine();  // first line and section 0 constains the checksum.
        QString sums    = line.section(QLatin1Char(' '), 0, 0);

        if (sums.isEmpty())
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "Checksum is invalid";
            emit signalDownloadError(i18n("Checksum is invalid."));

            return;
        }

        d->checksums = sums;
        qCDebug(DIGIKAM_GENERAL_LOG) << "Checksum is" << d->checksums;

        d->redirects = 0;
        download(QUrl(d->currentUrl.remove(QLatin1String(".sha256"))));

        return;
    }

    // Whole file to download is here

    QByteArray data = reply->readAll();

    if (data.isEmpty())
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Downloaded file is empty";
        emit signalDownloadError(i18n("Downloaded file is empty."));

        return;
    }

    // Compute checksum in a separated thread

    emit signalComputeChecksum();

    QString hash;
    QPointer<QEventLoop> loop = new QEventLoop(this);
    QFutureWatcher<void> fwatcher;

    connect(&fwatcher, SIGNAL(finished()),
            loop, SLOT(quit()));

    connect(static_cast<QDialog*>(parent()), SIGNAL(rejected()),
            &fwatcher, SLOT(cancel()));

    fwatcher.setFuture(QtConcurrent::run([&hash, &data]()
        {
            QCryptographicHash sha256(QCryptographicHash::Sha256);
            sha256.addData(data);
            hash = QString::fromLatin1(sha256.result().toHex());
        }
    ));

    loop->exec();

    if (d->checksums != hash)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Checksums error";
        emit signalDownloadError(i18n("Checksums error."));

        return;
    }

    // Checksum is fine, now save data to disk

    QString path = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    path         = QDir::toNativeSeparators(path + QLatin1String("/") + d->file);

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
