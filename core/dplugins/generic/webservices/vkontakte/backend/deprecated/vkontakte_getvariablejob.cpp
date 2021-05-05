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

#include "vkontakte_getvariablejob.h"

namespace Vkontakte
{

class Q_DECL_HIDDEN GetVariableJob::Private
{
public:

    QVariant variable;
};

GetVariableJob::GetVariableJob(const QString& accessToken, int index)
    : VkontakteJob(accessToken, "getVariable"),
      d(new Private)
{
    addQueryItem("key", QString::number(index));
}

GetVariableJob::~GetVariableJob()
{
    delete d;
}

void GetVariableJob::handleData(const QVariant& data)
{
    d->variable = data;
}

QVariant GetVariableJob::variable() const
{
    return d->variable;
}

} // namespace Vkontakte
