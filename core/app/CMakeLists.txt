#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

# -----------------------------------------------------------------------------------
# Headers to configure

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/utils/digikam_version.h.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/utils/digikam_version.h)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/utils/digikam_dbconfig.h.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/utils/digikam_dbconfig.h)

# -----------------------------------------------------------------------------------
# Common includes rules

include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::PrintSupport,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Xml,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Sql,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Network,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::Solid,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::WindowSystem,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::Service,INTERFACE_INCLUDE_DIRECTORIES>

                    ${CMAKE_SOURCE_DIR}/core/app/main
                    ${CMAKE_SOURCE_DIR}/core/app/date
                    ${CMAKE_SOURCE_DIR}/core/app/filters
                    ${CMAKE_SOURCE_DIR}/core/app/dragdrop
                    ${CMAKE_SOURCE_DIR}/core/app/views/stack
                    ${CMAKE_SOURCE_DIR}/core/app/views/utils
                    ${CMAKE_SOURCE_DIR}/core/app/views/sidebar
                    ${CMAKE_SOURCE_DIR}/core/app/views/tableview
                    ${CMAKE_SOURCE_DIR}/core/app/items/utils
                    ${CMAKE_SOURCE_DIR}/core/app/items/thumbbar
                    ${CMAKE_SOURCE_DIR}/core/app/items/delegate
                    ${CMAKE_SOURCE_DIR}/core/app/items/views
                    ${CMAKE_SOURCE_DIR}/core/app/items/overlays
                    ${CMAKE_SOURCE_DIR}/core/app/views/preview

                    ${CMAKE_SOURCE_DIR}/core/utilities/advancedrename
                    ${CMAKE_SOURCE_DIR}/core/utilities/firstrun
                    ${CMAKE_SOURCE_DIR}/core/utilities/setup
                    ${CMAKE_SOURCE_DIR}/core/utilities/setup/camera
                    ${CMAKE_SOURCE_DIR}/core/utilities/setup/editor
                    ${CMAKE_SOURCE_DIR}/core/utilities/setup/system
                    ${CMAKE_SOURCE_DIR}/core/utilities/setup/downloader
                    ${CMAKE_SOURCE_DIR}/core/utilities/searchwindow
                    ${CMAKE_SOURCE_DIR}/core/utilities/maintenance
                    ${CMAKE_SOURCE_DIR}/core/utilities/extrasupport/filesindexer
                    ${CMAKE_SOURCE_DIR}/core/utilities/extrasupport/addressbook
                    ${CMAKE_SOURCE_DIR}/core/utilities/import/main
                    ${CMAKE_SOURCE_DIR}/core/utilities/import/models
                    ${CMAKE_SOURCE_DIR}/core/utilities/import/views
                    ${CMAKE_SOURCE_DIR}/core/utilities/import/backend
                    ${CMAKE_SOURCE_DIR}/core/utilities/imageeditor/editor
                    ${CMAKE_SOURCE_DIR}/core/utilities/imageeditor/main
                    ${CMAKE_SOURCE_DIR}/core/utilities/imageeditor/core
                    ${CMAKE_SOURCE_DIR}/core/utilities/imageeditor/widgets
                    ${CMAKE_SOURCE_DIR}/core/utilities/facemanagement/threads
                    ${CMAKE_SOURCE_DIR}/core/utilities/facemanagement/database
                    ${CMAKE_SOURCE_DIR}/core/utilities/facemanagement/widgets
                    ${CMAKE_SOURCE_DIR}/core/utilities/facemanagement/items
                    ${CMAKE_SOURCE_DIR}/core/utilities/lighttable
                    ${CMAKE_SOURCE_DIR}/core/utilities/fuzzysearch
                    ${CMAKE_SOURCE_DIR}/core/utilities/focuspointmanagement
                    ${CMAKE_SOURCE_DIR}/core/utilities/queuemanager/main
                    ${CMAKE_SOURCE_DIR}/core/utilities/geolocation/mapsearches
                    ${CMAKE_SOURCE_DIR}/core/utilities/geolocation/geoiface/core
                    ${CMAKE_SOURCE_DIR}/core/utilities/geolocation/geoiface/tiles
                    ${CMAKE_SOURCE_DIR}/core/utilities/geolocation/geoiface/tracks
                    ${CMAKE_SOURCE_DIR}/core/utilities/geolocation/geoiface/correlator
                    ${CMAKE_SOURCE_DIR}/core/utilities/geolocation/geoiface/reversegeocoding
                    ${CMAKE_SOURCE_DIR}/core/utilities/geolocation/geoiface/items
                    ${CMAKE_SOURCE_DIR}/core/utilities/geolocation/geoiface/widgets
                    ${CMAKE_SOURCE_DIR}/core/utilities/geolocation/geomapwrapper
)

if(ENABLE_QWEBENGINE)
    include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::WebEngineWidgets,INTERFACE_INCLUDE_DIRECTORIES>)
else()
    include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::WebKitWidgets,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

if(KF5KIO_FOUND)
    include_directories($<TARGET_PROPERTY:KF5::KIOWidgets,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

if(Gphoto2_FOUND)
    include_directories(${GPHOTO2_INCLUDE_DIRS})
endif()

# -----------------------------------------------------------------------------------
# Targets to build

include(DigikamCoreTarget.cmake)
include(DigikamDatabaseTarget.cmake)
include(DigikamGuiTarget.cmake)
include(DigikamTarget.cmake)

# -----------------------------------------------------------------------------------
# Others Files to install

install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/main/org.kde.digikam.desktop     DESTINATION ${KDE_INSTALL_FULL_APPDIR})
install(FILES    ${CMAKE_CURRENT_SOURCE_DIR}/main/org.kde.digikam.appdata.xml DESTINATION ${KDE_INSTALL_FULL_METAINFODIR})
install(FILES    ${CMAKE_CURRENT_SOURCE_DIR}/main/digikamui5.rc               DESTINATION ${KDE_INSTALL_FULL_KXMLGUIDIR}/digikam)
install(FILES    ${CMAKE_CURRENT_SOURCE_DIR}/main/digikam.notifyrc            DESTINATION ${KDE_INSTALL_FULL_KNOTIFYRCDIR})

include(DigikamExportAPI.cmake)
