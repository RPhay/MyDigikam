/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-01
 * Description : world map widget library
 *
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2011 by Michael G. Hansen <mike at mghansen dot de>
 * SPDX-FileCopyrightText:      2014 by Justus Schwartz <justus at gmx dot li>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "mapwidget.h"

// C++ includes

#include <cmath>

// Qt includes

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QItemSelectionModel>
#include <QMenu>
#include <QPainter>
#include <QPointer>
#include <QStackedLayout>
#include <QTimer>
#include <QPushButton>
#include <QToolButton>
#include <QHBoxLayout>
#include <QAction>
#include <QActionGroup>
#include <QFrame>

// KDE includes

#include <klocalizedstring.h>
#include <kconfiggroup.h>

// Marble includes

#ifdef HAVE_GEOLOCATION

#   include "GeoDataLineString.h"
#   include "GeoDataLatLonBox.h"
#   include "MarbleGlobal.h"

#endif

// local includes

#include "geoifacecommon.h"
#include "geodragdrophandler.h"
#include "geomodelhelper.h"
#include "trackmanager.h"
#include "placeholderwidget.h"
#include "tilegrouper.h"
#include "digikam_debug.h"
#include "abstractmarkertiler.h"
#include "backendgooglemaps.h"
#include "geolocationsettings.h"

#ifdef HAVE_GEOLOCATION

#   include "backendmarble.h"

#endif

namespace Digikam
{

class Q_DECL_HIDDEN MapWidget::Private
{
public:

    explicit Private();

    QList<MapBackend*>      loadedBackends;
    MapBackend*             currentBackend;
    QString                 currentBackendName;
    QStackedLayout*         stackedLayout;

    /// NOTE: these values are cached in case the backend is not ready:
    GeoCoordinates          cacheCenterCoordinate;
    QString                 cacheZoom;

    /// actions for controlling the widget
    QMenu*                  configurationMenu;
    QActionGroup*           actionGroupBackendSelection;
    QAction*                actionZoomIn;
    QAction*                actionZoomOut;
    QAction*                actionShowThumbnails;
    QWidget*                mouseModesHolder;
    QPointer<QWidget>       controlWidget;
    QAction*                actionPreviewSingleItems;
    QAction*                actionPreviewGroupedItems;
    QAction*                actionShowNumbersOnItems;

    bool                    lazyReclusteringRequested;

    GeoDragDropHandler*     dragDropHandler;

    QMenu*                  sortMenu;
    QAction*                actionIncreaseThumbnailSize;
    QAction*                actionDecreaseThumbnailSize;
    QWidget*                hBoxForAdditionalControlWidgetItems;

    QActionGroup*           mouseModeActionGroup;
    QAction*                actionRemoveCurrentRegionSelection;
    QAction*                actionSetRegionSelectionMode;
    QAction*                actionSetPanMode;
    QAction*                actionSetZoomIntoGroupMode;
    QAction*                actionSetRegionSelectionFromIconMode;
    QAction*                actionSetFilterMode;
    QAction*                actionRemoveFilter;
    QAction*                actionSetSelectThumbnailMode;
    QAction*                actionLoadTracksFromAlbums;
    QToolButton*            setPanModeButton;
    QToolButton*            setSelectionModeButton;
    QToolButton*            removeCurrentSelectionButton;
    QToolButton*            setZoomModeButton;
    QToolButton*            setRegionSelectionFromIconModeButton;
    QToolButton*            setFilterModeButton;
    QToolButton*            removeFilterModeButton;
    QToolButton*            setSelectThumbnailMode;
    QToolButton*            loadTracksFromAlbums;

    QTimer*                 thumbnailTimer;
    int                     thumbnailTimerCount;
    bool                    thumbnailsHaveBeenLoaded;

    GeoExtraActions         availableExtraActions;
    GeoExtraActions         visibleExtraActions;
    QAction*                actionStickyMode;
    QToolButton*            buttonStickyMode;

    /// NOTE: to be sorted later
    PlaceholderWidget*      placeholderWidget;
};

} // namespace Digikam
