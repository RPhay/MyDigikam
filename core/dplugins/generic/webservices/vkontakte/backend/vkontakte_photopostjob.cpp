/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : a tool to export images to VKontakte web service
 *
 * Copyright (C) 2011-2015 by Alexander Potashev <aspotashev at gmail dot com>
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "vkontakte_photopostjob.h"

// Qt includes

#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHttpMultiPart>

// KDE includes

#include <KIO/Job>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"

namespace Vkontakte
{

PhotoPostJob::PhotoPostJob(Vkontakte::UploadPhotosJob::Dest dest,
                           const QUrl& url, const QStringList& files)
    : m_dest (dest),
      m_url  (url),
      m_files(files),
      m_ok   (true)
{
    setCapabilities(KJob::Killable);

    if ((files.size() <= 0) || (files.size() > 5))
    {
        m_ok = false;
    }
}

void PhotoPostJob::handleError(const QJsonValue& data)
{
    int error_code = -1;
    QString error_msg;

    if (data.isUndefined())
    {
        qCWarning(DIGIKAM_WEBSERVICES_LOG) << "Response from server has unexpected format";
    }
    else
    {
        const QVariantMap errorMap = data.toVariant().toMap();
        error_code                 = errorMap[QLatin1String("error_code")].toInt();
        error_msg                  = errorMap[QLatin1String("error_msg")].toString();

        qCWarning(DIGIKAM_WEBSERVICES_LOG) << "An error of type" << error_code << "occurred:" << error_msg;
    }

    setError(KJob::UserDefinedError);

    if (data.isUndefined())
    {
        setErrorText(i18n(
            "Response from the VKontakte server has unexpected format. "
            "Please report this problem against product libkvkontakte "
            "at the <a href=\"%1\">KDE bug tracker</a>.",
            QLatin1String("https://bugs.kde.org/")));
    }
    else
    {
        setErrorText(i18n(
            "The VKontakte server returned an error "
            "of type <i>%1</i> in reply to uploading to URL %2: <i>%3</i>",
            error_code, m_url.toString(), error_msg));
    }
}

bool PhotoPostJob::appendFile(QHttpMultiPart* multiPart, const QString& header, const QString& path)
{
    QString mime = QMimeDatabase().mimeTypeForUrl(QUrl(path)).name();

    if (mime.isEmpty())
    {
        return false;
    }

    QFileInfo fileInfo(path);

    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant(QString::fromUtf8("form-data; name=\"%1\"; filename=\"%2\"")
                            .arg(header).arg(fileInfo.fileName())));
    imagePart.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(fileInfo.size()));
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(mime));
    QFile* const file = new QFile(path);

    if (!file->open(QIODevice::ReadOnly))
    {
        delete file;
        return false;
    }

    imagePart.setBodyDevice(file);
    file->setParent(multiPart); // we cannot delete the file now, so delete it with the multiPart

    multiPart->append(imagePart);

    return true;
}

void PhotoPostJob::start()
{
    if (!m_ok)
    {
        setError(UserDefinedError);
        setErrorText(QLatin1String("Internal error"));
        emitResult();
    }

    QHttpMultiPart* const multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    switch (m_dest)
    {
        case Vkontakte::UploadPhotosJob::DEST_ALBUM:
        {
            // "file1" .. "file5"
            for (int i = 0 ; i < m_files.size() ; i++)
            {
                if (!appendFile(multiPart, QString::fromLatin1("file%1").arg(i + 1), m_files[i]))
                {
                    m_ok = false;
                    break;
                }
            }

            break;
        }

        case Vkontakte::UploadPhotosJob::DEST_PROFILE:
        case Vkontakte::UploadPhotosJob::DEST_WALL:
        {
            // "photo"
            if (!appendFile(multiPart, QLatin1String("photo"), m_files[0]))
            {
                m_ok = false;
            }

            break;
        }

        default:
        {
            m_ok = false;
            break;
        }
    }

    if (!m_ok)
    {
        setError(UserDefinedError);
        setErrorText(QLatin1String("Could not attach file"));
        emitResult();
    }

    QNetworkAccessManager* manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(parseNetworkResponse(QNetworkReply*)));

    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Starting POST request" << m_url;
    QNetworkReply* const reply = manager->post(QNetworkRequest(m_url), multiPart);
    multiPart->setParent(reply); // delete the multiPart with the reply
}

void PhotoPostJob::parseNetworkResponse(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        // A communication error has occurred
        setError(reply->error());
        setErrorText(KIO::buildErrorString(error(), QString()));
//         qCWarning(DIGIKAM_WEBSERVICES_LOG) << "Network error: " << reply->errorString();
    }
    else
    {
        QByteArray ba      = reply->readAll();
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Got data:" << ba;

        QJsonParseError parseError;
        QJsonDocument data = QJsonDocument::fromJson(ba, &parseError);

        if (parseError.error == QJsonParseError::NoError)
        {
            const QJsonObject object = data.object();

            if      (!data.isObject())
            {
                // Something went wrong, but there is no valid object "error"
                handleError(QJsonValue::Undefined);
            }
            else if (object.contains(QLatin1String("error")))
            {
                handleError(object.value(QLatin1String("error")));
            }
            else
            {
                // Handle data
                m_response = object.toVariantMap();
            }
        }
        else
        {
            qCWarning(DIGIKAM_WEBSERVICES_LOG) << "Unable to parse JSON data:" << ba;
            setError(KJob::UserDefinedError);
            setErrorText(
                i18n("Unable to parse data returned by the VKontakte server: %1",
                     parseError.errorString()));
        }
    }

    emitResult();
}

QVariantMap PhotoPostJob::response() const
{
    return m_response;
}

} // namespace Vkontakte
