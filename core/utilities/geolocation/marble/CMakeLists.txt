#
# SPDX-FileCopyrightText: 2010-2023 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

find_package(Qt6 ${QT6_MIN_VERSION} REQUIRED
    COMPONENTS
        Svg
        Core5Compat
)

add_definitions(-DMARBLE_NO_WEBKITWIDGETS)
add_definitions(-DKML_LAZY_IMP)

include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Xml,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Network,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Concurrent,INTERFACE_INCLUDE_DIRECTORIES>
)

include(astro/CMakeLists.txt)
include(geodata/CMakeLists.txt)
include(graphicsview/CMakeLists.txt)
include(layers/CMakeLists.txt)
include(osm/CMakeLists.txt)

set(marblewidget_SRCS
    ${astrolib_SRCS}
    ${geodata_SRCS}
    ${graphicsview_SRCS}
    ${layers_SRCS}
    ${osm_SRCS}
    blendings/Blending.cpp
    blendings/BlendingAlgorithms.cpp
    blendings/BlendingFactory.cpp
    blendings/SunLightBlending.cpp
    DownloadRegion.cpp
    DownloadRegionDialog.cpp
    LatLonBoxWidget.cpp
    MarbleWidget.cpp
    MarbleAbstractPresenter.cpp
    MarbleModel.cpp
    MarbleMap.cpp
    MarbleColors.cpp
    MapViewWidget.cpp
    CelestialSortFilterProxyModel.cpp
    FileViewWidget.cpp
    TourWidget.cpp
    FlyToEditWidget.cpp
    TourControlEditWidget.cpp
    WaitEditWidget.cpp
    SoundCueEditWidget.cpp
    RemoveItemEditWidget.cpp
    TourItemDelegate.cpp
    MapViewItemDelegate.cpp
    TourPlayback.cpp
    PlaybackItem.cpp
    PlaybackAnimatedUpdateItem.cpp
    PlaybackFlyToItem.cpp
    PlaybackSoundCueItem.cpp
    PlaybackTourControlItem.cpp
    PlaybackWaitItem.cpp
    SerialTrack.cpp
    SoundTrack.cpp
    AnimatedUpdateTrack.cpp
    CurrentLocationWidget.cpp
    MarbleNavigator.cpp
    MarbleAboutDialog.cpp
    MarbleInputHandler.cpp
    MarbleWidgetInputHandler.cpp
    MarbleWidgetPopupMenu.cpp
    MarblePlacemarkModel.cpp
    GeoDataTreeModel.cpp
    GeoUriParser.cpp
    kdescendantsproxymodel.cpp
    BranchFilterProxyModel.cpp
    TreeViewDecoratorModel.cpp
    MarbleDebug.cpp
    Tile.cpp
    TextureTile.cpp
    TileCoordsPyramid.cpp
    TileLevelRangeWidget.cpp
    TileLoader.cpp
    QtMarbleConfigDialog.cpp
    ClipPainter.cpp
    DownloadPolicy.cpp
    DownloadQueueSet.cpp
    GeoPainter.cpp
    HttpDownloadManager.cpp
    HttpJob.cpp
    RemoteIconLoader.cpp
    LayerManager.cpp
    PluginManager.cpp
    TimeControlWidget.cpp
    AbstractFloatItem.cpp
    MarbleGlobal.cpp
    MarbleDirs.cpp
    MarbleLocale.cpp
    MarblePhysics.cpp
    TileCreatorDialog.cpp
    MapThemeManager.cpp
    ViewportParams.cpp
    ViewParams.cpp
    projections/AbstractProjection.cpp
    projections/CylindricalProjection.cpp
    projections/AzimuthalProjection.cpp
    projections/SphericalProjection.cpp
    projections/EquirectProjection.cpp
    projections/MercatorProjection.cpp
    projections/GnomonicProjection.cpp
    projections/StereographicProjection.cpp
    projections/LambertAzimuthalProjection.cpp
    projections/AzimuthalEquidistantProjection.cpp
    projections/VerticalPerspectiveProjection.cpp
    VisiblePlacemark.cpp
    PlacemarkLayout.cpp
    Planet.cpp
    PlanetFactory.cpp
    Quaternion.cpp
    TextureColorizer.cpp
    TextureMapperInterface.cpp
    ScanlineTextureMapperContext.cpp
    SphericalScanlineTextureMapper.cpp
    EquirectScanlineTextureMapper.cpp
    MercatorScanlineTextureMapper.cpp
    TileScalingTextureMapper.cpp
    GenericScanlineTextureMapper.cpp
    VectorTileModel.cpp
    DiscCache.cpp
    ServerLayout.cpp
    StoragePolicy.cpp
    CacheStoragePolicy.cpp
    FileStoragePolicy.cpp
    FileStorageWatcher.cpp
    StackedTile.cpp
    TileId.cpp
    StackedTileLoader.cpp
    TileLoaderHelper.cpp
    TileCreator.cpp
    #jsonparser.cpp
    FileLoader.cpp
    FileManager.cpp
    PositionTracking.cpp
    DataMigration.cpp
    ImageF.cpp
    MovieCapture.cpp
    MovieCaptureDialog.cpp
    TourCaptureDialog.cpp
    EditPlacemarkDialog.cpp
    AddLinkDialog.cpp
    FormattedTextWidget.cpp

    AbstractDataPlugin.cpp
    AbstractDataPluginModel.cpp
    AbstractDataPluginItem.cpp
    AbstractWorkerThread.cpp

    PluginInterface.cpp
    DialogConfigurationInterface.cpp
    LayerInterface.cpp
    RenderState.cpp
    RenderPlugin.cpp
    RenderPluginInterface.cpp
    PositionProviderPlugin.cpp
    PositionProviderPluginInterface.cpp
    PlacemarkPositionProviderPlugin.cpp
    RouteSimulationPositionProviderPlugin.cpp
    MarblePluginSettingsWidget.cpp
    RenderPluginModel.cpp
    PluginAboutDialog.cpp
    PluginItemDelegate.cpp

    SunLocator.cpp
    MarbleClock.cpp
    SunControlWidget.cpp
    MergedLayerDecorator.cpp

    MathHelper.cpp

    LatLonEdit.cpp
    PlacemarkEditHeader.cpp

    MapThemeSortFilterProxyModel.cpp

    TemplateDocument.cpp

    routing/AlternativeRoutesModel.cpp
    routing/Maneuver.cpp
    routing/Route.cpp
    routing/RouteRequest.cpp
    routing/RouteSegment.cpp
    routing/RoutingModel.cpp
    routing/RoutingProfile.cpp
    routing/RoutingManager.cpp
    routing/RoutingLayer.cpp
    routing/RoutingInputWidget.cpp
    routing/RoutingWidget.cpp
    routing/RoutingProfilesWidget.cpp
    routing/RoutingProfilesModel.cpp
    routing/RoutingProfileSettingsDialog.cpp
    routing/SpeakersModel.cpp
    routing/VoiceNavigationModel.cpp
    routing/instructions/InstructionTransformation.cpp
    routing/instructions/RoutingInstruction.cpp
    routing/instructions/RoutingPoint.cpp
    routing/instructions/RoutingWaypoint.cpp
    routing/instructions/WaypointParser.cpp

    ParsingRunnerManager.cpp
    ReverseGeocodingRunnerManager.cpp
    RoutingRunnerManager.cpp
    SearchRunnerManager.cpp

    AutoNavigation.cpp

    SearchRunnerPlugin.cpp
    ReverseGeocodingRunnerPlugin.cpp
    RoutingRunnerPlugin.cpp
    ParseRunnerPlugin.cpp
    SearchRunner.cpp
    ReverseGeocodingRunner.cpp
    RoutingRunner.cpp
    ParsingRunner.cpp
    RunnerTask.cpp

    BookmarkManager.cpp
    EditBookmarkDialog.cpp
    BookmarkManagerDialog.cpp
    NewBookmarkFolderDialog.cpp

    PrintOptionsWidget.cpp
    ExternalEditorDialog.cpp
    GoToDialog.cpp

    MapWizard.cpp
    OwsServiceManager.cpp
    MapThemeDownloadDialog.cpp
    GeoGraphicsScene.cpp
    ElevationModel.cpp
    MarbleLineEdit.cpp
    SearchInputWidget.cpp
    SearchWidget.cpp
    kineticmodel.cpp
    NewstuffModel.cpp
    MarbleZip.cpp

    OsmcSymbol.cpp

    StyleBuilder.cpp

    cloudsync/CloudSyncManager.cpp
    cloudsync/RouteSyncManager.cpp
    cloudsync/OwncloudSyncBackend.cpp
    cloudsync/CloudRouteModel.cpp
    cloudsync/CloudRoutesDialog.cpp
    cloudsync/RouteItem.cpp
    cloudsync/RouteItemDelegate.cpp
    cloudsync/BookmarkSyncManager.cpp
    cloudsync/MergeItem.cpp
    cloudsync/ConflictDialog.cpp
)

set (marblewidget_UI
    ${osm_UIS}
    LatLonBoxWidget.ui
    MapViewWidget.ui
    CurrentLocationWidget.ui
    FileViewWidget.ui
    TourWidget.ui
    MarbleNavigator.ui
    MarbleCacheSettingsWidget.ui
    MarbleViewSettingsWidget.ui
    MarbleNavigationSettingsWidget.ui
    TimeControlWidget.ui
    MarbleTimeSettingsWidget.ui
    MarblePluginSettingsWidget.ui
    TileCreatorDialog.ui
    TileLevelRangeWidget.ui
    MarbleAboutDialog.ui
    SunControlWidget.ui
    LatLonEdit.ui
    PlacemarkEditHeader.ui
    routing/RoutingWidget.ui
    DataMigrationWidget.ui
    EditBookmarkDialog.ui
    BookmarkManagerDialog.ui
    NewBookmarkFolderDialog.ui
    PrintOptions.ui
    ExternalEditor.ui
    GoToDialog.ui
    routing/RoutingSettingsWidget.ui
    routing/RoutingProfileSettingsDialog.ui
    MapWizard.ui
    MapThemeDownloadDialog.ui
    cloudsync/CloudRoutesDialog.ui
    MarbleCloudSyncSettingsWidget.ui
    MovieCaptureDialog.ui
    TourCaptureDialog.ui
    EditPlacemarkDialog.ui
    AddLinkDialog.ui
    FormattedTextWidget.ui
    ElevationWidget.ui
)

qt_add_resources(marblewidget_SRCS libmarble.qrc)

qt_wrap_ui(marblewidget_SRCS ${marblewidget_UI})

# Used by digikamcore
add_library(core_marble_obj OBJECT  ${marblewidget_SRCS} ${SOURCES_UI_HDRS})

target_compile_definitions(core_marble_obj
                           PRIVATE
                           digikamcore_EXPORTS
)

target_link_libraries(core_marble_obj
    PUBLIC
        Qt6::Xml
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
        Qt6::Core5Compat
    PRIVATE
        Qt6::Core
        Qt6::Svg
        Qt6::PrintSupport
        Qt6::Concurrent
)
