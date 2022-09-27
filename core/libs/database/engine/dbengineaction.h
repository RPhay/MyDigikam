/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-06-27
 * Description : Database engine action
 *
 * SPDX-FileCopyrightText: 2009-2010 by Holger Foerster <hamsi2k at freenet dot de>
 * SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DB_ENGINE_ACTION_H
#define DIGIKAM_DB_ENGINE_ACTION_H

// Qt includes

#include <QList>
#include <QString>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT DbEngineActionElement
{
public:

    explicit DbEngineActionElement()
        : order(0)
    {
    }

    QString mode;
    int     order;
    QString statement;
};

// ---------------------------------------------------------------

class DIGIKAM_EXPORT DbEngineAction
{
public:

    QString                      name;
    QString                      mode;
    QList<DbEngineActionElement> dbActionElements;
};

} // namespace Digikam

#endif // DIGIKAM_DB_ENGINE_ACTION_H
