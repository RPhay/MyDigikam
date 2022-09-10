/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-01
 * Description : class GroupStateComputer
 *
 * SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2010 by Michael G. Hansen <mike at mghansen dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_GROUP_STATE_COMPUTER_H
#define DIGIKAM_GROUP_STATE_COMPUTER_H

#include "geogroupstate.h"

// Qt includes

#include <QScopedPointer>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT GroupStateComputer
{
public:

    explicit GroupStateComputer();
    virtual ~GroupStateComputer();

    GeoGroupState getState() const;

    void clear();

    void addState(const GeoGroupState state);
    void addSelectedState(const GeoGroupState state);
    void addFilteredPositiveState(const GeoGroupState state);
    void addRegionSelectedState(const GeoGroupState state);

private:

    class Private;
    const QScopedPointer<Private> d;
};

} // namespace Digikam

#endif // DIGIKAM_GROUP_STATE_COMPUTER_H
