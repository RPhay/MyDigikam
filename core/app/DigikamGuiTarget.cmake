#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

### digiKam GUI object library ##################################################################################################

if(ENABLE_DBUS)
    qt5_add_dbus_adaptor(digikamadaptor_SRCS
                         ${CMAKE_SOURCE_DIR}/core/app/main/org.kde.digikam.xml
                         ${CMAKE_SOURCE_DIR}/core/app/main/digikamapp.h
                         Digikam::DigikamApp)
endif()

set(libdigikamgui_SRCS

    ${CMAKE_SOURCE_DIR}/core/app/main/digikamapp.cpp
    ${CMAKE_SOURCE_DIR}/core/app/main/digikamapp_solid.cpp
    ${CMAKE_SOURCE_DIR}/core/app/main/digikamapp_camera.cpp
    ${CMAKE_SOURCE_DIR}/core/app/main/digikamapp_import.cpp
    ${CMAKE_SOURCE_DIR}/core/app/main/digikamapp_config.cpp
    ${CMAKE_SOURCE_DIR}/core/app/main/digikamapp_tools.cpp
    ${CMAKE_SOURCE_DIR}/core/app/main/digikamapp_setup.cpp

    ${CMAKE_SOURCE_DIR}/core/app/date/dpopupframe.cpp
    ${CMAKE_SOURCE_DIR}/core/app/date/ddateedit.cpp
    ${CMAKE_SOURCE_DIR}/core/app/date/ddatetable.cpp
    ${CMAKE_SOURCE_DIR}/core/app/date/ddatetable_p.cpp
    ${CMAKE_SOURCE_DIR}/core/app/date/ddatepicker.cpp
    ${CMAKE_SOURCE_DIR}/core/app/date/ddatepicker_p.cpp
    ${CMAKE_SOURCE_DIR}/core/app/date/ddatetimeedit.cpp
    ${CMAKE_SOURCE_DIR}/core/app/date/ddatepickerpopup.cpp
    ${CMAKE_SOURCE_DIR}/core/app/date/datefolderview.cpp
    ${CMAKE_SOURCE_DIR}/core/app/date/monthwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/app/date/timelinewidget.cpp

    ${CMAKE_SOURCE_DIR}/core/app/dragdrop/importdragdrop.cpp
    ${CMAKE_SOURCE_DIR}/core/app/dragdrop/albumdragdrop.cpp
    ${CMAKE_SOURCE_DIR}/core/app/dragdrop/ddragobjects.cpp
    ${CMAKE_SOURCE_DIR}/core/app/dragdrop/itemdragdrop.cpp
    ${CMAKE_SOURCE_DIR}/core/app/dragdrop/tagdragdrop.cpp

    ${CMAKE_SOURCE_DIR}/core/app/filters/filtersidebarwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/app/filters/tagfilterview.cpp

    ${CMAKE_SOURCE_DIR}/core/app/items/delegate/digikamitemdelegate.cpp
    ${CMAKE_SOURCE_DIR}/core/app/items/delegate/itemdelegate.cpp
    ${CMAKE_SOURCE_DIR}/core/app/items/delegate/itemfacedelegate.cpp
    ${CMAKE_SOURCE_DIR}/core/app/items/views/digikamitemview.cpp
    ${CMAKE_SOURCE_DIR}/core/app/items/views/digikamitemview_p.cpp
    ${CMAKE_SOURCE_DIR}/core/app/items/views/itemcategorizedview.cpp
    ${CMAKE_SOURCE_DIR}/core/app/items/thumbbar/itemthumbnailbar.cpp
    ${CMAKE_SOURCE_DIR}/core/app/items/thumbbar/itemthumbnaildelegate.cpp
    ${CMAKE_SOURCE_DIR}/core/app/items/overlays/assignnameoverlay.cpp
    ${CMAKE_SOURCE_DIR}/core/app/items/overlays/facerejectionoverlay.cpp
    ${CMAKE_SOURCE_DIR}/core/app/items/overlays/groupindicatoroverlay.cpp
    ${CMAKE_SOURCE_DIR}/core/app/items/overlays/itemfullscreenoverlay.cpp
    ${CMAKE_SOURCE_DIR}/core/app/items/overlays/itemratingoverlay.cpp
    ${CMAKE_SOURCE_DIR}/core/app/items/overlays/itemrotationoverlay.cpp
    ${CMAKE_SOURCE_DIR}/core/app/items/overlays/itemcoordinatesoverlay.cpp
    ${CMAKE_SOURCE_DIR}/core/app/items/overlays/itemselectionoverlay.cpp
    ${CMAKE_SOURCE_DIR}/core/app/items/utils/itemviewutilities.cpp
    ${CMAKE_SOURCE_DIR}/core/app/items/utils/tooltipfiller.cpp
    ${CMAKE_SOURCE_DIR}/core/app/items/utils/contextmenuhelper.cpp
    ${CMAKE_SOURCE_DIR}/core/app/items/utils/groupingviewimplementation.cpp
    ${CMAKE_SOURCE_DIR}/core/app/items/utils/itemcategorydrawer.cpp

    ${CMAKE_SOURCE_DIR}/core/app/utils/digikam_debug.cpp

    ${CMAKE_SOURCE_DIR}/core/app/views/tableview/tableview.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/tableview/tableview_treeview.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/tableview/tableview_treeview_delegate.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/tableview/tableview_column_configuration_dialog.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/tableview/tableview_model.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/tableview/tableview_columns.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/tableview/tableview_column_audiovideo.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/tableview/tableview_column_file.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/tableview/tableview_column_geo.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/tableview/tableview_column_digikam.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/tableview/tableview_column_item.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/tableview/tableview_column_photo.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/tableview/tableview_column_thumbnail.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/tableview/tableview_columnfactory.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/tableview/tableview_selection_model_syncer.cpp

    ${CMAKE_SOURCE_DIR}/core/app/views/stack/itemiconview.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/stack/itemiconview_albums.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/stack/itemiconview_groups.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/stack/itemiconview_items.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/stack/itemiconview_search.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/stack/itemiconview_sidebars.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/stack/itemiconview_slideshow.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/stack/itemiconview_tags.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/stack/itemiconview_tools.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/stack/itemiconview_views.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/stack/itemiconview_zoom.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/stack/trashview.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/stack/stackedview.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/stack/welcomepageview.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/preview/itempreviewcanvas.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/preview/itempreviewview.cpp

    ${CMAKE_SOURCE_DIR}/core/app/views/sidebar/albumfolderviewsidebarwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/sidebar/datefolderviewsidebarwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/sidebar/timelinesidebarwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/sidebar/searchsidebarwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/sidebar/fuzzysearchsidebarwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/sidebar/labelssidebarwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/sidebar/peoplesidebarwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/sidebar/tagviewsidebarwidget.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/sidebar/sidebarwidget.cpp

    ${CMAKE_SOURCE_DIR}/core/app/views/utils/dmodelfactory.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/utils/slideshowbuilder.cpp
    ${CMAKE_SOURCE_DIR}/core/app/views/utils/componentsinfodlg.cpp

    ${digikamadaptor_SRCS}
)

if(${Marble_FOUND})
    set(libdigikamgui_SRCS
        ${libdigikamgui_SRCS}
        ${CMAKE_SOURCE_DIR}/core/app/views/stack/mapwidgetview.cpp
        ${CMAKE_SOURCE_DIR}/core/app/views/sidebar/gpssearchsidebarwidget.cpp
       )
endif()


add_library(digikamgui_src
            OBJECT
            ${libdigikamgui_SRCS}
)

### digiKam GUI shared library objects declaration ##############################################################################

set(DIGIKAM_OBJECTS

            $<TARGET_OBJECTS:digikamdatabasemain_src>

            $<TARGET_OBJECTS:digikamfacesenginedatabase_src>
            $<TARGET_OBJECTS:digikamgui_src>
            $<TARGET_OBJECTS:digikamdeletedialog_src>
            $<TARGET_OBJECTS:digikamtemplate_src>
            $<TARGET_OBJECTS:itempropertiesdigikam_src>
            $<TARGET_OBJECTS:setup_src>
            $<TARGET_OBJECTS:lighttable_src>
            $<TARGET_OBJECTS:maintenance_src>
            $<TARGET_OBJECTS:searchwindow_src>
            $<TARGET_OBJECTS:digikammodels_src>
            $<TARGET_OBJECTS:digikamalbum_src>
            $<TARGET_OBJECTS:firstrun_src>
            $<TARGET_OBJECTS:fuzzysearch_src>
            $<TARGET_OBJECTS:imageeditorgui_src>
            $<TARGET_OBJECTS:fileactionmanager_src>
            $<TARGET_OBJECTS:digikamtags_src>
            $<TARGET_OBJECTS:digikamsettings_src>
            $<TARGET_OBJECTS:filters_src>
            $<TARGET_OBJECTS:importuibackend_src>
            $<TARGET_OBJECTS:imagehistorywidgets_src>
            $<TARGET_OBJECTS:iojobs_src>
            $<TARGET_OBJECTS:dtrash_src>
            $<TARGET_OBJECTS:facemanagement_src>
            $<TARGET_OBJECTS:queuemanager_src>
            $<TARGET_OBJECTS:importui_src>
            $<TARGET_OBJECTS:advancedrename_src>
)

if(${Marble_FOUND})

    set(DIGIKAM_OBJECTS
        ${DIGIKAM_OBJECTS}
        $<TARGET_OBJECTS:geomapwrapper_src>
        $<TARGET_OBJECTS:gpssearch_src>
    )

endif()

### digiKam GUI shared library target ###########################################################################################

add_library(digikamgui
            SHARED
            ${DIGIKAM_OBJECTS}
)

set_target_properties(digikamgui PROPERTIES VERSION ${DIGIKAM_VERSION_SHORT} SOVERSION ${DIGIKAM_VERSION_SHORT})

add_dependencies(digikamgui digikamcore digikamdatabase)

if(WIN32)
    set_target_properties(digikamgui PROPERTIES COMPILE_FLAGS -DJPEG_STATIC)
endif()

target_link_libraries(digikamgui

                      PUBLIC

                      digikamcore
                      digikamdatabase

                      Qt5::Core
                      Qt5::Gui
                      Qt5::Widgets
                      Qt5::Sql
                      Qt5::PrintSupport

                      KF5::XmlGui
                      KF5::Solid
                      KF5::ConfigCore
                      KF5::ConfigGui
                      KF5::Service
                      KF5::WindowSystem
                      KF5::I18n

                      ${OpenCV_LIBRARIES}
)

if(ENABLE_QWEBENGINE)
    target_link_libraries(digikamgui PRIVATE Qt5::WebEngineWidgets)
else()
    target_link_libraries(digikamgui PRIVATE Qt5::WebKitWidgets)
endif()

if(ENABLE_DBUS)
    target_link_libraries(digikamgui PRIVATE Qt5::DBus)
endif()

if(KF5IconThemes_FOUND)
    target_link_libraries(digikamgui PRIVATE KF5::IconThemes)
endif()

if(KF5KIO_FOUND)
    target_link_libraries(digikamgui PRIVATE KF5::KIOWidgets)
endif()

if(${Marble_FOUND})
     target_link_libraries(digikamgui PRIVATE ${MARBLE_LIBRARIES})
endif()

if(APPLE)
    target_link_libraries(digikamgui PRIVATE /System/Library/Frameworks/AppKit.framework)
endif()

if(NOT WIN32)
    # To link under Solaris (see bug #274484)
    target_link_libraries(digikamgui PRIVATE ${MATH_LIBRARY})
endif()

if(Gphoto2_FOUND)

    # See bug #258931: libgphoto2 library must be the last arg for linker.
    # See bug #268267 : digiKam need to be linked to libusb to prevent crash
    # at gphoto2 init if opencv is linked with libdc1394. Libusb linking rules are
    # add to gphoto2 linking rules by Gphoto2 cmake detection script.

    target_link_libraries(digikamgui PRIVATE ${GPHOTO2_LIBRARIES})

endif()

### Install Rules ###############################################################################################################

install(TARGETS digikamgui EXPORT DigikamGuiConfig ${INSTALL_TARGETS_DEFAULT_ARGS})
install(EXPORT DigikamGuiConfig  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/DigikamGui" NAMESPACE Digikam::)

write_basic_package_version_file(${CMAKE_BINARY_DIR}/core/app/DigikamGuiConfigVersion.cmake
                                 VERSION ${DIGIKAM_VERSION_SHORT}
                                 COMPATIBILITY SameMajorVersion)

install(FILES ${CMAKE_BINARY_DIR}/core/app/DigikamGuiConfigVersion.cmake
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/DigikamGui")
