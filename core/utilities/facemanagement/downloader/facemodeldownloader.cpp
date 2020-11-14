/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-11-14
 * Description : Face model downloader
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

#include "facemodeldownloader.h"

// Qt includes

#include <QDir>
#include <QByteArray>
#include <QMessageBox>
#include <QApplication>
#include <QStandardPaths>
#include <QNetworkRequest>
#include <QCryptographicHash>
#include <QNetworkAccessManager>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "opencvdnnfacedetector.h"

namespace Digikam
{

class Q_DECL_HIDDEN FaceModelDownloader::Private
{
public:

    explicit Private()
      : model       (DetectorNNModel::SSDMOBILENET),
        downloadUrl (QLatin1String("http://")),
        caffeModel  (QLatin1String("deploy.prototxt")),
        caffeData   (QLatin1String("res10_300x300_ssd_iter_140000_fp16.caffemodel")),
        caffeHash   (QLatin1String("a8e705245deb6f21a70f8690d44616d654ae300796a5532180a65dfede473232")),
        yoloModel   (QLatin1String("yolov3-face.cfg")),
        yoloData    (QLatin1String("yolov3-wider_16000.weights")),
        yoloHash    (QLatin1String("b60b588311ec780fb73dd3b41c6fa2617e18830dded1cc568e3020513888af73")),
        reply       (nullptr),
        netMngr     (nullptr)
    {
    }

    int                    model;

    const QString          downloadUrl;
    const QString          caffeModel;
    const QString          caffeData;
    const QString          caffeHash;

    const QString          yoloModel;
    const QString          yoloData;
    const QString          yoloHash;

    QByteArray             modelArray;
    QByteArray             dataArray;

    QNetworkReply*         reply;
    QNetworkAccessManager* netMngr;
};

FaceModelDownloader::FaceModelDownloader(QObject* const parent)
    : QObject (parent),
      d       (new Private)
{
    d->netMngr = new QNetworkAccessManager(this);

    connect(d->netMngr, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(downloaded(QNetworkReply*)));
}

FaceModelDownloader::~FaceModelDownloader()
{
    if (d->reply)
    {
        d->reply->abort();
    }

    delete d;
}

bool FaceModelDownloader::exists() const
{
    QString model;
    QString data;

    if (d->model == DetectorNNModel::SSDMOBILENET)
    {
        model = d->caffeModel;
        data  = d->caffeData;
    }
    else
    {
        model = d->yoloModel;
        data  = d->yoloData;
    }

    QString nnmodel = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                             QString::fromLatin1("digikam/facesengine/%1").arg(model));
    QString nndata  = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                             QString::fromLatin1("digikam/facesengine/%1").arg(data));

    return (!nnmodel.isEmpty() && !nndata.isEmpty());
}

void FaceModelDownloader::download(int model)
{
    d->modelArray.clear();
    d->dataArray.clear();

    d->model = model;
/*
    if (exists())
    {
        emit downloadFinished(true, d->model);

        return;
    }
*/
    nextDownload();
}

void FaceModelDownloader::nextDownload()
{
    QString model;
    QString data;
    QString hash;

    if (d->model == DetectorNNModel::SSDMOBILENET)
    {
        model = d->caffeModel;
        data  = d->caffeData;
        hash  = d->caffeHash;
    }
    else
    {
        model = d->yoloModel;
        data  = d->yoloData;
        hash  = d->yoloHash;
    }

    if      (d->modelArray.isEmpty())
    {
        QNetworkRequest request(QUrl(d->downloadUrl + model));
        d->reply = d->netMngr->get(request);
    }
    else if (d->dataArray.isEmpty())
    {
        QNetworkRequest request(QUrl(d->downloadUrl + data));
        d->reply = d->netMngr->get(request);
    }
    else
    {
        QCryptographicHash sha256(QCryptographicHash::Sha256);

        sha256.addData(d->modelArray);
        sha256.addData(d->dataArray);

        if (hash == QString::fromLatin1(sha256.result().toHex()))
        {
            bool ret = true;

            ret &= saveArray(d->modelArray, model);
            ret &= saveArray(d->dataArray, data);

            if (!ret)
            {
                QMessageBox::critical(qApp->activeWindow(), qApp->applicationName(),
                                      i18n("Save Error"));
            }

            emit downloadFinished(ret, d->model);
        }
        else
        {
            QMessageBox::critical(qApp->activeWindow(), qApp->applicationName(),
                                  i18n("Hash Error"));

            emit downloadFinished(false, d->model);
        }

        qDebug() << d->modelArray.size() << d->dataArray.size() << QString::fromLatin1(sha256.result().toHex());
    }
}

bool FaceModelDownloader::saveArray(const QByteArray& array, const QString& name)
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    path        += QLatin1String("/facesenigne");

    if (!QFileInfo::exists(path))
    {
        QDir().mkpath(path);
    }

    QFile file(path + QLatin1Char('/') + name);

    if (!file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    qint64 written = file.write(array);

    file.close();

    return (written == array.size());
}

void FaceModelDownloader::downloaded(QNetworkReply* reply)
{
    if (reply != d->reply)
    {
        return;
    }

    d->reply = nullptr;

    if (reply->error() != QNetworkReply::NoError)
    {
        QMessageBox::critical(qApp->activeWindow(), qApp->applicationName(),
                              i18n("Network Error:\n\n%1", reply->errorString()));

        emit downloadFinished(false, d->model);

        reply->deleteLater();

        return;
    }

    if      (d->modelArray.isEmpty())
    {
        d->modelArray = reply->readAll();
    }
    else if (d->dataArray.isEmpty())
    {
        d->dataArray = reply->readAll();
    }

    reply->deleteLater();

    nextDownload();
}

} // namespace Digikam
