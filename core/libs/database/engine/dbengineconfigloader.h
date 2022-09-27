/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-06-27
 * Description : Database Engine element configuration loader
 *
 * SPDX-FileCopyrightText: 2009-2010 by Holger Foerster <hamsi2k at freenet dot de>
 * SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DB_ENGINE_CONFIG_LOADER_H
#define DIGIKAM_DB_ENGINE_CONFIG_LOADER_H

#include "dbengineconfigsettings.h"

// Qt includes

#include <QString>
#include <QMap>
#include <QDomElement>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT DbEngineConfigSettingsLoader
{
public:

    explicit DbEngineConfigSettingsLoader(const QString& filepath, int xmlVersion);

    bool readConfig(const QString& filepath, int xmlVersion);
    DbEngineConfigSettings readDatabase(QDomElement& databaseElement);

    void readDBActions(const QDomElement& sqlStatementElements, DbEngineConfigSettings& configElement);

public:

    bool                                  isValid;
    QString                               errorMessage;
    QMap<QString, DbEngineConfigSettings> databaseConfigs;
};

} // namespace Digikam

#endif // DIGIKAM_DB_ENGINE_CONFIG_LOADER_H
