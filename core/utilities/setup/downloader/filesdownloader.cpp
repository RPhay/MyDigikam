/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-11-14
 * Description : Files downloader
 *
 * Copyright (C) 2020 by Maik Qualmann <metzpinguin at gmail dot com>
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
        index      (0),
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

    QList<DownloadInfo*>   files;

    int                    index;
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
    d->files << new DownloadInfo();
    d->files.last()->path = QLatin1String("facesengine/shape-predictor/");
    d->files.last()->name = QLatin1String("shapepredictor.dat");
    d->files.last()->hash = QLatin1String("6f3d2a59dc30c7c9166983224dcf5732b25de734fff1e36ff1f3047ef90ed82b");
    d->files.last()->size = 67740572;

    if (qApp->applicationName() == QLatin1String("digikam"))
    {
        d->files << new DownloadInfo();
        d->files.last()->path = QLatin1String("facesengine/dnnface/");
        d->files.last()->name = QLatin1String("openface_nn4.small2.v1.t7");
        d->files.last()->hash = QLatin1String("9b72d54aeb24a64a8135dca8e792f7cc675c99a884a6940350a6cedcf7b7ba08");
        d->files.last()->size = 31510785;

        d->files << new DownloadInfo();
        d->files.last()->path = QLatin1String("facesengine/dnnface/");
        d->files.last()->name = QLatin1String("deploy.prototxt");
        d->files.last()->hash = QLatin1String("f62621cac923d6f37bd669298c428bb7ee72233b5f8c3389bb893e35ebbcf795");
        d->files.last()->size = 28092;

        d->files << new DownloadInfo();
        d->files.last()->path = QLatin1String("facesengine/dnnface/");
        d->files.last()->name = QLatin1String("res10_300x300_ssd_iter_140000_fp16.caffemodel");
        d->files.last()->hash = QLatin1String("510ffd2471bd81e3fcc88a5beb4eae4fb445ccf8333ebc54e7302b83f4158a76");
        d->files.last()->size = 5351047;

        d->files << new DownloadInfo();
        d->files.last()->path = QLatin1String("facesengine/dnnface/");
        d->files.last()->name = QLatin1String("yolov3-face.cfg");
        d->files.last()->hash = QLatin1String("f6563bd6923fd6500d2c2d6025f32ebdba916a85e5c9798351d916909f62aaf5");
        d->files.last()->size = 8334;

        d->files << new DownloadInfo();
        d->files.last()->path = QLatin1String("facesengine/dnnface/");
        d->files.last()->name = QLatin1String("yolov3-wider_16000.weights");
        d->files.last()->hash = QLatin1String("a88f3b3882e3cce1e553a81d42beef6202cb9afc3db88e7944f9ffbcc369e7df");
        d->files.last()->size = 246305388;
    }
}

FilesDownloader::~FilesDownloader()
{
    if (d->reply)
    {
        d->reply->abort();
        d->reply = nullptr;
    }

    qDeleteAll(d->files);
    d->files.clear();

    delete d;
}

bool FilesDownloader::checkDownloadFiles() const
{
    for (int i = 0 ; i < d->files.size() ; ++i)
    {
        if (!exists(i))
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

    for (int i = 0 ; i < d->files.size() ; ++i)
    {
        size += d->files.at(i)->size;
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
        for ( ; d->index < d->files.size() ; d->index++)
        {
            if (!exists(d->index))
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
        QMessageBox::critical(this, qApp->applicationName(),
                              i18n("An error occurred during the download.\n\n"
                                   "File: %1\n\n%2\n\n"
                                   "The download will continue at the next start.",
                                   d->files.at(d->index)->name, d->error));

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

    QUrl request(d->downloadUrl              +
                 d->files.at(d->index)->path +
                 d->files.at(d->index)->name);

    d->redirects = 0;
    createRequest(request);
}

void FilesDownloader::nextDownload()
{
    QTimer::singleShot(100, this, SLOT(slotDownload()));
}

bool FilesDownloader::exists(int index) const
{
    QString file = d->files.at(index)->name;
    QString path = QStandardPaths::locate(QStandardPaths::AppDataLocation,
                                          QString::fromLatin1("facesengine/%1").arg(file));

    return (!path.isEmpty() && (QFileInfo(path).size() == d->files.at(index)->size));
}

void FilesDownloader::createRequest(const QUrl& url)
{
    d->nameLabel->setText(d->files.at(d->index)->name);
    d->progress->setMaximum(d->files.at(d->index)->size);
    d->progress->setValue(0);

    d->redirects++;
    d->reply = d->netMngr->get(QNetworkRequest(url));

    connect(d->reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(slotDownloadProgress(qint64,qint64)));

    connect(d->reply, SIGNAL(sslErrors(QList<QSslError>)),
            d->reply, SLOT(ignoreSslErrors()));
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

    if (d->files.at(d->index)->hash != QString::fromLatin1(sha256.result().toHex()))
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

    QFile file(path + QLatin1Char('/') + d->files.at(d->index)->name);

    if (file.open(QIODevice::WriteOnly))
    {
        qint64 written = file.write(data);

        if (written != d->files.at(d->index)->size)
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
    if (d->reply && (bytesReceived > d->files.at(d->index)->size))
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

} // namespace Digikam
