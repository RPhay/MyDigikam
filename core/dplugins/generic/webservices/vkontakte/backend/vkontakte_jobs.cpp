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

#include "vkontakte_jobs.h"

// Qt includes

#include <QTimer>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>

// KDE includes

#include <klocalizedstring.h>
#include <KIO/Job>
#include <KIO/StoredTransferJob>

// Local includes

#include "digikam_debug.h"

namespace Vkontakte
{

 KJobWithSubjob::KJobWithSubjob(QObject* const parent)
    : KJob(parent)
{
}

bool KJobWithSubjob::doKill()
{
    if (m_job)
    {
        m_job->kill(KJob::Quietly);
    }

    return KJob::doKill();
}

//---------------------------------------------------------------

 KJobWithSubjobs::KJobWithSubjobs(QObject* const parent)
    : KJob(parent)
{
}

bool KJobWithSubjobs::doKill()
{
    foreach (KJob* const job, m_jobs)
    {
        job->kill(KJob::Quietly);
    }

    m_jobs.clear();

    return KJob::doKill();
}

//---------------------------------------------------------------

/*
 * VkontakteJobs base class
 */
VkontakteJob::VkontakteJob(const QString& accessToken, const QString& method, bool httpPost)
    : m_accessToken(accessToken),
      m_method     (method),
      m_httpPost   (httpPost)
{
    setCapabilities(KJob::Killable);
}

void VkontakteJob::addQueryItem(const QString& key, const QString& value)
{
    QueryItem item;
    item.first  = key;
    item.second = value;
    m_queryItems.append(item);
}

bool VkontakteJob::handleError(const QJsonValue& data)
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

    if (error_code == 6)
    {
        // "Too many requests per second", we will retry after a delay.
        // VK API limit the rate of requests to 3 requests per second,
        // so it should be OK if we wait for 340 ms.

        QTimer::singleShot(340, this, SLOT(slotRetry()));

        return true;
    }
    else
    {
        setError(KJob::UserDefinedError);

        if (data.isUndefined())
        {
            setErrorText(i18n("Response from the VKontakte server has unexpected format. "
                              "Please report this problem against product digiKam "
                              "at the <a href=\"%1\">KDE bug tracker</a>.",
                         QLatin1String("https://bugs.kde.org/")));
        }
        else
        {
            setErrorText(i18n("The VKontakte server returned an error "
                              "of type <i>%1</i> in reply to method %2: <i>%3</i>",
                         error_code, m_method, error_msg));
        }

        return false;
    }
}

KJob* VkontakteJob::createHttpJob()
{
    QUrl url;
    url.setScheme(QLatin1String("https"));
    url.setHost(QLatin1String("api.vk.com"));
    url.setPath(QLatin1String("/method/") + m_method);

    // Collect query items in "query"

    QUrlQuery query;

    prepareQueryItems();

    foreach (const QueryItem& item, m_queryItems)
    {
        query.addQueryItem(item.first, item.second);
    }

    if (!m_accessToken.isEmpty())
    {
        query.addQueryItem(QLatin1String("access_token"), m_accessToken);
    }

    url.setQuery(query);

    // TODO: Save QUrl to reuse it if we need to retry the HTTP request
/*
    m_url = url;
*/
    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Starting request" << url;

    if (m_httpPost)
    {
        return KIO::storedHttpPost(QByteArray(), url, KIO::HideProgressInfo);
    }
    else
    {
        return KIO::storedGet(url, KIO::Reload, KIO::HideProgressInfo);
    }
}

void VkontakteJob::start()
{
    m_job = createHttpJob();

    connect(m_job, SIGNAL(result(KJob*)),
            this, SLOT(jobFinished(KJob*)));

    m_job->start();
}

void VkontakteJob::slotRetry()
{
    start();
}

void VkontakteJob::jobFinished(KJob* kjob)
{
    KIO::StoredTransferJob* const job = dynamic_cast<KIO::StoredTransferJob*>(kjob);
    Q_ASSERT(job);

    if      (job == nullptr)
    {
        setError(-1);
        setErrorText(i18n("Internal error: No valid instance of KIO::StoredTransferJob "
                          "passed into VkontakteJob::jobFinished."));

        qCWarning(DIGIKAM_WEBSERVICES_LOG) << "KIO::StoredTransferJob is null";
    }
    else if (job->error())
    {
        setError(job->error());
        setErrorText(KIO::buildErrorString(error(), job->errorText()));
        qCWarning(DIGIKAM_WEBSERVICES_LOG) << "Job error:" << job->errorString();
    }
    else
    {
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Got data:" << job->data();

        QJsonParseError parseError;
        QJsonDocument data = QJsonDocument::fromJson(job->data(), &parseError);

        if (parseError.error == QJsonParseError::NoError)
        {
            const QJsonObject object = data.object();

            if      (!data.isObject() ||
                     (!object.contains(QLatin1String("response")) && !object.contains(QLatin1String("error"))))
            {
                // Something went wrong, but there is no valid object "error"

                handleError(QJsonValue::Undefined);
            }
            else if (object.contains(QLatin1String("error")))
            {
                bool willRetry = handleError(object.value(QLatin1String("error")));

                if (willRetry)
                {
                    return;
                }
            }
            else
            {
                handleData(object.value(QLatin1String("response")));
            }
        }
        else
        {
            qCWarning(DIGIKAM_WEBSERVICES_LOG) << "Unable to parse JSON data:" << parseError.errorString();
            qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Received data:" << job->data();

            setError(KJob::UserDefinedError);
            setErrorText(i18n("Unable to parse data returned by the VKontakte server: %1",
                         parseError.errorString()));
        }
    }

    emitResult();

    m_job = nullptr;
}

} // namespace Vkontakte
