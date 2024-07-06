/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

#include "KmlColorStyleTagWriter.h"
#include "GeoTagWriter.h"

namespace Marble
{

class KmlLabelStyleTagWriter : public KmlColorStyleTagWriter
{
public:

    KmlLabelStyleTagWriter();

protected:

    bool writeMid( const GeoNode *node, GeoWriter &writer ) const override;
    bool isEmpty( const GeoNode *node ) const override;
    QColor defaultColor() const override;
};

} // namespace Marble
