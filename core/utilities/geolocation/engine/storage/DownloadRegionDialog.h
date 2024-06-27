// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
//

#pragma once

#include <QDialog>
#include <QVector>

#include <MarbleGlobal.h>

#include "digikam_export.h"

namespace Marble
{

class GeoDataLatLonAltBox;
class TileCoordsPyramid;
class MarbleWidget;

class DIGIKAM_EXPORT DownloadRegionDialog: public QDialog
{
    Q_OBJECT

public:

    enum SelectionMethod { VisibleRegionMethod, SpecifiedRegionMethod, RouteDownloadMethod };

    explicit DownloadRegionDialog( MarbleWidget *const widget, QWidget * const parent = nullptr,
                                   Qt::WindowFlags const f = Qt::WindowFlags() );
    ~DownloadRegionDialog() override;
    void setAllowedTileLevelRange( int const minimumTileLevel,
                                   int const maximumTileLevel );
    void setVisibleTileLevel( int const tileLevel );
    void setSelectionMethod( SelectionMethod const );

    QVector<TileCoordsPyramid> region() const;

    TileType tileType() const;

public Q_SLOTS:

    void setSpecifiedLatLonAltBox( GeoDataLatLonAltBox const & );
    void setVisibleLatLonAltBox( GeoDataLatLonAltBox const & );
    void updateTileLayer();
    void delayUpdateTileLayer();

Q_SIGNALS:

    /// This signal is Q_EMITted when the "Apply" button is pressed.
    void applied();
    /// This signal is Q_EMITted when the dialog receives a QHideEvent.
    void hidden();
    /// This signal is Q_EMITted when the dialog receives a QShowEvent
    void shown();

protected:

    void hideEvent( QHideEvent * event ) override;
    void showEvent( QShowEvent * event ) override;

private Q_SLOTS:

    void toggleSelectionMethod();
    void updateTileCount();
    void updateTileType();

    /// This slot is called upon to update the route download UI when a route exists
    void updateRouteDialog();
    /// This slot sets the unit of the offset(m or km) in the spinbox
    void setOffsetUnit();

private:

    Q_DISABLE_COPY( DownloadRegionDialog )
    class Private;
    Private* const d = nullptr;
};

} // namespace Marble
