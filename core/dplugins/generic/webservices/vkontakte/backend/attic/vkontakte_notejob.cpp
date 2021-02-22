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

#include "vkontakte_notejob.h"

// Qt includes

#include <qjson/qobjecthelper.h>

namespace Vkontakte
{

class Q_DECL_HIDDEN NoteJob::Private
{
public:

    NoteInfoPtr noteInfo;
};

// https://vk.com/dev/notes.getById
NoteJob::NoteJob(const QString& accessToken, int nid)
    : VkontakteJob(accessToken, "notes.getById"),
      d(new Private)
{
    addQueryItem("nid",       QString::number(nid));
    addQueryItem("need_wiki", "1");                     // works only for current user's notes
}

NoteJob::~NoteJob()
{
    delete d;
}

void NoteJob::handleData(const QVariant& data)
{
    d->noteInfo = NoteInfoPtr(new NoteInfo());
    QJson::QObjectHelper::qvariant2qobject(data.toMap(), d->noteInfo.data());
}

NoteInfoPtr NoteJob::noteInfo()
{
    return d->noteInfo;
}

} // namespace Vkontakte
