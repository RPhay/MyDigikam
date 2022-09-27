/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-06-27
 * Description : Database engine element configuration settings
 *
 * SPDX-FileCopyrightText: 2009-2010 by Holger Foerster <hamsi2k at freenet dot de>
 * SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DB_ENGINE_CONFIG_SETTINGS_H
#define DIGIKAM_DB_ENGINE_CONFIG_SETTINGS_H

// Qt includes

#include <QMap>

// Local includes

#include "dbengineaction.h"

namespace Digikam
{

class DbEngineConfigSettings
{

public:

    QString                       databaseID;
    QString                       hostName;
    QString                       port;
    QString                       connectOptions;
    QString                       databaseName;
    QString                       userName;
    QString                       password;
    QMap<QString, DbEngineAction> sqlStatements;
};

} // namespace Digikam

#endif // DIGIKAM_DB_ENGINE_CONFIG_SETTINGS_H
