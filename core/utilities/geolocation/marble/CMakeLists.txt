#
# SPDX-FileCopyrightText: 2010-2023 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

add_definitions(-DMARBLE_NO_WEBKITWIDGETS)

add_definitions(
    -DQT_USE_QSTRINGBUILDER
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_BYTEARRAY
)

if (NOT WIN32)
    # Strict iterators can't be used on Windows, they lead to a link error
    # when application code iterates over a QVector<QPoint> for instance
    # See example at https://bugreports.qt.io/browse/QTBUG-78112
    add_definitions(-DQT_STRICT_ITERATORS)
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
    # Its good programming practice to build with no warnings...
    add_definitions( -Wall -Wextra -Wundef -Wnon-virtual-dtor -Woverloaded-virtual -Wno-long-long
                     -Wchar-subscripts -Wcast-align -Wpointer-arith -Wformat-security )
endif()

include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Xml,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Network,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Svg,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Concurrent,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core5Compat,INTERFACE_INCLUDE_DIRECTORIES>
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
    TimeControlWidget.cpp
    AbstractFloatItem.cpp
    MarbleGlobal.cpp
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

    cloudsync/CloudSyncManager.cpp
    cloudsync/RouteSyncManager.cpp
    cloudsync/CloudRouteModel.cpp
    cloudsync/CloudRoutesDialog.cpp
    cloudsync/RouteItem.cpp
    cloudsync/RouteItemDelegate.cpp
    cloudsync/BookmarkSyncManager.cpp
    cloudsync/MergeItem.cpp
    cloudsync/ConflictDialog.cpp
    cloudsync/OwncloudSyncBackend.cpp

    MarbleDirs.cpp
    PluginManager.cpp
    StyleBuilder.cpp
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

set_target_properties(core_marble_obj
                      PROPERTIES
                      COMPILE_FLAGS "-DKML_LAZY_IMP"
)

add_subdirectory(data)
