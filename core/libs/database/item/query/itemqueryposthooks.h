/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-03-22
 * Description : database SQL queries helper class
 *
 * SPDX-FileCopyrightText: 2007-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ITEM_QUERY_POST_HOOKS_H
#define DIGIKAM_ITEM_QUERY_POST_HOOKS_H

// Qt includes

#include <QList>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class Q_DECL_HIDDEN ItemQueryPostHook
{
public:

    /// This is the single hook, ItemQueryPostHookS is the container
    ItemQueryPostHook()          = default;
    virtual ~ItemQueryPostHook() = default;

    virtual bool checkPosition(double /*latitudeNumber*/, double /*longitudeNumber*/)
    {
        return true;
    };

private:

    Q_DISABLE_COPY(ItemQueryPostHook)
};

// --------------------------------------------------------------------

class DIGIKAM_DATABASE_EXPORT ItemQueryPostHooks
{
public:

    explicit ItemQueryPostHooks();
    ~ItemQueryPostHooks();

    /**
     * Call this method after passing the object to buildQuery
     * and executing the statement. Returns true if the search is matched.
     */
    bool checkPosition(double latitudeNumber, double longitudeNumber);

    /**
     * Called by ItemQueryBuilder. Ownership of the object is passed.
     */
    void addHook(ItemQueryPostHook* const hook);

protected:

    QList<ItemQueryPostHook*> m_postHooks;
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_QUERY_POST_HOOKS_H
