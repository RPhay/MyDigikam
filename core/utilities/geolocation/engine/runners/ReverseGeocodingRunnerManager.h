// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Henry de Valence <hdevalence@gmail.com>
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2010-2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#pragma once

#include <QObject>

#include "digikam_export.h"

class QAbstractItemModel;

namespace Marble
{

class GeoDataCoordinates;
class GeoDataPlacemark;
class MarbleModel;
class ReverseGeocodingTask;

class DIGIKAM_EXPORT ReverseGeocodingRunnerManager : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor.
     * @param pluginManager The plugin manager that gives access to RunnerPlugins
     * @param parent Optional parent object
     */
    explicit ReverseGeocodingRunnerManager( const MarbleModel *marbleModel, QObject *parent = nullptr );

    ~ReverseGeocodingRunnerManager() override;

    /**
     * Find the address and other meta information for a given geoposition.
     * @see reverseGeocoding is asynchronous with currently one result
     * returned using the @see reverseGeocodingFinished signal.
     * @see searchReverseGeocoding is blocking.
     * @see reverseGeocodingFinished signal indicates all runners are finished.
     */
    void reverseGeocoding( const GeoDataCoordinates &coordinates );
    QString searchReverseGeocoding( const GeoDataCoordinates &coordinates, int timeout = 30000 );

Q_SIGNALS:
    /**
     * The reverse geocoding request is finished, the result is stored
     * in the given placemark. This signal is Q_EMITted when the first
     * runner found a result, subsequent results are discarded and do not
     * Q_EMIT further signals. If no result is found, this signal is Q_EMITted
     * with an empty (default constructed) placemark.
     */
    void reverseGeocodingFinished( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark );

    /**
     * Emitted whenever all runners are finished for the query
     */
    void reverseGeocodingFinished();

private:
    Q_PRIVATE_SLOT( d, void addReverseGeocodingResult( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark ) )
    Q_PRIVATE_SLOT( d, void cleanupReverseGeocodingTask( ReverseGeocodingTask *task ) )

    class Private;
    friend class Private;
    Private *const d;
};

} // namespace Marble
