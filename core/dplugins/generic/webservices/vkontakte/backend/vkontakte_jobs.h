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

#ifndef DIGIKAM_VKONTAKTE_JOBS_H
#define DIGIKAM_VKONTAKTE_JOBS_H

// Qt includes

#include <QPointer>

// KDE includes

#include <KJob>

namespace Vkontakte
{

typedef QPair<QString, QString> QueryItem;

class KJobWithSubjob : public KJob
{
    Q_OBJECT

public:

     explicit KJobWithSubjob(QObject* const parent = nullptr);

protected:

    /**
     * Kill the currentjobs and its subjobs
     */
    bool doKill() override;

    /**
     * Pointer to the running job
     */
    QPointer<KJob> m_job;
};

// ------------------------------------------------------------

class KJobWithSubjobs : public KJob
{
    Q_OBJECT

public:

     explicit KJobWithSubjobs(QObject* const parent = nullptr);

protected:

    /**
     * Kill the currentjobs and its subjobs
     */
    bool doKill() override;

    /**
     * Pointers to running jobs
     */
    QList<KJob*> m_jobs;
};

// ------------------------------------------------------------

/**
 * VkontakteJob base class
 */
class VkontakteJob : public KJobWithSubjob
{
    Q_OBJECT

public:

    enum JobErrorType
    {
        AuthenticationProblem = KJob::UserDefinedError + 42
    };

public:

    /**
     * Constructor that sets the path and the accesstoken
     *
     * @param method For the list of methods see https://vk.com/dev/methods (in English or Russian)
     * @param accessToken The accessToken to access our data on vkontakte
     * @param httpPost Whether to make a POST http request instead of GET ("false" by default)
     * */
    VkontakteJob(const QString& accessToken, const QString& method, bool httpPost = false);

    void start() override;

protected:

    /**
     * Add a query item to the list
     */
    void addQueryItem(const QString& key, const QString& value);

private:

    /**
     * Check for a return error and set the appropriate error messages
     */

    /**
     * Returns "true" if we will retry the call.
     */
    bool handleError(const QJsonValue& data);

    virtual void handleData(const QJsonValue& data) = 0;

    /**
     * Called right before sending request to server
     */
    virtual void prepareQueryItems() {}

    // TODO: cache url in a member variable
    KJob* createHttpJob();

private Q_SLOTS:

    void jobFinished(KJob* kjob);
    void slotRetry();

private:

    QString          m_accessToken;         ///< Vkontakte Access token
    QString          m_method;
    bool             m_httpPost;
    QList<QueryItem> m_queryItems;          ///< The query items

private:

    // Disable
    explicit VkontakteJob(QObject*) = delete;
};

} // namespace Vkontakte

#endif // DIGIKAM_VKONTAKTE_JOBS_H
