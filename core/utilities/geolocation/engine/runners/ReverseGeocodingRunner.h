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

#include <QObject>

#include "digikam_export.h"

namespace Marble
{

class GeoDataCoordinates;
class GeoDataPlacemark;
class MarbleModel;

class DIGIKAM_EXPORT ReverseGeocodingRunner : public QObject
{
    Q_OBJECT

public:

    explicit ReverseGeocodingRunner( QObject *parent );

    /**
     * Stores a pointer to the currently used model
     */
    void setModel( const MarbleModel *model );

    /**
     * Start a reverse geocoding request. Called by MarbleRunnerManager, runners
     * are expected to return the result via the reverseGeocodingFinished signal.
     * If implemented in a plugin, make sure to include ReverseGeocoding in the
     * plugin capabilities, otherwise MarbleRunnerManager will ignore the plugin
     */
    virtual void reverseGeocoding( const GeoDataCoordinates &coordinates ) = 0;

Q_SIGNALS:

    /**
     * Reverse geocoding is finished, result in the given placemark.
     * To be Q_EMITted by runners after a @see reverseGeocoding call.
     */
    void reverseGeocodingFinished( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark );

protected:

    /**
     * Access to the currently used model, or null if no was set with @see setModel
     */
    const MarbleModel *model() const;

private:

    const MarbleModel* m_model = nullptr;
};

} // namespace Marble
