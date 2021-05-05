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

#ifndef DIGIKAM_VKONTAKTE_MESSAGESLISTJOB_H
#define DIGIKAM_VKONTAKTE_MESSAGESLISTJOB_H

// Local includes

#include "vkontakte_jobs.h"
#include "vkontakte_messageinfo.h"

namespace Vkontakte
{

// TODO: do not export in KF5 version
class MessagesListJob : public VkontakteJob
{
public:

    /**
     * @brief Constructs a job to retrieve an array of messages from vk.com.
     *
     * @param accessToken vk.com access token.
     * @param out 1 if you want outgoing messages. Defaults to 0.
     * @param offset Offset in the list of all discussions. Defaults to 0.
     * @param count Number of discussions to retrieve. Defaults to 100.
     * @param previewLength 0 means to not truncate messages. Defaults to 0.
     * @param filters ... Defaults to 0.
     * @param timeOffset ... Defaults to 0.
     **/
    explicit MessagesListJob(const QString& accessToken,
                    int out = 0,
                    int offset = 0, int count = 100, int previewLength = 0,
                    int filters = 0, int timeOffset = 0);
    ~MessagesListJob();

    QList<MessageInfoPtr> list() const;
    int totalCount()             const;

    /**
     * @brief Used by AllMessagesListJob
     **/
    int out() const;

protected:

    void handleData(const QVariant& data) override;
    void handleItem(const QVariant& data);

private:

    class Private;
    Private* const d;
};

} // namespace Vkontakte

#endif // DIGIKAM_VKONTAKTE_MESSAGESLISTJOB_H
