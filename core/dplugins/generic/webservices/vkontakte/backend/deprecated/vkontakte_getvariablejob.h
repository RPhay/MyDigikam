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

#ifndef DIGIKAM_VKONTAKTE_GETVARIABLEJOB_H
#define DIGIKAM_VKONTAKTE_GETVARIABLEJOB_H

// Qt includes

#include <QVariant>

// Local includes

#include "vkontakte_jobs.h"

namespace Vkontakte
{

// For the info about specific variables see:
// http://vkontakte.ru/developers.php?o=-1&p=%D5%F0%E0%ED%E5%ED%E8%E5%20%E4%E0%ED%ED%FB%F5%20%ED%E0%20%F1%E5%F0%E2%E5%F0%E0%F5%20%C2%CA%EE%ED%F2%E0%EA%F2%E5
class GetVariableJob : public VkontakteJob
{

public:

    // "getVariable" is deprecated, please use "users.get" with empty user_ids
    Q_DECL_DEPRECATED GetVariableJob(const QString& accessToken, int index);
    ~GetVariableJob();

    QVariant variable() const;

protected:

    void handleData(const QVariant& data) override;

private:

    class Private;
    Private* const d;
};

} // namespace Vkontakte

#endif // DIGIKAM_VKONTAKTE_GETVARIABLEJOB_H
