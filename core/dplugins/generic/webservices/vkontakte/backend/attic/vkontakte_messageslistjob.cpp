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

#include "vkontakte_messageslistjob.h"

// Qt includes

#include <qjson/qobjecthelper.h>

namespace Vkontakte
{

class Q_DECL_HIDDEN MessagesListJob::Private
{
public:

    int                   out;
    int                   totalCount; ///< number of all messages, not only messages retrieved in this request
    QList<MessageInfoPtr> list;
};

MessagesListJob::MessagesListJob(const QString& accessToken,
                                 int out,
                                 int offset, int count, int previewLength,
                                 int filters, int timeOffset)
    : VkontakteJob(accessToken, "messages.get"),
      d(new Private)
{
    d->out        = out;
    d->totalCount = 0;

    addQueryItem("out",             QString::number(out));
    addQueryItem("offset",          QString::number(offset));
    addQueryItem("count",           QString::number(count));
    addQueryItem("filters",         QString::number(filters));
    addQueryItem("preview_length",  QString::number(previewLength));
    addQueryItem("time_offset",     QString::number(timeOffset));
}

MessagesListJob::~MessagesListJob()
{
    delete d;
}

void MessagesListJob::handleItem(const QVariant& data)
{
    MessageInfoPtr item(new MessageInfo());
    QJson::QObjectHelper::qvariant2qobject(data.toMap(), item.data());
    d->list.append(item);
}

void MessagesListJob::handleData(const QVariant& data)
{
    QVariantList list = data.toList();
    d->totalCount     = list[0].toInt();
    list.pop_front();

    foreach (const QVariant& item, list)
    {
        handleItem(item);
    }
}

QList<MessageInfoPtr> MessagesListJob::list() const
{
    return d->list;
}

int MessagesListJob::totalCount() const
{
    return d->totalCount;
}

int MessagesListJob::out() const
{
    return d->out;
}

} // namespace Vkontakte
