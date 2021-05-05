#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

### digiKam GUI object library ##################################################################################################

if(ENABLE_DBUS)
    qt5_add_dbus_adaptor(digikamadaptor_SRCS
                         ${CMAKE_CURRENT_SOURCE_DIR}/main/org.kde.digikam.xml
                         ${CMAKE_CURRENT_SOURCE_DIR}/main/digikamapp.h
                         Digikam::DigikamApp)
endif()

set(libdigikamgui_SRCS

    ${CMAKE_CURRENT_SOURCE_DIR}/main/digikamapp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main/digikamapp_solid.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main/digikamapp_camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main/digikamapp_import.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main/digikamapp_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main/digikamapp_tools.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main/digikamapp_setup.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/date/dpopupframe.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/date/ddateedit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/date/ddatetable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/date/ddatetable_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/date/ddatepicker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/date/ddatepicker_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/date/ddatetimeedit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/date/ddatepickerpopup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/date/datefolderview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/date/monthwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/date/timelinewidget.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/dragdrop/importdragdrop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dragdrop/albumdragdrop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dragdrop/ddragobjects.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dragdrop/itemdragdrop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dragdrop/tagdragdrop.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/filters/filtersidebarwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/filters/tagfilterview.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/items/delegate/digikamitemdelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/delegate/itemdelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/delegate/itemfacedelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/views/digikamitemview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/views/digikamitemview_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/views/itemcategorizedview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/thumbbar/itemthumbnailbar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/thumbbar/itemthumbnaildelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/overlays/assignnameoverlay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/overlays/facerejectionoverlay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/overlays/groupindicatoroverlay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/overlays/itemfullscreenoverlay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/overlays/itemratingoverlay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/overlays/itemrotationoverlay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/overlays/itemcoordinatesoverlay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/overlays/itemselectionoverlay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/utils/itemviewutilities.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/utils/tooltipfiller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/utils/contextmenuhelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/utils/groupingviewimplementation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/utils/itemcategorydrawer.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/views/tableview/tableview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/tableview/tableview_model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/tableview/tableview_shared.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/tableview/tableview_treeview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/tableview/tableview_treeview_delegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/tableview/tableview_column_configuration_dialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/tableview/tableview_column_audiovideo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/tableview/tableview_column_file.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/tableview/tableview_column_geo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/tableview/tableview_column_digikam.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/tableview/tableview_column_item.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/tableview/tableview_column_photo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/tableview/tableview_column_thumbnail.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/tableview/tableview_columnfactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/tableview/tableview_selection_model_syncer.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/views/stack/itemiconview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/stack/itemiconview_albums.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/stack/itemiconview_groups.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/stack/itemiconview_items.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/stack/itemiconview_search.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/stack/itemiconview_sidebars.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/stack/itemiconview_tags.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/stack/itemiconview_tools.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/stack/itemiconview_views.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/stack/itemiconview_zoom.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/stack/trashview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/stack/stackedview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/stack/welcomepageview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/preview/itempreviewcanvas.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/preview/itempreviewview.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/views/sidebar/albumfolderviewsidebarwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/sidebar/datefolderviewsidebarwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/sidebar/timelinesidebarwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/sidebar/searchsidebarwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/sidebar/fuzzysearchsidebarwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/sidebar/labelssidebarwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/sidebar/peoplesidebarwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/sidebar/tagviewsidebarwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/sidebar/sidebarwidget.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/views/utils/dmodelfactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/utils/componentsinfodlg.cpp

    ${digikamadaptor_SRCS}
)

if(${Marble_FOUND})
    set(libdigikamgui_SRCS
        ${libdigikamgui_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/views/stack/mapwidgetview.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/views/sidebar/gpssearchsidebarwidget.cpp
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

# NOTE: all this target dependencies must be private and not exported to prevent inherited dependencies on external plugins.

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

                      opencv_core
                      opencv_objdetect
                      opencv_imgproc
                      opencv_imgcodecs
                      opencv_dnn
                      opencv_ml
                      opencv_flann
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

if(APPLE)
    install(FILES "$<TARGET_FILE:digikamgui>.dSYM" DESTINATION "${CMAKE_INSTALL_LIBDIR}" CONFIGURATIONS Debug RelWithDebInfo)
endif()
