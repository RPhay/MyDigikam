/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-03-22
 * Description : database SQL queries helper class
 *
 * SPDX-FileCopyrightText: 2007-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "itemqueryposthooks.h"

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

ItemQueryPostHooks::~ItemQueryPostHooks()
{
    Q_FOREACH (const ItemQueryPostHook* const hook, m_postHooks)
    {
        delete hook;
    }
}

void ItemQueryPostHooks::addHook(ItemQueryPostHook* const hook)
{
    m_postHooks << hook;
}

bool ItemQueryPostHooks::checkPosition(double latitudeNumber, double longitudeNumber)
{
    Q_FOREACH (ItemQueryPostHook* const hook, m_postHooks)
    {
        if (!hook->checkPosition(latitudeNumber, longitudeNumber))
        {   // cppcheck-suppress useStlAlgorithm
            return false;
        }
    }

    return true;
}

} // namespace Digikam
