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

#include "vkontakte_noteslistjob.h"

// Qt includes

#include <QVariant>
#include <qjson/qobjecthelper.h>

namespace Vkontakte
{

class Q_DECL_HIDDEN NotesListJob::Private
{
public:

    int                totalCount; ///< number of all notes, not only notes retrieved in this request
    QList<NoteInfoPtr> list;
};

NotesListJob::NotesListJob(const QString& accessToken,
                           int uid, int offset, int count)
    : VkontakteJob(accessToken, "notes.get"),
      d(new Private)
{
    // Not passing "nids", because we want all notes.

    if (uid > 0)
    {
        addQueryItem("user_id", QString::number(uid));
    }

    addQueryItem("sort",    "1"); // ascending by date
    addQueryItem("count",   QString::number(count));
    addQueryItem("offset",  QString::number(offset));
}

NotesListJob::~NotesListJob()
{
    delete d;
}

void NotesListJob::handleItem(const QVariant& data)
{
    NoteInfoPtr item(new NoteInfo());
    QJson::QObjectHelper::qvariant2qobject(data.toMap(), item.data());
    d->list.append(item);
}

void NotesListJob::handleData(const QVariant& data)
{
    QVariantList list = data.toList();
    d->totalCount = list[0].toInt();
    list.pop_front();

    foreach (const QVariant& item, list)
    {
        handleItem(item);
    }
}

QList<NoteInfoPtr> NotesListJob::list() const
{
    return d->list;
}

int NotesListJob::totalCount() const
{
    return d->totalCount;
}

} // namespace Vkontakte
