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

#include "GeoDocument.h"
#include "digikam_export.h"

namespace Marble
{

/**
 * @short Zoom properties of a GeoScene document.
 */
class DIGIKAM_EXPORT GeoSceneZoom : public GeoNode
{
public:

    GeoSceneZoom();

    int minimum() const;
    void setMinimum(int name);
    int maximum() const;
    void setMaximum(int target);
    bool discrete() const;
    void setDiscrete(bool theme);

    const char* nodeType() const override;

private:

    int  m_minimum;
    int  m_maximum;
    bool m_discrete;
};

} // namespace Marble
