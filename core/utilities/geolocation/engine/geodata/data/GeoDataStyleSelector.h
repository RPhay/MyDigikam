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

// Local includes

#include "GeoDataObject.h"
#include "digikam_export.h"

namespace Marble
{

class GeoDataStyleSelectorPrivate;

/**
 * @short an abstract base class for the style classes
 *
 * A GeoDataStyleSelector is a base class for the style classes Style
 * and StyleMap classes. The StyleMap class selects a style based on
 * the current mode of a Placemark (highlighted or normal). An element
 * derived from StyleSelector is uniquely identified by its id and its
 * url.
 *
 * @sa GeoDataStyle
 * @sa GeoDataStyleMap
 */
class DIGIKAM_EXPORT GeoDataStyleSelector : public GeoDataObject
{
public:

    ~GeoDataStyleSelector() override;

    /**
    * @brief assignment operator
    */
    GeoDataStyleSelector& operator=(const GeoDataStyleSelector& other);

    bool operator==(const GeoDataStyleSelector& other) const;
    bool operator!=(const GeoDataStyleSelector& other) const;

    /**
     * @brief Serialize the styleselector to a stream
     * @param  stream  the stream
     */
    void pack(QDataStream& stream) const override;

    /**
     * @brief  Unserialize the styleselector from a stream
     * @param  stream  the stream
     */
    void unpack(QDataStream& stream) override;

protected:

    GeoDataStyleSelector();
    GeoDataStyleSelector(const GeoDataStyleSelector& other);

private:

    GeoDataStyleSelectorPrivate* const d = nullptr;
};

} // namespace Marble
