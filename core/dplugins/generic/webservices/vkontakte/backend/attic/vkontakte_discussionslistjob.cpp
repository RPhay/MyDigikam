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

#include "vkontakte_discussionslistjob.h"

// Qt includes

#include <qjson/qobjecthelper.h>

namespace Vkontakte
{

class Q_DECL_HIDDEN DiscussionsListJob::Private
{
public:

    int                   totalCount; ///< number of all discussions, not only discussions retrieved in this request
    QList<MessageInfoPtr> list;
};

DiscussionsListJob::DiscussionsListJob(const QString& accessToken, int offset, int count, int previewLength)
    : VkontakteJob(accessToken, "messages.getDialogs"),
      d(new Private)
{
    addQueryItem("offset",         QString::number(offset));
    addQueryItem("count",          QString::number(count));
    addQueryItem("preview_length", QString::number(previewLength));
}

DiscussionsListJob::~DiscussionsListJob()
{
    delete d;
}

void DiscussionsListJob::handleItem(const QVariant& data)
{
    MessageInfoPtr item(new MessageInfo());
    QJson::QObjectHelper::qvariant2qobject(data.toMap(), item.data());
    d->list.append(item);
}

void DiscussionsListJob::handleData(const QVariant& data)
{
    QVariantList list = data.toList();
    d->totalCount     = list[0].toInt();
    list.pop_front();

    foreach (const QVariant& item, list)
    {
        handleItem(item);
    }

    qSort(d->list); // sort by message ID (which should be equivalent to sorting by date)
}

QList<MessageInfoPtr> DiscussionsListJob::list() const
{
    return d->list;
}

int DiscussionsListJob::totalCount() const
{
    return d->totalCount;
}

} // namespace Vkontakte
