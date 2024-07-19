/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team <marble dot kde dot org>
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "GeoTagWriter.h"
#include "GeoDataHotSpot.h"
#include "KmlColorStyleTagWriter.h"

namespace Marble
{

class KmlIconStyleTagWriter : public KmlColorStyleTagWriter
{
public:

    KmlIconStyleTagWriter();

protected:

    bool writeMid(const GeoNode* node, GeoWriter& writer) const override;
    bool isEmpty(const GeoNode* node) const override;

private:

    static QString unitString(GeoDataHotSpot::Units unit);
};

} // namespace Marble
