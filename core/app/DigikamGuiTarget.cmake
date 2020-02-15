#
# Copyright (c) 2010-2020 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

### digiKam GUI object library ##################################################################################################

if(ENABLE_DBUS)
    qt5_add_dbus_adaptor(digikamadaptor_SRCS
                         main/org.kde.digikam.xml
                         main/digikamapp.h
                         Digikam::DigikamApp)
endif()

set(libdigikamgui_SRCS

    main/digikamapp.cpp
    main/digikamapp_solid.cpp
    main/digikamapp_camera.cpp
    main/digikamapp_import.cpp
    main/digikamapp_config.cpp
    main/digikamapp_tools.cpp
    main/digikamapp_setup.cpp

    date/dpopupframe.cpp
    date/ddateedit.cpp
    date/ddatetable.cpp
    date/ddatetable_p.cpp
    date/ddatepicker.cpp
    date/ddatepicker_p.cpp
    date/ddatetimeedit.cpp
    date/ddatepickerpopup.cpp
    date/datefolderview.cpp
    date/monthwidget.cpp
    date/timelinewidget.cpp

    dragdrop/importdragdrop.cpp
    dragdrop/albumdragdrop.cpp
    dragdrop/ddragobjects.cpp
    dragdrop/itemdragdrop.cpp
    dragdrop/tagdragdrop.cpp

    filters/filtersidebarwidget.cpp
    filters/tagfilterview.cpp

    items/delegate/digikamitemdelegate.cpp
    items/delegate/itemdelegate.cpp
    items/delegate/itemfacedelegate.cpp
    items/views/digikamitemview.cpp
    items/views/digikamitemview_p.cpp
    items/views/itemcategorizedview.cpp
    items/thumbbar/itemthumbnailbar.cpp
    items/thumbbar/itemthumbnaildelegate.cpp
    items/overlays/assignnameoverlay.cpp
    items/overlays/facerejectionoverlay.cpp
    items/overlays/groupindicatoroverlay.cpp
    items/overlays/itemfullscreenoverlay.cpp
    items/overlays/itemratingoverlay.cpp
    items/overlays/itemrotationoverlay.cpp
    items/overlays/itemcoordinatesoverlay.cpp
    items/overlays/itemselectionoverlay.cpp
    items/utils/itemviewutilities.cpp
    items/utils/tooltipfiller.cpp
    items/utils/contextmenuhelper.cpp
    items/utils/groupingviewimplementation.cpp
    items/utils/itemcategorydrawer.cpp

    views/tableview/tableview.cpp
    views/tableview/tableview_treeview.cpp
    views/tableview/tableview_treeview_delegate.cpp
    views/tableview/tableview_column_configuration_dialog.cpp
    views/tableview/tableview_model.cpp
    views/tableview/tableview_columns.cpp
    views/tableview/tableview_column_audiovideo.cpp
    views/tableview/tableview_column_file.cpp
    views/tableview/tableview_column_geo.cpp
    views/tableview/tableview_column_digikam.cpp
    views/tableview/tableview_column_item.cpp
    views/tableview/tableview_column_photo.cpp
    views/tableview/tableview_column_thumbnail.cpp
    views/tableview/tableview_columnfactory.cpp
    views/tableview/tableview_selection_model_syncer.cpp

    views/stack/itemiconview.cpp
    views/stack/itemiconview_albums.cpp
    views/stack/itemiconview_groups.cpp
    views/stack/itemiconview_items.cpp
    views/stack/itemiconview_search.cpp
    views/stack/itemiconview_sidebars.cpp
    views/stack/itemiconview_slideshow.cpp
    views/stack/itemiconview_tags.cpp
    views/stack/itemiconview_tools.cpp
    views/stack/itemiconview_views.cpp
    views/stack/itemiconview_zoom.cpp
    views/stack/trashview.cpp
    views/stack/stackedview.cpp
    views/stack/welcomepageview.cpp
    views/preview/itempreviewcanvas.cpp
    views/preview/itempreviewview.cpp

    views/sidebar/albumfolderviewsidebarwidget.cpp
    views/sidebar/datefolderviewsidebarwidget.cpp
    views/sidebar/timelinesidebarwidget.cpp
    views/sidebar/searchsidebarwidget.cpp
    views/sidebar/fuzzysearchsidebarwidget.cpp
    views/sidebar/labelssidebarwidget.cpp
    views/sidebar/peoplesidebarwidget.cpp
    views/sidebar/tagviewsidebarwidget.cpp
    views/sidebar/sidebarwidget.cpp

    views/utils/dmodelfactory.cpp
    views/utils/slideshowbuilder.cpp
    views/utils/componentsinfodlg.cpp

    ${digikamadaptor_SRCS}
)

if(${Marble_FOUND})
    set(libdigikamgui_SRCS
        ${libdigikamgui_SRCS}
        views/stack/mapwidgetview.cpp
        views/sidebar/gpssearchsidebarwidget.cpp
       )
endif()

# FIXME
#
#if(ENABLE_DIGIKAM_MODELTEST)
#    message(STATUS "Modeltest enabled")
#
#    set(libdigikamgui_SRCS ${libdigikamgui_SRCS}
#        modeltest/modeltest.cpp)
#
#    add_definitions(-DENABLE_DIGIKAM_MODELTEST)
#endif()

add_library(gui_digikam_obj
            OBJECT
            ${libdigikamgui_SRCS}
)

target_compile_definitions(gui_digikam_obj
                           PRIVATE
                           digikamgui_EXPORTS
)

### digiKam GUI shared library objects declaration ##############################################################################

set(DIGIKAMGUI_OBJECTS

            $<TARGET_OBJECTS:gui_digikam_obj>

            # Libs
            $<TARGET_OBJECTS:gui_digikamdatabasemain_obj>
            $<TARGET_OBJECTS:gui_digikamfacesenginedatabase_obj>
            $<TARGET_OBJECTS:gui_digikamdeletedialog_obj>
            $<TARGET_OBJECTS:gui_digikamtemplate_obj>
            $<TARGET_OBJECTS:gui_itempropertiesdigikam_obj>
            $<TARGET_OBJECTS:gui_digikammodels_obj>
            $<TARGET_OBJECTS:gui_digikamalbum_obj>
            $<TARGET_OBJECTS:gui_fileactionmanager_obj>
            $<TARGET_OBJECTS:gui_digikamtags_obj>
            $<TARGET_OBJECTS:gui_digikamsettings_obj>
            $<TARGET_OBJECTS:gui_filters_obj>
            $<TARGET_OBJECTS:gui_imagehistorywidgets_obj>
            $<TARGET_OBJECTS:gui_iojobs_obj>
            $<TARGET_OBJECTS:gui_dtrash_obj>

            # Utilities
            $<TARGET_OBJECTS:gui_setup_obj>
            $<TARGET_OBJECTS:gui_lighttable_obj>
            $<TARGET_OBJECTS:gui_maintenance_obj>
            $<TARGET_OBJECTS:gui_searchwindow_obj>
            $<TARGET_OBJECTS:gui_firstrun_obj>
            $<TARGET_OBJECTS:gui_fuzzysearch_obj>
            $<TARGET_OBJECTS:gui_imageeditorgui_obj>
            $<TARGET_OBJECTS:gui_importui_obj>
            $<TARGET_OBJECTS:gui_importuibackend_obj>
            $<TARGET_OBJECTS:gui_facemanagement_obj>
            $<TARGET_OBJECTS:gui_queuemanager_obj>
            $<TARGET_OBJECTS:gui_advancedrename_obj>
)

if(${Marble_FOUND})

    set(DIGIKAMGUI_OBJECTS
        ${DIGIKAMGUI_OBJECTS}
        $<TARGET_OBJECTS:gui_gpssearch_obj>
    )

endif()

### digiKam GUI shared library target ###########################################################################################

add_library(digikamgui
            SHARED
            ${DIGIKAMGUI_OBJECTS}
)

set_target_properties(digikamgui PROPERTIES
                      VERSION ${DIGIKAM_VERSION_SHORT}
                      SOVERSION ${DIGIKAM_VERSION_SHORT}
)

target_compile_definitions(digikamgui
                           PRIVATE
                           digikamcore_EXPORTS
)

add_dependencies(digikamgui digikamcore digikamdatabase)

# All codes from this target are exported with digikam_gui_export.h header and DIGIKAM_GUI_EXPORT macro.
generate_export_header(digikamgui
                       BASE_NAME digikam_gui
                       EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/utils/digikam_gui_export.h"
)

if(WIN32)
    set_target_properties(digikamgui PROPERTIES COMPILE_FLAGS -DJPEG_STATIC)
endif()

# Note all this target dependencies must be private and not exported to prevent inherited external plugins dependencies.

target_link_libraries(digikamgui

                      PRIVATE

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

                      digikamcore
                      digikamdatabase
)

if(ENABLE_QWEBENGINE)

    target_link_libraries(digikamgui
                          PRIVATE
                          Qt5::WebEngineWidgets
    )

else()

    target_link_libraries(digikamgui
                          PRIVATE
                          Qt5::WebKitWidgets
    )

endif()

if(ENABLE_DBUS)

    target_link_libraries(digikamgui
                          PRIVATE
                          Qt5::DBus
    )

endif()

if(KF5IconThemes_FOUND)

    target_link_libraries(digikamgui
                          PRIVATE
                          KF5::IconThemes
    )

endif()

if(KF5KIO_FOUND)

    target_link_libraries(digikamgui
                          PRIVATE
                          KF5::KIOWidgets
    )

endif()

if(${Marble_FOUND})

     target_link_libraries(digikamgui
                           PRIVATE
                           ${MARBLE_LIBRARIES}
     )

endif()

if(APPLE)

    target_link_libraries(digikamgui
                          PRIVATE
                          /System/Library/Frameworks/AppKit.framework
    )

endif()


if(NOT WIN32)

    # To link under Solaris (see bug #274484)
    target_link_libraries(digikamgui
                          PRIVATE
                          ${MATH_LIBRARY}
    )

endif()

if(Gphoto2_FOUND)

    # See bug #258931: libgphoto2 library must be the last arg for linker.
    # See bug #268267 : digiKam need to be linked to libusb to prevent crash
    # at gphoto2 init if opencv is linked with libdc1394. Libusb linking rules are
    # add to gphoto2 linking rules by Gphoto2 cmake detection script.

    target_link_libraries(digikamgui
                          PRIVATE
                          ${GPHOTO2_LIBRARIES}
    )

endif()

### Install Rules ###############################################################################################################

install(TARGETS digikamgui EXPORT DigikamGuiConfig ${INSTALL_TARGETS_DEFAULT_ARGS})
install(EXPORT DigikamGuiConfig  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/DigikamGui" NAMESPACE Digikam::)

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/DigikamGuiConfigVersion.cmake
                                 VERSION ${DIGIKAM_VERSION_SHORT}
                                 COMPATIBILITY SameMajorVersion)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/DigikamGuiConfigVersion.cmake
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/DigikamGui")
