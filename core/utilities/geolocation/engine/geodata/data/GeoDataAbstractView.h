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

#include "MarbleGlobal.h"
#include "GeoDataObject.h"
#include "digikam_export.h"

namespace Marble
{

class GeoDataCoordinates;
class GeoDataTimeSpan;
class GeoDataTimeStamp;
class GeoDataAbstractViewPrivate;

/**
 * @see GeoDataLookAt
 */
class DIGIKAM_EXPORT GeoDataAbstractView : public GeoDataObject
{
public:

    GeoDataAbstractView();

    ~GeoDataAbstractView() override;

    GeoDataAbstractView(const GeoDataAbstractView& other);

    GeoDataAbstractView& operator=(const GeoDataAbstractView& other);

    bool operator==(const GeoDataAbstractView& other) const;
    inline bool operator!=(const GeoDataAbstractView& other) const
    {
        return !(*this == other);
    }

    virtual GeoDataAbstractView* copy() const = 0;

    const GeoDataTimeSpan& timeSpan() const;

    GeoDataTimeSpan& timeSpan();

    void setTimeSpan(const GeoDataTimeSpan& timeSpan);

    GeoDataTimeStamp& timeStamp();

    const GeoDataTimeStamp& timeStamp() const;

    void setTimeStamp(const GeoDataTimeStamp& timeStamp);

    AltitudeMode altitudeMode() const;

    void setAltitudeMode(const AltitudeMode altitudeMode);

    GeoDataCoordinates coordinates() const;

private:

    GeoDataAbstractViewPrivate* const d = nullptr;

protected:

    bool equals(const GeoDataAbstractView& other) const;

    using GeoDataObject::equals;
};

} // namespace Marble
