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

#include <QString>
#include <QVector>

#include "GeoDocument.h"

#include "digikam_export.h"

namespace Marble
{

class GeoSceneAbstractDataset;
class GeoSceneFilter;
class GeoSceneLayerPrivate;

/**
 * @short Layer of a GeoScene document.
 */
class DIGIKAM_EXPORT GeoSceneLayer : public GeoNode
{
public:

    explicit GeoSceneLayer( const QString& name );
    ~GeoSceneLayer() override;

    const char* nodeType() const override;

    /**
     * @brief  Add a data set to the legend
     * @param  dataset  the new data set
     */
    void addDataset( GeoSceneAbstractDataset* );

    const GeoSceneAbstractDataset * dataset( const QString& ) const;
    GeoSceneAbstractDataset * dataset( const QString& );

    const GeoSceneAbstractDataset * groundDataset() const;
    GeoSceneAbstractDataset * groundDataset();

    QVector<GeoSceneAbstractDataset*> datasets() const;

    QString name() const;

    QString backend() const;
    void setBackend( const QString& plugin );

    /**
     * @brief  returns whether the data is organized in quad tiles.
     */
    bool isTiled() const;
    void setTiled( bool );

    QString role() const;
    void setRole( const QString& type );

    const GeoSceneFilter * filter() const;
    GeoSceneFilter * filter();
    void addFilter( GeoSceneFilter * filter );
    void removeFilter( GeoSceneFilter * filter );

private:

    Q_DISABLE_COPY( GeoSceneLayer )

private:

    GeoSceneLayerPrivate* const d = nullptr;
};

} // namespace Marble
