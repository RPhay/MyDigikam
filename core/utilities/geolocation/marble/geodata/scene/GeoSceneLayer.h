/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QString>
#include <QVector>

#include "digikam_export.h"

#include "GeoDocument.h"

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

    GeoSceneLayerPrivate * const d;
};

} // namespace Marble
