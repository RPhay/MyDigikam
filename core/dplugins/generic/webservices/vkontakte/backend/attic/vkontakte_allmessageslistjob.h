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

#ifndef DIGIKAM_VKONTAKTE_ALLMESSAGESLISTJOB_H
#define DIGIKAM_VKONTAKTE_ALLMESSAGESLISTJOB_H

// Local includes

#include "vkontakte_messageslistjob.h"

namespace Vkontakte
{

class AllMessagesListJob : public KJobWithSubjobs
{
    Q_OBJECT

public:

    /**
     * @brief ...
     *
     * @param accessToken ...
     * @param out -1 means to retrieve both incoming and outgoing messages. Defaults to -1.
     * @param previewLength ... Defaults to 0.
     * @param filters ... Defaults to 0.
     * @param timeOffset ... Defaults to 0.
     **/
    explicit AllMessagesListJob(const QString& accessToken,
                       int out = -1, int previewLength = 0,
                       int filters = 0, int timeOffset = 0);
    ~AllMessagesListJob();

    void start() override;

    QList<MessageInfoPtr> list() const;
    int count()                  const;

protected:

    void startNewJob(int offset, int count, int out);

private Q_SLOTS:

    void jobFinished(KJob* kjob);

private:

    class Private;
    Private* const d;
};

} // namespace Vkontakte

#endif // DIGIKAM_VKONTAKTE_ALLMESSAGESLISTJOB_H
