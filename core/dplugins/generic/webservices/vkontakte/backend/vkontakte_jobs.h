/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : a tool to export images to VKontakte web service
 *
 * Copyright (C) 2011-2015 by Alexander Potashev <aspotashev at gmail dot com>
 * Copyright (C) 2011-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef VKONTAKTEJOBS_H
#define VKONTAKTEJOBS_H

#include <KJob>

#include <QPointer>

namespace Vkontakte
{

typedef QPair<QString, QString> QueryItem;

class KJobWithSubjob : public KJob
{
protected:
    /** Kill the currentjobs and its subjobs */
    bool doKill() Q_DECL_OVERRIDE;

    QPointer<KJob> m_job; /** Pointer to the running job */
};

class KJobWithSubjobs : public KJob
{
protected:
    /** Kill the currentjobs and its subjobs */
    bool doKill() Q_DECL_OVERRIDE;

    QList<KJob *> m_jobs; /** Pointers to running jobs */
};

/**
 * VkontakteJob base class
 */
class VkontakteJob : public KJobWithSubjob
{
    Q_OBJECT
public:
    /**
     * Constructor that sets the path and the accesstoken
     *
     * @param method For the list of methods see http://vk.com/dev/methods (in English or Russian)
     * @param accessToken The accessToken to access our data on vkontakte
     * @param httpPost Whether to make a POST http request instead of GET ("false" by default)
     * */
    VkontakteJob(const QString &accessToken, const QString &method, bool httpPost = false);

    void start() Q_DECL_OVERRIDE;

    enum JobErrorType { AuthenticationProblem = KJob::UserDefinedError + 42 };

protected:
    /** Add a query item to the list */
    void addQueryItem(const QString &key, const QString &value);

private:
    /** Check for a return error and set the appropriate error messages */
    // Returns "true" if we will retry the call.
    bool handleError(const QJsonValue &data);

    virtual void handleData(const QJsonValue &data) = 0;

    /** Called right before sending request to server */
    virtual void prepareQueryItems() {}

    // TODO: cache url in a member variable
    KJob* createHttpJob();

    QString m_accessToken;         /** Vkontakte Access token */
    QString m_method;
    bool m_httpPost;
    QList<QueryItem> m_queryItems; /** The query items */

private Q_SLOTS:
    void jobFinished(KJob *kjob);
    void slotRetry();
};

} /* namespace Vkontakte */

#endif // VKONTAKTEJOBS_H
