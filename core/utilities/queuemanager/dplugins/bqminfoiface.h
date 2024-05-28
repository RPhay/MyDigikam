/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-06
 * Description : interface to database information for BQM tools.
 *
 * SPDX-FileCopyrightText: 2017-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_BQM_INFO_IFACE_H
#define DIGIKAM_BQM_INFO_IFACE_H

// Qt includes

#include <QStringList>

// Local includes

#include "dbinfoiface.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT BqmInfoIface : public DBInfoIface
{
    Q_OBJECT

public:

    explicit BqmInfoIface(QObject* const parent);
    ~BqmInfoIface()                                          override = default;

public:

    /**
     * Return pending item paths from the current queue.
     */
    QStringList pendingItemPathsFromCurrentQueue()      const;

    /**
     * Return selected item paths from the current queue.
     */
    QStringList selectedItemPathsFromCurrentQueue()     const;

    /**
     * Return all item paths from the current queue.
     */
    QStringList allItemPathsFromCurrentQueue()          const;

private:

    QStringList itemPathsFromCurrentQueue(int type)     const;
};

} // namespace Digikam

#endif // DIGIKAM_BQM_INFO_IFACE_H
