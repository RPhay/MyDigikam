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
      : downloadUrl (QLatin1String("https://files.kde.org/digikam/")),
        size        (0),
        success     (true),
        buttons     (nullptr),
        progress    (nullptr),
        nameLabel   (nullptr),
        reply       (nullptr),
        netMngr     (nullptr)
    {
    }

    const QString          downloadUrl;

    QString                url;
    QString                name;
    QString                hash;

    int                    size;
    bool                   success;

    QDialogButtonBox*      buttons;
    QProgressBar*          progress;
    QLabel*                nameLabel;

    QNetworkReply*         reply;
    QNetworkAccessManager* netMngr;
};

FilesDownloader::FilesDownloader(QWidget* const parent)
    : QDialog (parent),
      d       (new Private)
{
}

FilesDownloader::~FilesDownloader()
{
    if (d->reply)
    {
        d->reply->abort();
    }

    delete d;
}

bool FilesDownloader::checkDownloadFiles() const
{
    const QMap<QString, QList<QVariant> >& allFilesMap = getAllFilesMap();
    QMap<QString, QList<QVariant> >::const_iterator it;

    for (it = allFilesMap.constBegin() ; it != allFilesMap.constEnd() ; ++ it)
    {
        if (it.value().size() == 3)
        {
            if (exists(it.key(), it.value().at(2).toInt()))
            {
                continue;
            }

            return false;
        }
    }

    return true;
}

void FilesDownloader::startDownload()
{
    setWindowTitle(i18n("Download required files"));

    d->buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    d->buttons->button(QDialogButtonBox::Cancel)->setDefault(true);
    setMinimumWidth(600);
    setMinimumHeight(300);
    d->buttons->button(QDialogButtonBox::Ok)->setText(i18n("Download"));
    d->buttons->button(QDialogButtonBox::Ok)->setIcon(QIcon::fromTheme(QLatin1String("edit-download")));

    QWidget* const mainWidget = new QWidget(this);
    QVBoxLayout* const vBox   = new QVBoxLayout(mainWidget);

    // Some explanation.

    QString path              = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                       QString::fromLatin1("digikam"),
                                                       QStandardPaths::LocateDirectory);

    QLabel* const infoLabel   = new QLabel(i18n("<p>For the face engine and red eye removal tool, digiKam "
                                                "needs some large binary files. Some of these files were "
                                                "not found and are now being downloaded. You can cancel this "
                                                "process, the next time digiKam is started this dialog will "
                                                "appear again. Face recognition will not work without these "
                                                "files.</p>"
                                                "<p>The files will be downloaded to %1. Make sure there are "
                                                "around 300 MiB available. After the successful download "
                                                "you have to restart digiKam.</p>", path));
    infoLabel->setWordWrap(true);

    d->progress               = new QProgressBar();
    d->progress->setMinimum(0);

    d->nameLabel              = new QLabel();

    vBox->addWidget(infoLabel);
    vBox->addStretch(10);
    vBox->addWidget(d->nameLabel);
    vBox->addWidget(d->progress);
    vBox->addWidget(d->buttons);

    setLayout(vBox);

    // Setup the signals and slots.

    connect(d->buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(slotDownload()));

    connect(d->buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
            this, SLOT(reject()));

    exec();
}

void FilesDownloader::slotDownload()
{
    d->buttons->button(QDialogButtonBox::Ok)->setEnabled(false);

    if (d->success)
    {
        const QMap<QString, QList<QVariant> >& allFilesMap = getAllFilesMap();
        QMap<QString, QList<QVariant> >::const_iterator it;

        for (it = allFilesMap.constBegin() ; it != allFilesMap.constEnd() ; ++ it)
        {
            if (it.value().size() == 3)
            {
                if (exists(it.key(), it.value().at(2).toInt()))
                {
                    continue;
                }

                d->url  = it.value().at(0).toString();
                d->hash = it.value().at(1).toString();
                d->size = it.value().at(2).toInt();
                d->name = it.key();

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
                              i18n("An error occurred during the download.\n"
                                   "The download will continue at the next start."));

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

    d->progress->setMaximum(d->size);
    d->nameLabel->setText(d->name);
    d->progress->reset();

    QNetworkRequest request(QUrl(d->downloadUrl + d->url + d->name));
    request.setMaximumRedirectsAllowed(10);
    d->reply = d->netMngr->get(request);

    connect(d->reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(slotDownloadProgress(qint64,qint64)));
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
        QMessageBox::critical(this, qApp->applicationName(),
                              i18n("Network Error:\n\n%1", reply->errorString()));

        reply->deleteLater();

        d->success = false;

        return;
    }

    QByteArray data = reply->readAll();

    QCryptographicHash sha256(QCryptographicHash::Sha256);

    sha256.addData(data);

    if (d->hash != QString::fromLatin1(sha256.result().toHex()))
    {
        reply->deleteLater();

        d->success = false;

        slotDownload();

        return;
    }

    QString path = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    path        += QLatin1String("/facesengine");

    if (!QFileInfo::exists(path))
    {
        QDir().mkpath(path);
    }

    QFile file(path + QLatin1Char('/') + d->name);

    if (file.open(QIODevice::WriteOnly))
    {
        qint64 written = file.write(data);

        if (written != d->size)
        {
            d->success = false;
        }

        file.close();
    }

    reply->deleteLater();

    slotDownload();
}

void FilesDownloader::slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    d->progress->setMaximum(bytesTotal);
    d->progress->setValue(bytesReceived);
}

QMap<QString, QList<QVariant> > FilesDownloader::getAllFilesMap() const
{
    QMap<QString, QList<QVariant> > allFilesMap;
    QList<QVariant> fileList;

    fileList << QLatin1String("facesengine/shape-predictor/");
    fileList << QLatin1String("6f3d2a59dc30c7c9166983224dcf5732b25de734fff1e36ff1f3047ef90ed82b");
    fileList << 67740572;

    allFilesMap.insert(QLatin1String("shapepredictor.dat"), fileList);
    fileList.clear();

    if (qApp->applicationName() == QLatin1String("digikam"))
    {
        fileList << QLatin1String("facesengine/dnnface/");
        fileList << QLatin1String("9b72d54aeb24a64a8135dca8e792f7cc675c99a884a6940350a6cedcf7b7ba08");
        fileList << 31510785;

        allFilesMap.insert(QLatin1String("openface_nn4.small2.v1.t7"), fileList);
        fileList.clear();

        fileList << QLatin1String("facesengine/dnnface/");
        fileList << QLatin1String("f62621cac923d6f37bd669298c428bb7ee72233b5f8c3389bb893e35ebbcf795");
        fileList << 28092;

        allFilesMap.insert(QLatin1String("deploy.prototxt"), fileList);
        fileList.clear();

        fileList << QLatin1String("facesengine/dnnface/");
        fileList << QLatin1String("510ffd2471bd81e3fcc88a5beb4eae4fb445ccf8333ebc54e7302b83f4158a76");
        fileList << 5351047;

        allFilesMap.insert(QLatin1String("res10_300x300_ssd_iter_140000_fp16.caffemodel"), fileList);
        fileList.clear();

        fileList << QLatin1String("facesengine/dnnface/");
        fileList << QLatin1String("f6563bd6923fd6500d2c2d6025f32ebdba916a85e5c9798351d916909f62aaf5");
        fileList << 8334;

        allFilesMap.insert(QLatin1String("yolov3-face.cfg"), fileList);
        fileList.clear();

        fileList << QLatin1String("facesengine/dnnface/");
        fileList << QLatin1String("a88f3b3882e3cce1e553a81d42beef6202cb9afc3db88e7944f9ffbcc369e7df");
        fileList << 246305388;

        allFilesMap.insert(QLatin1String("yolov3-wider_16000.weights"), fileList);
        fileList.clear();
    }

    return allFilesMap;
}

bool FilesDownloader::exists(const QString& file, int size) const
{
    QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                          QString::fromLatin1("digikam/facesengine/%1").arg(file));

    return (!path.isEmpty() && (QFileInfo(path).size() == size));
}

} // namespace Digikam
