/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-05-05
 * Description : Factory to create instances of Lookup backends
 *
 * SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2010-2011 by Michael G. Hansen <mike at mghansen dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_LOOKUP_FACTORY_H
#define DIGIKAM_LOOKUP_FACTORY_H

// Local includes

#include "geoifacetypes.h"
#include "digikam_export.h"

namespace Digikam
{

class LookupAltitude;

class DIGIKAM_EXPORT LookupFactory
{
public:

    static LookupAltitude* getAltitudeLookup(const QString& backendName,
                                             QObject* const parent);
};

} // namespace Digikam

#endif // DIGIKAM_LOOKUP_FACTORY_H
