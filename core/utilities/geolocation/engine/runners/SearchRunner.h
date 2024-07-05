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
#include <QVector>

#include "GeoDataPlacemark.h"

#include "digikam_export.h"

namespace Marble
{

class GeoDataLatLonBox;
class MarbleModel;

class DIGIKAM_EXPORT SearchRunner : public QObject
{
    Q_OBJECT

public:

    explicit SearchRunner( QObject *parent = nullptr );

    /**
     * Stores a pointer to the currently used model
     */
    void setModel( const MarbleModel *model );

    /**
     * Start a placemark search. Called by MarbleRunnerManager, runners
     * are expected to return the result via the searchFinished signal.
     * If implemented in a plugin, make sure to include Search in the
     * plugin capabilities, otherwise MarbleRunnerManager will ignore the plugin
     */
    virtual void search( const QString &searchTerm, const GeoDataLatLonBox &preferred ) = 0;

Q_SIGNALS:

    /**
     * This is Q_EMITted to indicate that the runner has finished the placemark search.
     * @param result the result of the search.
     */
    void searchFinished( const QVector<GeoDataPlacemark*>& result );

protected:

    /**
     * Access to the currently used model, or null if no was set with @see setModel
     */
    const MarbleModel *model() const;

private:

    const MarbleModel* m_model = nullptr;
};

} // namespace Marble
