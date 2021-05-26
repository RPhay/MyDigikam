/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-11-14
 * Description : Files downloader
 *
 * Copyright (C) 2020-2021 by Maik Qualmann <metzpinguin at gmail dot com>
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

#include "filesdownloader.h"

// Qt includes

#include <QDir>
#include <QLabel>
#include <QTimer>
#include <QPointer>
#include <QByteArray>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QApplication>
#include <QStandardPaths>
#include <QNetworkRequest>
#include <QDialogButtonBox>
#include <QCryptographicHash>
#include <QNetworkAccessManager>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "itempropertiestab.h"

namespace Digikam
{

class Q_DECL_HIDDEN FilesDownloader::Private
{
public:

    explicit Private()
      : downloadUrl(QLatin1String("https://files.kde.org/digikam/")),
        redirects  (0),
        buttons    (nullptr),
        progress   (nullptr),
        nameLabel  (nullptr),
        reply      (nullptr),
        netMngr    (nullptr)
    {
    }

    const QString          downloadUrl;

    QString                error;

    QList<DownloadInfo>    files;
    DownloadInfo           currentInfo;

    int                    redirects;

    QDialogButtonBox*      buttons;
    QProgressBar*          progress;
    QLabel*                nameLabel;

    QNetworkReply*         reply;
    QNetworkAccessManager* netMngr;
};

FilesDownloader::FilesDownloader(QWidget* const parent)
    : QDialog(parent),
      d      (new Private)
{
    d->files << DownloadInfo(QLatin1String("facesengine/shape-predictor/"),
                             QLatin1String("shapepredictor.dat"),
                             QLatin1String("6f3d2a59dc30c7c9166983224dcf5732b25de734fff1e36ff1f3047ef90ed82b"),
                             67740572
                            );

    if (qApp->applicationName() == QLatin1String("digikam"))
    {
        d->files << DownloadInfo(QLatin1String("facesengine/dnnface/"),
                                 QLatin1String("openface_nn4.small2.v1.t7"),
                                 QLatin1String("9b72d54aeb24a64a8135dca8e792f7cc675c99a884a6940350a6cedcf7b7ba08"),
                                 31510785
                                );

        d->files << DownloadInfo(QLatin1String("facesengine/dnnface/"),
                                 QLatin1String("deploy.prototxt"),
                                 QLatin1String("f62621cac923d6f37bd669298c428bb7ee72233b5f8c3389bb893e35ebbcf795"),
                                 28092
                                );

        d->files << DownloadInfo(QLatin1String("facesengine/dnnface/"),
                                 QLatin1String("res10_300x300_ssd_iter_140000_fp16.caffemodel"),
                                 QLatin1String("510ffd2471bd81e3fcc88a5beb4eae4fb445ccf8333ebc54e7302b83f4158a76"),
                                 5351047
                                );

        d->files << DownloadInfo(QLatin1String("facesengine/dnnface/"),
                                 QLatin1String("yolov3-face.cfg"),
                                 QLatin1String("f6563bd6923fd6500d2c2d6025f32ebdba916a85e5c9798351d916909f62aaf5"),
                                 8334
                                );

        d->files << DownloadInfo(QLatin1String("facesengine/dnnface/"),
                                 QLatin1String("yolov3-wider_16000.weights"),
                                 QLatin1String("a88f3b3882e3cce1e553a81d42beef6202cb9afc3db88e7944f9ffbcc369e7df"),
                                 246305388
                                );
    }
}

FilesDownloader::~FilesDownloader()
{
    if (d->reply)
    {
        d->reply->abort();
        d->reply = nullptr;
    }

    delete d;
}

bool FilesDownloader::checkDownloadFiles() const
{
    foreach (const DownloadInfo& info, d->files)
    {
        if (!downloadExists(info))
        {
            return false;
        }
    }

    return true;
}

void FilesDownloader::startDownload()
{
    setWindowTitle(i18n("Download required files"));
    setMinimumHeight(250);
    setMinimumWidth(600);

    QWidget* const mainWidget = new QWidget(this);
    QVBoxLayout* const vBox   = new QVBoxLayout(mainWidget);

    d->buttons                = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                     QDialogButtonBox::Cancel, mainWidget);
    d->buttons->button(QDialogButtonBox::Ok)->setDefault(true);
    d->buttons->button(QDialogButtonBox::Ok)->setText(i18n("Download"));
    d->buttons->button(QDialogButtonBox::Ok)->setIcon(QIcon::fromTheme(QLatin1String("edit-download")));

    QString path              = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    path                      = QDir::toNativeSeparators(path + QLatin1String("/facesengine"));
    qint64 size               = 0;

    foreach (const DownloadInfo& info, d->files)
    {
        if (!downloadExists(info))
        {
            size += info.size;
        }
    }

    QString total             = ItemPropertiesTab::humanReadableBytesCount(size);

    QLabel* const infoLabel   = new QLabel(i18nc("%1: folder path, %2: disk size with unit",
                                                 "<p>For the face engine and red eye removal tool, digiKam "
                                                 "needs some large binary files. Some of these files were "
                                                 "not found and are now being downloaded. You can cancel this "
                                                 "process, the next time digiKam is started this dialog will "
                                                 "appear again. Face detection will not work without these "
                                                 "files.</p>"
                                                 "<p>The files will be downloaded to %1. Make sure there are "
                                                 "around %2 available. <b>After the successful download you "
                                                 "have to restart digiKam.</b></p>", path, total), mainWidget);
    infoLabel->setWordWrap(true);

    d->progress               = new QProgressBar(mainWidget);
    d->progress->setMinimum(0);
    d->progress->setMaximum(1);
    d->progress->setValue(0);

    d->nameLabel              = new QLabel(mainWidget);

    vBox->addWidget(infoLabel);
    vBox->addStretch(10);
    vBox->addWidget(d->nameLabel);
    vBox->addWidget(d->progress);
    vBox->addWidget(d->buttons);

    setLayout(vBox);

    connect(d->buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(slotDownload()));

    connect(d->buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
            this, SLOT(reject()));

    exec();
}

void FilesDownloader::slotDownload()
{
    d->buttons->button(QDialogButtonBox::Ok)->setEnabled(false);

    if (d->error.isEmpty())
    {
        while (!d->files.isEmpty())
        {
            d->currentInfo = d->files.takeFirst();

            if (!downloadExists(d->currentInfo))
            {
                download();

                return;
            }
        }

        QMessageBox::information(this, qApp->applicationName(),
                                 i18n("All files were downloaded successfully."));

        close();
    }
    else
    {
        QPointer<QMessageBox> msgBox = new QMessageBox(QMessageBox::Critical,
                 i18n("Download error"),
                 i18n("An error occurred during the download.\n\n"
                      "File: %1\n\n%2\n\n"
                      "You can try again or continue the "
                      "download at the next start.",
                      d->currentInfo.name, d->error),
                 QMessageBox::Yes | QMessageBox::Cancel,
                 qApp->activeWindow());

        msgBox->button(QMessageBox::Yes)->setText(i18n("Try again"));
        msgBox->button(QMessageBox::Yes)->setIcon(QIcon::fromTheme(QLatin1String("edit-download")));

        int result = msgBox->exec();
        delete msgBox;

        if (result == QMessageBox::Yes)
        {
            d->error.clear();
            download();

            return;
        }

        close();
    }
}

void FilesDownloader::download()
{
    if (!d->netMngr)
    {
        d->netMngr = new QNetworkAccessManager(this);
        d->netMngr->setRedirectPolicy(QNetworkRequest::ManualRedirectPolicy);

        connect(d->netMngr, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(slotDownloaded(QNetworkReply*)));
    }

    QUrl request(d->downloadUrl      +
                 d->currentInfo.path +
                 d->currentInfo.name);

    d->redirects = 0;
    createRequest(request);
}

void FilesDownloader::nextDownload()
{
    QTimer::singleShot(100, this, SLOT(slotDownload()));
}

void FilesDownloader::createRequest(const QUrl& url)
{
    d->progress->setMaximum(d->currentInfo.size);
    d->progress->setValue(0);
    printDownloadInfo(url);

    d->redirects++;
    d->reply = d->netMngr->get(QNetworkRequest(url));

    connect(d->reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(slotDownloadProgress(qint64,qint64)));

    connect(d->reply, SIGNAL(sslErrors(QList<QSslError>)),
            d->reply, SLOT(ignoreSslErrors()));
}

void FilesDownloader::printDownloadInfo(const QUrl& url)
{
    QString text = QString::fromUtf8("%1 (%2://%3)")
                   .arg(d->currentInfo.name)
                   .arg(url.scheme())
                   .arg(url.host());

    d->nameLabel->setText(text);
}

bool FilesDownloader::downloadExists(const DownloadInfo& info) const
{
    QString path = QStandardPaths::locate(QStandardPaths::AppDataLocation,
                                          QString::fromLatin1("facesengine/%1").arg(info.name));

    return (!path.isEmpty() && (QFileInfo(path).size() == info.size));
}

void FilesDownloader::reject()
{
    if (d->reply)
    {
        d->reply->abort();
        d->reply = nullptr;
    }

    QDialog::reject();
}

void FilesDownloader::slotDownloaded(QNetworkReply* reply)
{
    if (reply != d->reply)
    {
        return;
    }

    d->reply = nullptr;

    if ((reply->error() != QNetworkReply::NoError)             &&
        (reply->error() != QNetworkReply::InsecureRedirectError))
    {
        d->error = reply->errorString();

        reply->deleteLater();

        nextDownload();

        return;
    }

    QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

    if (redirectUrl.isValid() && (reply->url() != redirectUrl) && (d->redirects < 10))
    {
        createRequest(redirectUrl);

        reply->deleteLater();

        return;
    }

    QCryptographicHash sha256(QCryptographicHash::Sha256);

    QByteArray data = reply->readAll();

    sha256.addData(data);

    if (d->currentInfo.hash != QString::fromLatin1(sha256.result().toHex()))
    {
        d->error = i18n("Checksum is incorrect.");

        reply->deleteLater();

        nextDownload();

        return;
    }

    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    path        += QLatin1String("/facesengine");

    if (!QFileInfo::exists(path))
    {
        QDir().mkpath(path);
    }

    QFile file(path + QLatin1Char('/') + d->currentInfo.name);

    if (file.open(QIODevice::WriteOnly))
    {
        qint64 written = file.write(data);

        if (written != d->currentInfo.size)
        {
            d->error = i18n("File write error.");
        }

        file.close();
    }
    else
    {
        d->error = i18n("File open error.");
    }

    reply->deleteLater();

    nextDownload();
}

void FilesDownloader::slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (d->reply && (bytesReceived > d->currentInfo.size))
    {
        d->reply->abort();
        d->reply = nullptr;

        d->error = i18n("File on the server is too large.");

        nextDownload();

        return;
    }

    d->progress->setMaximum(bytesTotal);
    d->progress->setValue(bytesReceived);
}

//-----------------------------------------------------------------------------

DownloadInfo::DownloadInfo()
    : size(0)
{
}

DownloadInfo::DownloadInfo(const QString& _path,
                           const QString& _name,
                           const QString& _hash,
                           const qint64&  _size)
    : path(_path),
      name(_name),
      hash(_hash),
      size(_size)
{
}

DownloadInfo::DownloadInfo(const DownloadInfo& other)
    : path(other.path),
      name(other.name),
      hash(other.hash),
      size(other.size)
{
}

DownloadInfo::~DownloadInfo()
{
}

DownloadInfo& DownloadInfo::operator=(const DownloadInfo& other)
{
    path = other.path;
    name = other.name;
    hash = other.hash;
    size = other.size;

    return *this;
}

} // namespace Digikam
