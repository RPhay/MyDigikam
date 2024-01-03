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

MapWidget::MapWidget(QWidget* const parent)
    : QWidget(parent),
      s      (new GeoIfaceSharedData),
      d      (new Private)
{
    createActions();

    s->worldMapWidget    = this;
    s->tileGrouper       = new TileGrouper(s, this);

    d->stackedLayout     = new QStackedLayout(this);
    setLayout(d->stackedLayout);

    d->placeholderWidget = new PlaceholderWidget();
    d->stackedLayout->addWidget(d->placeholderWidget);

    d->loadedBackends.append(new BackendGoogleMaps(s, this));

#ifdef HAVE_GEOLOCATION

    d->loadedBackends.append(new BackendMarble(s, this));

#endif

/*
    d->loadedBackends.append(new BackendOSM(s, this));
*/
    createActionsForBackendSelection();

    setAcceptDrops(true);
}

void MapWidget::createActions()
{
    d->actionZoomIn  = new QAction(this);
    d->actionZoomIn->setIcon(QIcon::fromTheme(QLatin1String("zoom-in")));
    d->actionZoomIn->setToolTip(i18n("Zoom in"));

    connect(d->actionZoomIn, &QAction::triggered,
            this, &MapWidget::slotZoomIn);

    d->actionZoomOut = new QAction(this);
    d->actionZoomOut->setIcon(QIcon::fromTheme(QLatin1String("zoom-out")));
    d->actionZoomOut->setToolTip(i18n("Zoom out"));

    connect(d->actionZoomOut, &QAction::triggered,
            this, &MapWidget::slotZoomOut);

    d->actionShowThumbnails = new QAction(this);
    d->actionShowThumbnails->setToolTip(i18n("Switch between markers and thumbnails."));
    d->actionShowThumbnails->setCheckable(true);
    d->actionShowThumbnails->setChecked(true);

    connect(d->actionShowThumbnails, &QAction::triggered,
            this, &MapWidget::slotShowThumbnailsChanged);

    // create backend selection entries:

    d->actionGroupBackendSelection = new QActionGroup(this);
    d->actionGroupBackendSelection->setExclusive(true);

    connect(d->actionGroupBackendSelection, &QActionGroup::triggered,
            this, &MapWidget::slotChangeBackend);

    createActionsForBackendSelection();

    d->configurationMenu         = new QMenu(this);
    d->actionPreviewSingleItems  = new QAction(i18n("Preview single items"), this);
    d->actionPreviewSingleItems->setCheckable(true);
    d->actionPreviewSingleItems->setChecked(true);
    d->actionPreviewGroupedItems = new QAction(i18n("Preview grouped items"), this);
    d->actionPreviewGroupedItems->setCheckable(true);
    d->actionPreviewGroupedItems->setChecked(true);
    d->actionShowNumbersOnItems  = new QAction(i18n("Show numbers"), this);
    d->actionShowNumbersOnItems->setCheckable(true);
    d->actionShowNumbersOnItems->setChecked(true);

    d->actionIncreaseThumbnailSize = new QAction(this);
    d->actionIncreaseThumbnailSize->setIcon(QIcon::fromTheme(QLatin1String("list-add")));
    d->actionIncreaseThumbnailSize->setToolTip(i18n("Increase the thumbnail size on the map"));
    d->actionDecreaseThumbnailSize = new QAction(this);
    d->actionDecreaseThumbnailSize->setIcon(QIcon::fromTheme(QLatin1String("list-remove")));
    d->actionDecreaseThumbnailSize->setToolTip(i18n("Decrease the thumbnail size on the map"));

    d->actionRemoveCurrentRegionSelection = new QAction(this);
/*
    d->actionRemoveCurrentRegionSelection->setEnabled(false);
*/
    d->actionRemoveCurrentRegionSelection->setIcon(QIcon::fromTheme(QLatin1String("edit-clear")));
    d->actionRemoveCurrentRegionSelection->setToolTip(i18n("Remove the current region selection"));

    d->mouseModeActionGroup = new QActionGroup(this);
    d->mouseModeActionGroup->setExclusive(true);

    d->actionSetRegionSelectionMode = new QAction(d->mouseModeActionGroup);
    d->actionSetRegionSelectionMode->setCheckable(true);
    d->actionSetRegionSelectionMode->setIcon(QIcon::fromTheme(QLatin1String("select-rectangular")));
    d->actionSetRegionSelectionMode->setToolTip(i18n("Select images by drawing a rectangle"));
    d->actionSetRegionSelectionMode->setData(QVariant::fromValue<Digikam::GeoMouseModes>(MouseModeRegionSelection));

    d->actionSetPanMode = new QAction(d->mouseModeActionGroup);
    d->actionSetPanMode->setCheckable(true);
    d->actionSetPanMode->setToolTip(i18n("Pan mode"));
    d->actionSetPanMode->setIcon(QIcon::fromTheme(QLatin1String("transform-move")));
    d->actionSetPanMode->setChecked(true);
    d->actionSetPanMode->setData(QVariant::fromValue<Digikam::GeoMouseModes>(MouseModePan));

    d->actionSetZoomIntoGroupMode = new QAction(d->mouseModeActionGroup);
    d->actionSetZoomIntoGroupMode->setCheckable(true);
    d->actionSetZoomIntoGroupMode->setToolTip(i18n("Zoom into a group"));
    d->actionSetZoomIntoGroupMode->setIcon(QIcon::fromTheme(QLatin1String("zoom-fit-best")));
    d->actionSetZoomIntoGroupMode->setData(QVariant::fromValue<Digikam::GeoMouseModes>(MouseModeZoomIntoGroup));

    d->actionSetRegionSelectionFromIconMode = new QAction(d->mouseModeActionGroup);
    d->actionSetRegionSelectionFromIconMode->setCheckable(true);
    d->actionSetRegionSelectionFromIconMode->setToolTip(i18n("Create a region selection from a thumbnail"));
    d->actionSetRegionSelectionFromIconMode->setIcon(QIcon::fromTheme(QLatin1String("edit-node")));
    d->actionSetRegionSelectionFromIconMode->setData(QVariant::fromValue<Digikam::GeoMouseModes>(MouseModeRegionSelectionFromIcon));

    d->actionSetFilterMode = new QAction(d->mouseModeActionGroup);
    d->actionSetFilterMode->setCheckable(true);
    d->actionSetFilterMode->setToolTip(i18n("Filter images"));
    d->actionSetFilterMode->setIcon(QIcon::fromTheme(QLatin1String("view-filter")));
    d->actionSetFilterMode->setData(QVariant::fromValue<Digikam::GeoMouseModes>(MouseModeFilter));

    d->actionRemoveFilter = new QAction(this);
    d->actionRemoveFilter->setToolTip(i18n("Remove the current filter"));
    d->actionRemoveFilter->setIcon(QIcon::fromTheme(QLatin1String("window-close")));

    d->actionSetSelectThumbnailMode = new QAction(d->mouseModeActionGroup);
    d->actionSetSelectThumbnailMode->setCheckable(true);
    d->actionSetSelectThumbnailMode->setToolTip(i18n("Select images"));
    d->actionSetSelectThumbnailMode->setIcon(QIcon::fromTheme(QLatin1String("edit-select")));
    d->actionSetSelectThumbnailMode->setData(QVariant::fromValue<Digikam::GeoMouseModes>(MouseModeSelectThumbnail));

    d->actionStickyMode = new QAction(this);
    d->actionStickyMode->setCheckable(true);
    d->actionStickyMode->setToolTip(i18n("Lock the map position"));

    d->actionLoadTracksFromAlbums = new QAction(this);
    d->actionLoadTracksFromAlbums->setToolTip(i18n("Load tracks from albums"));
    d->actionLoadTracksFromAlbums->setIcon(QIcon::fromTheme(QLatin1String("draw-polyline")));

    connect(d->actionStickyMode, &QAction::triggered,
            this, &MapWidget::slotStickyModeChanged);

    connect(d->actionIncreaseThumbnailSize, &QAction::triggered,
            this, &MapWidget::slotIncreaseThumbnailSize);

    connect(d->actionDecreaseThumbnailSize, &QAction::triggered,
            this, &MapWidget::slotDecreaseThumbnailSize);

    connect(d->actionPreviewSingleItems, &QAction::changed,
            this, &MapWidget::slotItemDisplaySettingsChanged);

    connect(d->actionPreviewGroupedItems, &QAction::changed,
            this, &MapWidget::slotItemDisplaySettingsChanged);

    connect(d->actionShowNumbersOnItems, &QAction::changed,
            this, &MapWidget::slotItemDisplaySettingsChanged);

    connect(d->mouseModeActionGroup, &QActionGroup::triggered,
            this, &MapWidget::slotMouseModeChanged);

    connect(d->actionRemoveFilter, &QAction::triggered,
            this, &MapWidget::signalRemoveCurrentFilter);

    connect(d->actionRemoveCurrentRegionSelection, &QAction::triggered,
            this, &MapWidget::slotRemoveCurrentRegionSelection);

    connect(d->actionLoadTracksFromAlbums, &QAction::triggered,
            this, &MapWidget::signalLoadTracksFromAlbums);
}

void MapWidget::createActionsForBackendSelection()
{
    // delete the existing actions:

    qDeleteAll(d->actionGroupBackendSelection->actions());

    // create actions for all backends:

    for (int i = 0 ; i < d->loadedBackends.size() ; ++i)
    {
        const QString backendName    = d->loadedBackends.at(i)->backendName();
        QAction* const backendAction = new QAction(d->actionGroupBackendSelection);
        backendAction->setData(backendName);
        backendAction->setText(d->loadedBackends.at(i)->backendHumanName());
        backendAction->setCheckable(true);
    }
}

MapWidget::~MapWidget()
{
    // release all widgets:

    for (int i = 0 ; i < d->stackedLayout->count() ; ++i)
    {
        d->stackedLayout->removeWidget(d->stackedLayout->widget(i));
    }

    GeolocationSettings::instance()->unregisterWidget(this);

    qDeleteAll(d->loadedBackends);
    d->currentBackend = nullptr;
    d->loadedBackends.clear();

    delete d;

    /// @todo delete s, but make sure it is not accessed by any other objects any more!
}

void MapWidget::stopThumbnailTimer()
{
    d->currentBackend->updateMarkers();
    d->thumbnailTimerCount++;

    if (d->thumbnailTimerCount == 10)
    {
        d->thumbnailTimer->stop();
        d->thumbnailsHaveBeenLoaded = true;
    }
}

void MapWidget::rebuildConfigurationMenu()
{
    d->configurationMenu->clear();
    const QList<QAction*> backendSelectionActions = d->actionGroupBackendSelection->actions();

    for (int i = 0 ; i < backendSelectionActions.count() ; ++i)
    {
        QAction* const backendAction = backendSelectionActions.at(i);

        if (backendAction->data().toString() == d->currentBackendName)
        {
            backendAction->setChecked(true);
        }

        d->configurationMenu->addAction(backendAction);
    }

    if (currentBackendReady())
    {
        d->currentBackend->addActionsToConfigurationMenu(d->configurationMenu);
    }

    if (s->showThumbnails)
    {
        d->configurationMenu->addSeparator();

        if (d->sortMenu)
        {
            d->configurationMenu->addMenu(d->sortMenu);
        }

        d->configurationMenu->addAction(d->actionPreviewSingleItems);
        d->configurationMenu->addAction(d->actionPreviewGroupedItems);
        d->configurationMenu->addAction(d->actionShowNumbersOnItems);
    }

    slotUpdateActionsEnabled();
}

QAction* MapWidget::getControlAction(const QString& actionName)
{
    if      (actionName == QLatin1String("zoomin"))
    {
        return d->actionZoomIn;
    }
    else if (actionName == QLatin1String("zoomout"))
    {
        return d->actionZoomOut;
    }
    else if (actionName == QLatin1String("mousemode-regionselectionmode"))
    {
        return d->actionSetRegionSelectionMode;
    }
    else if (actionName == QLatin1String("mousemode-removecurrentregionselection"))
    {
        return d->actionRemoveCurrentRegionSelection;
    }
    else if (actionName == QLatin1String("mousemode-regionselectionfromiconmode"))
    {
        return d->actionSetRegionSelectionFromIconMode;
    }
    else if (actionName == QLatin1String("mousemode-removefilter"))
    {
        return d->actionRemoveFilter;
    }

    return nullptr;
}

/**
 * @brief Returns the control widget.
 */
QWidget* MapWidget::getControlWidget()
{
    if (!d->controlWidget)
    {
        d->controlWidget                           = new QWidget(this);
        QHBoxLayout* const controlWidgetHBoxLayout = new QHBoxLayout(d->controlWidget);
        controlWidgetHBoxLayout->setContentsMargins(QMargins());

        QPushButton* const configurationButton = new QPushButton(d->controlWidget);
        configurationButton->setIcon(QIcon::fromTheme(QLatin1String("globe")));
        controlWidgetHBoxLayout->addWidget(configurationButton);
        configurationButton->setToolTip(i18n("Map settings"));
        configurationButton->setMenu(d->configurationMenu);

        QToolButton* const zoomInButton = new QToolButton(d->controlWidget);
        controlWidgetHBoxLayout->addWidget(zoomInButton);
        zoomInButton->setDefaultAction(d->actionZoomIn);

        QToolButton* const zoomOutButton = new QToolButton(d->controlWidget);
        controlWidgetHBoxLayout->addWidget(zoomOutButton);
        zoomOutButton->setDefaultAction(d->actionZoomOut);

        QToolButton* const showThumbnailsButton = new QToolButton(d->controlWidget);
        controlWidgetHBoxLayout->addWidget(showThumbnailsButton);
        showThumbnailsButton->setDefaultAction(d->actionShowThumbnails);

        QFrame* const vline1 = new QFrame(d->controlWidget);
        vline1->setLineWidth(1);
        vline1->setMidLineWidth(0);
        vline1->setFrameShape(QFrame::VLine);
        vline1->setFrameShadow(QFrame::Sunken);
        vline1->setMinimumSize(2, 0);
        vline1->updateGeometry();
        controlWidgetHBoxLayout->addWidget(vline1);

        QToolButton* const increaseThumbnailSizeButton = new QToolButton(d->controlWidget);
        controlWidgetHBoxLayout->addWidget(increaseThumbnailSizeButton);
        increaseThumbnailSizeButton->setDefaultAction(d->actionIncreaseThumbnailSize);

        QToolButton* const decreaseThumbnailSizeButton = new QToolButton(d->controlWidget);
        controlWidgetHBoxLayout->addWidget(decreaseThumbnailSizeButton);
        decreaseThumbnailSizeButton->setDefaultAction(d->actionDecreaseThumbnailSize);

        /* --- --- --- */

        d->mouseModesHolder                           = new QWidget(d->controlWidget);
        QHBoxLayout* const mouseModesHolderHBoxLayout = new QHBoxLayout(d->mouseModesHolder);
        mouseModesHolderHBoxLayout->setContentsMargins(QMargins());
        controlWidgetHBoxLayout->addWidget(d->mouseModesHolder);

        QFrame* const vline2 = new QFrame(d->mouseModesHolder);
        vline2->setLineWidth(1);
        vline2->setMidLineWidth(0);
        vline2->setFrameShape(QFrame::VLine);
        vline2->setFrameShadow(QFrame::Sunken);
        vline2->setMinimumSize(2, 0);
        vline2->updateGeometry();
        mouseModesHolderHBoxLayout->addWidget(vline2);

        d->setPanModeButton = new QToolButton(d->mouseModesHolder);
        mouseModesHolderHBoxLayout->addWidget(d->setPanModeButton);
        d->setPanModeButton->setDefaultAction(d->actionSetPanMode);

        d->setSelectionModeButton = new QToolButton(d->mouseModesHolder);
        mouseModesHolderHBoxLayout->addWidget(d->setSelectionModeButton);
        d->setSelectionModeButton->setDefaultAction(d->actionSetRegionSelectionMode);

        d->setRegionSelectionFromIconModeButton = new QToolButton(d->mouseModesHolder);
        mouseModesHolderHBoxLayout->addWidget(d->setRegionSelectionFromIconModeButton);
        d->setRegionSelectionFromIconModeButton->setDefaultAction(d->actionSetRegionSelectionFromIconMode);

        d->removeCurrentSelectionButton = new QToolButton(d->mouseModesHolder);
        mouseModesHolderHBoxLayout->addWidget(d->removeCurrentSelectionButton);
        d->removeCurrentSelectionButton->setDefaultAction(d->actionRemoveCurrentRegionSelection);

        d->setZoomModeButton = new QToolButton(d->mouseModesHolder);
        mouseModesHolderHBoxLayout->addWidget(d->setZoomModeButton);
        d->setZoomModeButton->setDefaultAction(d->actionSetZoomIntoGroupMode);

        d->setFilterModeButton = new QToolButton(d->mouseModesHolder);
        mouseModesHolderHBoxLayout->addWidget(d->setFilterModeButton);
        d->setFilterModeButton->setDefaultAction(d->actionSetFilterMode);

        d->removeFilterModeButton = new QToolButton(d->mouseModesHolder);
        mouseModesHolderHBoxLayout->addWidget(d->removeFilterModeButton);
        d->removeFilterModeButton->setDefaultAction(d->actionRemoveFilter);

        d->setSelectThumbnailMode = new QToolButton(d->mouseModesHolder);
        mouseModesHolderHBoxLayout->addWidget(d->setSelectThumbnailMode);
        d->setSelectThumbnailMode->setDefaultAction(d->actionSetSelectThumbnailMode);

        d->buttonStickyMode = new QToolButton(d->controlWidget);
        controlWidgetHBoxLayout->addWidget(d->buttonStickyMode);
        d->buttonStickyMode->setDefaultAction(d->actionStickyMode);

        d->hBoxForAdditionalControlWidgetItems = new QWidget(d->controlWidget);
        QHBoxLayout *hBoxForAdditionalControlWidgetItemsHBoxLayout = new QHBoxLayout(d->hBoxForAdditionalControlWidgetItems);
        hBoxForAdditionalControlWidgetItemsHBoxLayout->setContentsMargins(QMargins());
        controlWidgetHBoxLayout->addWidget(d->hBoxForAdditionalControlWidgetItems);

        d->loadTracksFromAlbums = new QToolButton(d->controlWidget);
        hBoxForAdditionalControlWidgetItemsHBoxLayout->addWidget(d->loadTracksFromAlbums);
        d->loadTracksFromAlbums->setDefaultAction(d->actionLoadTracksFromAlbums);

        setVisibleMouseModes(s->visibleMouseModes);
        setVisibleExtraActions(d->visibleExtraActions);

        // add stretch after the controls:

        QHBoxLayout* const hBoxLayout = reinterpret_cast<QHBoxLayout*>(d->controlWidget->layout());

        if (hBoxLayout)
        {
            hBoxLayout->addStretch();
        }
    }

    // make sure the menu exists, even if no backend has been set:

    rebuildConfigurationMenu();

    return d->controlWidget;
}

void MapWidget::slotZoomIn()
{
    if (!currentBackendReady())
    {
        return;
    }

    d->currentBackend->zoomIn();
}

void MapWidget::slotZoomOut()
{
    if (!currentBackendReady())
    {
        return;
    }

    d->currentBackend->zoomOut();
}

void MapWidget::slotUpdateActionsEnabled()
{
    if (!s->activeState)
    {
        // this widget is not active, no need to update the action availability

        return;
    }

    d->actionDecreaseThumbnailSize->setEnabled((s->showThumbnails)&&(s->thumbnailSize>GeoIfaceMinThumbnailSize));

    /// @todo Define an upper limit for the thumbnail size!

    d->actionIncreaseThumbnailSize->setEnabled(s->showThumbnails);

    d->actionSetRegionSelectionMode->setEnabled(s->availableMouseModes.testFlag(MouseModeRegionSelection));

    d->actionSetPanMode->setEnabled(s->availableMouseModes.testFlag(MouseModePan));
    d->actionSetZoomIntoGroupMode->setEnabled(s->availableMouseModes.testFlag(MouseModeZoomIntoGroup));
    d->actionSetRegionSelectionFromIconMode->setEnabled(s->availableMouseModes.testFlag(MouseModeRegionSelectionFromIcon));
    d->actionSetFilterMode->setEnabled(s->availableMouseModes.testFlag(MouseModeFilter));
    d->actionSetSelectThumbnailMode->setEnabled(s->availableMouseModes.testFlag(MouseModeSelectThumbnail));

    // the 'Remove X' actions are only available if the corresponding X is actually there:

    bool clearRegionSelectionAvailable = s->availableMouseModes.testFlag(MouseModeRegionSelection);

    if (clearRegionSelectionAvailable && s->markerModel)
    {
        clearRegionSelectionAvailable = s->markerModel->getGlobalGroupState() & RegionSelectedMask;
    }

    d->actionRemoveCurrentRegionSelection->setEnabled(clearRegionSelectionAvailable);
    bool clearFilterAvailable = s->availableMouseModes.testFlag(MouseModeRegionSelectionFromIcon);

    if (clearFilterAvailable && s->markerModel)
    {
        clearFilterAvailable = s->markerModel->getGlobalGroupState() & FilteredPositiveMask;
    }

    d->actionRemoveFilter->setEnabled(clearFilterAvailable);

    d->actionStickyMode->setEnabled(d->availableExtraActions.testFlag(ExtraActionSticky));

    /// @todo Only set the icons if they have to be changed!

    d->actionStickyMode->setIcon(QIcon::fromTheme(QLatin1String(d->actionStickyMode->isChecked()
                                                                ? "document-encrypted"
                                                                : "document-decrypt")));
    d->actionShowThumbnails->setIcon(d->actionShowThumbnails->isChecked()
                                     ? QIcon::fromTheme(QLatin1String("folder-pictures"))
                                     : GeoIfaceGlobalObject::instance()->getMarkerPixmap(QLatin1String("marker-icon-16x16")));

    // make sure the action for the current mouse mode is checked

    const QList<QAction*> mouseModeActions = d->mouseModeActionGroup->actions();

    Q_FOREACH (QAction* const action, mouseModeActions)
    {
        if (action->data().value<GeoMouseModes>() == s->currentMouseMode)
        {   // cppcheck-suppress useStlAlgorithm
            action->setChecked(true);
            break;
        }
    }
}

void MapWidget::getColorInfos(const int clusterIndex,
                              QColor* fillColor,
                              QColor* strokeColor,
                              Qt::PenStyle* strokeStyle,
                              QString* labelText,
                              QColor* labelColor,
                              const GeoGroupState* const overrideSelection,
                              const int* const overrideCount) const
{
    /// @todo Call the new getColorInfos function!

    const GeoIfaceCluster& cluster = s->clusterList.at(clusterIndex);

    /// @todo Check that this number is already valid!

    const int nMarkers             = overrideCount ? *overrideCount : cluster.markerCount;

    getColorInfos(overrideSelection ? *overrideSelection : cluster.groupState,
                  nMarkers,
                  fillColor, strokeColor, strokeStyle, labelText, labelColor);
}

void MapWidget::getColorInfos(const GeoGroupState groupState,
                              const int nMarkers,
                              QColor* fillColor, QColor* strokeColor,
                              Qt::PenStyle* strokeStyle, QString* labelText,
                              QColor* labelColor) const
{
    if      (nMarkers < 1000)
    {
        *labelText = QString::number(nMarkers);
    }
    else if ((nMarkers >= 1000) && (nMarkers <= 1950))      // cppcheck-suppress knownConditionTrueFalse
    {
        *labelText = QString::fromLatin1("%L1k").arg(qreal(nMarkers)/1000.0, 0, 'f', 1);
    }
    else if ((nMarkers >= 1951) && (nMarkers < 19500))      // cppcheck-suppress knownConditionTrueFalse
    {
        *labelText = QString::fromLatin1("%L1k").arg(qreal(nMarkers)/1000.0, 0, 'f', 0);
    }
    else
    {
        // convert to "1E5" notation for numbers >=20k:

        qreal exponent           = floor(log((qreal)nMarkers)/log((qreal)10));
        qreal nMarkersFirstDigit = round(qreal(nMarkers)/pow(10, exponent));

        if (nMarkersFirstDigit >= 10)
        {
            nMarkersFirstDigit=round(nMarkersFirstDigit/10.0);
            exponent++;
        }

        *labelText = QString::fromLatin1("%1E%2").arg(int(nMarkersFirstDigit)).arg(int(exponent));
    }

    *labelColor  = QColor(Qt::black);
    *strokeStyle = Qt::NoPen;

    /// @todo On my system, digikam uses QColor(67, 172, 232) as the selection color. Or should we just use blue?

    switch (groupState & SelectedMask)
    {
        case SelectedNone:
        {
            *strokeStyle = Qt::SolidLine;
            *strokeColor = QColor(Qt::black);
            break;
        }

        case SelectedSome:
        {
            *strokeStyle = Qt::DotLine;
            *strokeColor = QColor(Qt::blue); // 67, 172, 232);
            break;
        }

        case SelectedAll:
        {
            *strokeStyle = Qt::SolidLine;
            *strokeColor = QColor(Qt::blue); // 67, 172, 232);
            break;
        }
    }

    /**
     * @todo These are the fill colors for the circles, for cases in which only some or all of the images
     * are positively filtered. Filtering is implemented in GeoIface, but the code here has not been adapted yet.
     */
    QColor fillAll, fillSome, fillNone;

    if      (nMarkers >= 100)
    {
        fillAll  = QColor(255, 0,   0);
        fillSome = QColor(255, 188, 125);
        fillNone = QColor(255, 185, 185);
    }
    else if (nMarkers >= 50)
    {
        fillAll  = QColor(255, 127, 0);
        fillSome = QColor(255, 190, 125);
        fillNone = QColor(255, 220, 185);
    }
    else if (nMarkers >= 10)
    {
        fillAll  = QColor(255, 255, 0);
        fillSome = QColor(255, 255, 105);
        fillNone = QColor(255, 255, 185);
    }
    else if (nMarkers >= 2)
    {
        fillAll  = QColor(0,   255, 0);
        fillSome = QColor(125, 255, 125);
        fillNone = QColor(185, 255, 255);
    }
    else
    {
        fillAll  = QColor(0,   255, 255);
        fillSome = QColor(125, 255, 255);
        fillNone = QColor(185, 255, 255);
    }

    *fillColor = fillAll;
/*
    switch (groupState)
    {
        case PartialAll:
        {
            *fillColor = fillAll;
            break;
        }

        case PartialSome:
        {
            *fillColor = fillSome;
            break;
        }

        case PartialNone:
        {
            if (haveAnySolo)
            {
                *fillColor = fillNone;
            }
            else
            {
                *fillColor = fillAll;
            }

            break;
        }
    }
*/
}

QString MapWidget::convertZoomToBackendZoom(const QString& someZoom,
                                            const QString& targetBackend) const
{
    const QStringList zoomParts = someZoom.split(QLatin1Char(':'));
    GEOIFACE_ASSERT(zoomParts.count() == 2);
    const QString sourceBackend = zoomParts.first();

    if (sourceBackend == targetBackend)
    {
        return someZoom;
    }

    const int sourceZoom = zoomParts.last().toInt();
    int targetZoom       = -1;

#ifdef HAVE_GEOLOCATION

    // all of these values were found experimentally!

    if (targetBackend == QLatin1String("marble" ))
    {
        if      (sourceZoom == 0) { targetZoom =  900; }
        else if (sourceZoom == 1) { targetZoom =  970; }
        else if (sourceZoom == 2) { targetZoom = 1108; }
        else if (sourceZoom == 3) { targetZoom = 1250; }
        else if (sourceZoom == 4) { targetZoom = 1384; }
        else if (sourceZoom == 5) { targetZoom = 1520; }
        else if (sourceZoom == 6) { targetZoom = 1665; }
        else if (sourceZoom == 7) { targetZoom = 1800; }
        else if (sourceZoom == 8) { targetZoom = 1940; }
        else if (sourceZoom == 9) { targetZoom = 2070; }
        else if (sourceZoom ==10) { targetZoom = 2220; }
        else if (sourceZoom ==11) { targetZoom = 2357; }
        else if (sourceZoom ==12) { targetZoom = 2510; }
        else if (sourceZoom ==13) { targetZoom = 2635; }
        else if (sourceZoom ==14) { targetZoom = 2775; }
        else if (sourceZoom ==15) { targetZoom = 2900; }
        else if (sourceZoom ==16) { targetZoom = 3051; }
        else if (sourceZoom ==17) { targetZoom = 3180; }
        else if (sourceZoom ==18) { targetZoom = 3295; }
        else if (sourceZoom ==19) { targetZoom = 3450; }
        else                      { targetZoom = 3500; } /// @todo Find values for level 20 and up
    }

#endif

    if (targetBackend == QLatin1String("googlemaps" ))
    {
        if      (sourceZoom <= 900) { targetZoom =  0; }
        else if (sourceZoom <= 970) { targetZoom =  1; }
        else if (sourceZoom <=1108) { targetZoom =  2; }
        else if (sourceZoom <=1250) { targetZoom =  3; }
        else if (sourceZoom <=1384) { targetZoom =  4; }
        else if (sourceZoom <=1520) { targetZoom =  5; }
        else if (sourceZoom <=1665) { targetZoom =  6; }
        else if (sourceZoom <=1800) { targetZoom =  7; }
        else if (sourceZoom <=1940) { targetZoom =  8; }
        else if (sourceZoom <=2070) { targetZoom =  9; }
        else if (sourceZoom <=2220) { targetZoom = 10; }
        else if (sourceZoom <=2357) { targetZoom = 11; }
        else if (sourceZoom <=2510) { targetZoom = 12; }
        else if (sourceZoom <=2635) { targetZoom = 13; }
        else if (sourceZoom <=2775) { targetZoom = 14; }
        else if (sourceZoom <=2900) { targetZoom = 15; }
        else if (sourceZoom <=3051) { targetZoom = 16; }
        else if (sourceZoom <=3180) { targetZoom = 17; }
        else if (sourceZoom <=3295) { targetZoom = 18; }
        else if (sourceZoom <=3450) { targetZoom = 19; }
        else                        { targetZoom = 20; } /// @todo Find values for level 20 and up
    }

    GEOIFACE_ASSERT(targetZoom >= 0);

    return QString::fromLatin1("%1:%2").arg(targetBackend).arg(targetZoom);
}

void MapWidget::addUngroupedModel(GeoModelHelper* const modelHelper)
{
    s->ungroupedModels << modelHelper;

    /// @todo monitor all model signals!

    connect(modelHelper->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(slotUngroupedModelChanged()));

    connect(modelHelper->model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(slotUngroupedModelChanged()));

    connect(modelHelper->model(), SIGNAL(modelReset()),
            this, SLOT(slotUngroupedModelChanged()));

    connect(modelHelper, SIGNAL(signalVisibilityChanged()),
            this, SLOT(slotUngroupedModelChanged()));

    if (modelHelper->selectionModel())
    {
        connect(modelHelper->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                this, SLOT(slotUngroupedModelChanged()));
    }

    Q_EMIT signalUngroupedModelChanged(s->ungroupedModels.count() - 1);
}

void MapWidget::removeUngroupedModel(GeoModelHelper* const modelHelper)
{
    if (!modelHelper)
    {
        return;
    }

    const int modelIndex = s->ungroupedModels.indexOf(modelHelper);

    if (modelIndex < 0)
    {
        return;
    }

    /// @todo monitor all model signals!

    disconnect(modelHelper->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
               this, SLOT(slotUngroupedModelChanged()));

    disconnect(modelHelper->model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
               this, SLOT(slotUngroupedModelChanged()));

    disconnect(modelHelper->model(), SIGNAL(modelReset()),
               this, SLOT(slotUngroupedModelChanged()));

    disconnect(modelHelper, SIGNAL(signalVisibilityChanged()),
               this, SLOT(slotUngroupedModelChanged()));

    if (modelHelper->selectionModel())
    {
        disconnect(modelHelper->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                   this, SLOT(slotUngroupedModelChanged()));
    }

    s->ungroupedModels.removeAt(modelIndex);

    // the indices changed, therefore send out notifications
    // sending out a signal with i=s->ungroupedModel.count()
    // will cause the backends to see that the last model is missing

    for (int i = modelIndex ; i <= s->ungroupedModels.count() ; ++i)
    {
        Q_EMIT signalUngroupedModelChanged(i);
    }
}

void MapWidget::setGroupedModel(AbstractMarkerTiler* const markerModel)
{
    s->markerModel = markerModel;

    if (s->markerModel)
    {
        s->markerModel->setActive(s->activeState);

        /// @todo this needs some buffering for the google maps backend

        connect(s->markerModel, SIGNAL(signalTilesOrSelectionChanged()),
                this, SLOT(slotRequestLazyReclustering()));

        if (d->currentBackend)
        {
            connect(s->markerModel, SIGNAL(signalThumbnailAvailableForIndex(QVariant,QPixmap)),
                    d->currentBackend, SLOT(slotThumbnailAvailableForIndex(QVariant,QPixmap)));
        }
    }

    slotRequestLazyReclustering();
}

void MapWidget::setShowThumbnails(const bool state)
{
    s->showThumbnails = state;

    rebuildConfigurationMenu();
    slotUpdateActionsEnabled();
    slotRequestLazyReclustering();
}

void MapWidget::slotShowThumbnailsChanged()
{
    setShowThumbnails(d->actionShowThumbnails->isChecked());
}

void MapWidget::dragEnterEvent(QDragEnterEvent* event)
{
    /// @todo ignore drops if no marker tiler or model can accept them

    if (!d->dragDropHandler)
    {
        event->ignore();
        return;
    }

    if (d->dragDropHandler->accepts(event) == Qt::IgnoreAction)
    {
        event->ignore();
        return;
    }

    /// @todo need data about the dragged object: #markers, selected, icon, ...

    event->accept();
}

void MapWidget::dragMoveEvent(QDragMoveEvent* event)
{
    if (!d->dragDropHandler)
    {
        event->ignore();
        return;
    }

    event->accept();
}

void MapWidget::dropEvent(QDropEvent* event)
{
    // remove the drag marker:
/*
    d->currentBackend->updateDragDropMarker(QPoint(), 0);
*/
    if (!d->dragDropHandler)
    {
        event->ignore();
        return;
    }

    GeoCoordinates dropCoordinates;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

    if (!d->currentBackend->geoCoordinates(event->position().toPoint(), &dropCoordinates))

#else

    if (!d->currentBackend->geoCoordinates(event->pos(), &dropCoordinates))

#endif

    {
        return;
    }

    // the drag and drop handler handled the drop if it returned true here

    if (d->dragDropHandler->dropEvent(event, dropCoordinates))
    {
        event->acceptProposedAction();
    }
}

void MapWidget::dragLeaveEvent(QDragLeaveEvent* event)
{
    Q_UNUSED(event);

    // remove the marker:
/*
    d->currentBackend->updateDragDropMarker(QPoint(), 0);
*/
}

void MapWidget::setDragDropHandler(GeoDragDropHandler* const dragDropHandler)
{
    d->dragDropHandler = dragDropHandler;
}

void MapWidget::setSortOptionsMenu(QMenu* const sortMenu)
{
    d->sortMenu = sortMenu;

    rebuildConfigurationMenu();
}

void MapWidget::setSortKey(const int sortKey)
{
    s->sortKey = sortKey;

    // this is probably faster than writing a function that changes all the clusters icons...
    /// @todo We just need to update the display, no need to recluster?

    slotRequestLazyReclustering();
}

void MapWidget::setThumnailSize(const int newThumbnailSize)
{
    s->thumbnailSize = qMax(GeoIfaceMinThumbnailSize, newThumbnailSize);

    // make sure the grouping radius is larger than the thumbnail size

    if (2 * s->thumbnailGroupingRadius < newThumbnailSize)
    {
        /// @todo more straightforward way for this?

        s->thumbnailGroupingRadius = newThumbnailSize / 2 + newThumbnailSize % 2;
    }

    if (s->showThumbnails)
    {
        slotRequestLazyReclustering();
    }

    slotUpdateActionsEnabled();
}

void MapWidget::setThumbnailGroupingRadius(const int newGroupingRadius)
{
    s->thumbnailGroupingRadius = qMax(GeoIfaceMinThumbnailGroupingRadius, newGroupingRadius);

    // make sure the thumbnails are smaller than the grouping radius

    if (2*s->thumbnailGroupingRadius < s->thumbnailSize)
    {
        s->thumbnailSize = 2 * newGroupingRadius;
    }

    if (s->showThumbnails)
    {
        slotRequestLazyReclustering();
    }

    slotUpdateActionsEnabled();
}

void MapWidget::setMarkerGroupingRadius(const int newGroupingRadius)
{
    s->markerGroupingRadius = qMax(GeoIfaceMinMarkerGroupingRadius, newGroupingRadius);

    if (!s->showThumbnails)
    {
        slotRequestLazyReclustering();
    }

    slotUpdateActionsEnabled();
}

void MapWidget::slotDecreaseThumbnailSize()
{
    if (!s->showThumbnails)
    {
        return;
    }

    if (s->thumbnailSize > GeoIfaceMinThumbnailSize)
    {
        const int newThumbnailSize = qMax(GeoIfaceMinThumbnailSize, s->thumbnailSize-5);

        // make sure the grouping radius is also decreased
        // this will automatically decrease the thumbnail size as well

        setThumbnailGroupingRadius(newThumbnailSize / 2);
    }
}

void MapWidget::slotIncreaseThumbnailSize()
{
    if (!s->showThumbnails)
    {
        return;
    }

    setThumnailSize(s->thumbnailSize+5);
}

int MapWidget::getThumbnailSize() const
{
    return s->thumbnailSize;
}

int MapWidget::getUndecoratedThumbnailSize() const
{
    return s->thumbnailSize-2;
}

void MapWidget::slotUngroupedModelChanged()
{
    // determine the index under which we handle this model

    QObject* const senderObject           = sender();
    QAbstractItemModel* const senderModel = qobject_cast<QAbstractItemModel*>(senderObject);

    if (senderModel)
    {
        for (int i = 0 ; i < s->ungroupedModels.count() ; ++i)
        {
            if (s->ungroupedModels.at(i)->model() == senderModel)
            {
                Q_EMIT signalUngroupedModelChanged(i);

                break;
            }
        }
        return;
    }

    GeoModelHelper* const senderHelper = qobject_cast<GeoModelHelper*>(senderObject);

    if (senderHelper)
    {
        for (int i = 0 ; i < s->ungroupedModels.count() ; ++i)
        {
            if (s->ungroupedModels.at(i) == senderHelper)
            {
                Q_EMIT signalUngroupedModelChanged(i);

                break;
            }
        }
    }

    QItemSelectionModel* const senderSelectionModel = qobject_cast<QItemSelectionModel*>(senderObject);

    if (senderSelectionModel)
    {
        for (int i = 0 ; i < s->ungroupedModels.count() ; ++i)
        {
            if (s->ungroupedModels.at(i)->selectionModel() == senderSelectionModel)
            {
                Q_EMIT signalUngroupedModelChanged(i);

                break;
            }
        }

        return;
    }
}

void MapWidget::addWidgetToControlWidget(QWidget* const newWidget)
{
    // make sure the control widget exists

    if (!d->controlWidget)
    {
        getControlWidget();
    }

    QHBoxLayout* const hBoxLayout = reinterpret_cast<QHBoxLayout*>(d->hBoxForAdditionalControlWidgetItems->layout());

    if (hBoxLayout)
    {
        hBoxLayout->addWidget(newWidget);
    }
}

void MapWidget::setActive(const bool state)
{
    const bool oldState = s->activeState;
    s->activeState      = state;

    if (d->currentBackend)
    {
        d->currentBackend->setActive(state);
    }

    if (s->markerModel)
    {
        s->markerModel->setActive(state);
    }

    if (state)
    {
        // do we have a map widget shown?

        if ((d->stackedLayout->count() == 1) && d->currentBackend)
        {
            setMapWidgetInFrame(d->currentBackend->mapWidget());

            // call this slot manually in case the backend was ready right away:

            if (d->currentBackend->isReady())
            {
                slotBackendReadyChanged(d->currentBackendName);
            }
            else
            {
                rebuildConfigurationMenu();
            }
        }
    }

    if (state && !oldState && s->tileGrouper->getClustersDirty())
    {
        slotRequestLazyReclustering();
    }
}

bool MapWidget::getActiveState()
{
    return s->activeState;
}

void MapWidget::setVisibleMouseModes(const GeoMouseModes mouseModes)
{
    s->visibleMouseModes = mouseModes;

    if (d->mouseModesHolder)
    {
        d->mouseModesHolder->setVisible(s->visibleMouseModes);
        d->setSelectionModeButton->setVisible(s->visibleMouseModes.testFlag(MouseModeRegionSelection));
        d->removeCurrentSelectionButton->setVisible(s->visibleMouseModes.testFlag(MouseModeRegionSelection));
        d->setPanModeButton->setVisible(s->visibleMouseModes.testFlag(MouseModePan));
        d->setZoomModeButton->setVisible(s->visibleMouseModes.testFlag(MouseModeZoomIntoGroup));
        d->setRegionSelectionFromIconModeButton->setVisible(s->visibleMouseModes.testFlag(MouseModeRegionSelectionFromIcon));
        d->setFilterModeButton->setVisible(s->visibleMouseModes.testFlag(MouseModeFilter));
        d->removeFilterModeButton->setVisible(s->visibleMouseModes.testFlag(MouseModeFilter));
        d->setSelectThumbnailMode->setVisible(s->visibleMouseModes.testFlag(MouseModeSelectThumbnail));
    }
}

void MapWidget::setAvailableMouseModes(const GeoMouseModes mouseModes)
{
    s->availableMouseModes = mouseModes;
}

bool MapWidget::getStickyModeState() const
{
    return d->actionStickyMode->isChecked();
}

void MapWidget::setStickyModeState(const bool state)
{
    d->actionStickyMode->setChecked(state);

    slotUpdateActionsEnabled();
}

void MapWidget::setVisibleExtraActions(const GeoExtraActions actions)
{
    d->visibleExtraActions = actions;

    if (d->buttonStickyMode)
    {
        d->buttonStickyMode->setVisible(actions.testFlag(ExtraActionSticky));
    }

    if (d->loadTracksFromAlbums)
    {
        d->loadTracksFromAlbums->setVisible(actions.testFlag(ExtraLoadTracks));
    }

    slotUpdateActionsEnabled();
}

void MapWidget::setEnabledExtraActions(const GeoExtraActions actions)
{
    d->availableExtraActions = actions;

    slotUpdateActionsEnabled();
}

void MapWidget::slotStickyModeChanged()
{
    slotUpdateActionsEnabled();

    Q_EMIT signalStickyModeChanged();
}

void MapWidget::setAllowModifications(const bool state)
{
    s->modificationsAllowed = state;

    slotUpdateActionsEnabled();
    slotRequestLazyReclustering();
}

void MapWidget::slotMouseModeChanged(QAction* triggeredAction)
{
    // determine the new mouse mode:

    const QVariant triggeredActionData = triggeredAction->data();
    const GeoMouseModes newMouseMode   = triggeredActionData.value<Digikam::GeoMouseModes>();

    if (newMouseMode == s->currentMouseMode)
    {
        return;
    }

    // store the new mouse mode:

    s->currentMouseMode = newMouseMode;

    if (d->currentBackend)
    {
        d->currentBackend->mouseModeChanged();
    }

    Q_EMIT signalMouseModeChanged(s->currentMouseMode);

    /// @todo Update action availability?
}

void MapWidget::setMouseMode(const GeoMouseModes mouseMode)
{
    s->currentMouseMode = mouseMode;

    if (currentBackendReady())
    {
        d->currentBackend->mouseModeChanged();
    }

    slotUpdateActionsEnabled();
}

void MapWidget::setTrackManager(TrackManager* const trackManager)
{
    s->trackManager = trackManager;

    // Some backends track the track manager activity even when not active
    // therefore they have to be notified.

    Q_FOREACH (MapBackend* const backend, d->loadedBackends)
    {
        backend->slotTrackManagerChanged();
    }
}

} // namespace Digikam

#include "moc_mapwidget.cpp"
