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

#include "mapwidget_p.h"

namespace Digikam
{

MapWidget::Private::Private()
    : loadedBackends                          (),
    currentBackend                          (nullptr),
    currentBackendName                      (),
    stackedLayout                           (nullptr),
    cacheCenterCoordinate                   (52.0, 6.0),
    cacheZoom                               (QLatin1String("marble:900")),
    configurationMenu                       (nullptr),
    actionGroupBackendSelection             (nullptr),
    actionZoomIn                            (nullptr),
    actionZoomOut                           (nullptr),
    actionShowThumbnails                    (nullptr),
    mouseModesHolder                        (nullptr),
    controlWidget                           (nullptr),
    actionPreviewSingleItems                (nullptr),
    actionPreviewGroupedItems               (nullptr),
    actionShowNumbersOnItems                (nullptr),
    lazyReclusteringRequested               (false),
    dragDropHandler                         (nullptr),
    sortMenu                                (nullptr),
    actionIncreaseThumbnailSize             (nullptr),
    actionDecreaseThumbnailSize             (nullptr),
    hBoxForAdditionalControlWidgetItems     (nullptr),
    mouseModeActionGroup                    (nullptr),
    actionRemoveCurrentRegionSelection      (nullptr),
    actionSetRegionSelectionMode            (nullptr),
    actionSetPanMode                        (nullptr),
    actionSetZoomIntoGroupMode              (nullptr),
    actionSetRegionSelectionFromIconMode    (nullptr),
    actionSetFilterMode                     (nullptr),
    actionRemoveFilter                      (nullptr),
    actionSetSelectThumbnailMode            (nullptr),
    actionLoadTracksFromAlbums              (nullptr),
    setPanModeButton                        (nullptr),
    setSelectionModeButton                  (nullptr),
    removeCurrentSelectionButton            (nullptr),
    setZoomModeButton                       (nullptr),
    setRegionSelectionFromIconModeButton    (nullptr),
    setFilterModeButton                     (nullptr),
    removeFilterModeButton                  (nullptr),
    setSelectThumbnailMode                  (nullptr),
    loadTracksFromAlbums                    (nullptr),
    thumbnailTimer                          (nullptr),
    thumbnailTimerCount                     (0),
    thumbnailsHaveBeenLoaded                (false),
    availableExtraActions                   (),
    visibleExtraActions                     (),
    actionStickyMode                        (nullptr),
    buttonStickyMode                        (nullptr),
    placeholderWidget                       (nullptr)
{
}

} // namespace Digikam
