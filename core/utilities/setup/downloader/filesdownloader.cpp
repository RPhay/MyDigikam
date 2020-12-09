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

namespace Digikam
{

class Q_DECL_HIDDEN FilesDownloader::Private
{
public:

    explicit Private()
      : downloadUrl(QLatin1String("http://files.kde.org/digikam/")),
        index      (0),
        buttons    (nullptr),
        progress   (nullptr),
        nameLabel  (nullptr),
        reply      (nullptr),
        netMngr    (nullptr)
    {
    }

    const QString          downloadUrl;

    QString                error;

    QList<QVariant>        files;

    int                    index;

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
    d->files << QLatin1String("facesengine/shape-predictor/");
    d->files << QLatin1String("shapepredictor.dat");
    d->files << QLatin1String("6f3d2a59dc30c7c9166983224dcf5732b25de734fff1e36ff1f3047ef90ed82b");
    d->files << 67740572;

    if (qApp->applicationName() == QLatin1String("digikam"))
    {
        d->files << QLatin1String("facesengine/dnnface/");
        d->files << QLatin1String("openface_nn4.small2.v1.t7");
        d->files << QLatin1String("9b72d54aeb24a64a8135dca8e792f7cc675c99a884a6940350a6cedcf7b7ba08");
        d->files << 31510785;

        d->files << QLatin1String("facesengine/dnnface/");
        d->files << QLatin1String("deploy.prototxt");
        d->files << QLatin1String("f62621cac923d6f37bd669298c428bb7ee72233b5f8c3389bb893e35ebbcf795");
        d->files << 28092;

        d->files << QLatin1String("facesengine/dnnface/");
        d->files << QLatin1String("res10_300x300_ssd_iter_140000_fp16.caffemodel");
        d->files << QLatin1String("510ffd2471bd81e3fcc88a5beb4eae4fb445ccf8333ebc54e7302b83f4158a76");
        d->files << 5351047;

        d->files << QLatin1String("facesengine/dnnface/");
        d->files << QLatin1String("yolov3-face.cfg");
        d->files << QLatin1String("f6563bd6923fd6500d2c2d6025f32ebdba916a85e5c9798351d916909f62aaf5");
        d->files << 8334;

        d->files << QLatin1String("facesengine/dnnface/");
        d->files << QLatin1String("yolov3-wider_16000.weights");
        d->files << QLatin1String("a88f3b3882e3cce1e553a81d42beef6202cb9afc3db88e7944f9ffbcc369e7df");
        d->files << 246305388;
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
    for (int i = 0 ; i < d->files.size() ; i += 4)
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

    QLabel* const infoLabel   = new QLabel(i18n("<p>For the face engine and red eye removal tool, digiKam "
                                                "needs some large binary files. Some of these files were "
                                                "not found and are now being downloaded. You can cancel this "
                                                "process, the next time digiKam is started this dialog will "
                                                "appear again. Face detection will not work without these "
                                                "files.</p>"
                                                "<p>The files will be downloaded to %1. Make sure there are "
                                                "around 300 MiB available. After the successful download "
                                                "you have to restart digiKam.</p>", path), mainWidget);
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
        for ( ; d->index < d->files.size() ; d->index += 4)
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
                                   d->files.at(d->index + 1).toString(), d->error));

        close();
    }
}

void FilesDownloader::download()
{
    if (!d->netMngr)
    {
        d->netMngr = new QNetworkAccessManager(this);
        d->netMngr->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);

        connect(d->netMngr, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(slotDownloaded(QNetworkReply*)));
    }

    d->nameLabel->setText(d->files.at(d->index + 1).toString());
    d->progress->setMaximum(d->files.at(d->index + 3).toInt());
    d->progress->setValue(0);

    QNetworkRequest request(QUrl(d->downloadUrl                   +
                                 d->files.at(d->index).toString() +
                                 d->files.at(d->index + 1).toString()));

    request.setMaximumRedirectsAllowed(10);
    d->reply = d->netMngr->get(request);

    connect(d->reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(slotDownloadProgress(qint64,qint64)));

    connect(d->reply, SIGNAL(sslErrors(QList<QSslError>)),
            d->reply, SLOT(ignoreSslErrors()));
}

void FilesDownloader::nextDownload()
{
    QTimer::singleShot(100, this, SLOT(slotDownload()));
}

bool FilesDownloader::exists(int index) const
{
    QString file = d->files.at(index + 1).toString();
    QString path = QStandardPaths::locate(QStandardPaths::AppDataLocation,
                                          QString::fromLatin1("facesengine/%1").arg(file));

    return (!path.isEmpty() && (QFileInfo(path).size() == d->files.at(index + 3).toInt()));
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

    if (reply->error() != QNetworkReply::NoError)
    {
        d->error = reply->errorString();

        reply->deleteLater();

        nextDownload();

        return;
    }

    QByteArray data = reply->readAll();

    QCryptographicHash sha256(QCryptographicHash::Sha256);

    sha256.addData(data);

    if (d->files.at(d->index + 2).toString() != QString::fromLatin1(sha256.result().toHex()))
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

    QFile file(path + QLatin1Char('/') + d->files.at(d->index + 1).toString());

    if (file.open(QIODevice::WriteOnly))
    {
        qint64 written = file.write(data);

        if (written != d->files.at(d->index + 3).toInt())
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
    if (d->reply && (bytesReceived > d->files.at(d->index + 3).toInt()))
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
