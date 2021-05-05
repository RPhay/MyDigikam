#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories($<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::Solid,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::Service,INTERFACE_INCLUDE_DIRECTORIES>
)

if(KF5KIO_FOUND)
    include_directories($<TARGET_PROPERTY:KF5::KIOWidgets,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

if(Gphoto2_FOUND)
    include_directories(${GPHOTO2_INCLUDE_DIRS})
endif()

set(libimportuibackend_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/cameracontroller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/camerathumbsctrl.cpp
#   ${CMAKE_CURRENT_SOURCE_DIR}/backend/camerahistoryupdater.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/dkcamera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/gpcamera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/camiteminfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/umscamera.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/main/importsettings.cpp
)

set(libimportui_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/camerafolderdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/camerainfodialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/capturedlg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/cameramessagebox.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/capturewidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/freespacewidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/renamecustomizer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/albumcustomizer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/advancedsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/scriptingsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/filter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/importfiltercombobox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/importcontextmenu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/dngconvertsettings.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/models/importimagemodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/models/importthumbnailmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/models/importfiltermodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/models/camitemsortsettings.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/items/camerafolderitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/itemviewimportdelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/importcategorydrawer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/importdelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/importtooltipfiller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/items/importoverlays.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/views/camerafolderview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/cameranamehelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/freespacetooltip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/importcategorizedview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/importthumbnailbar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/importpreviewview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/importiconview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/importiconview_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/importstackedview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/importview.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/main/importui.cpp
)

# Used by digikamgui
add_library(gui_importuibackend_obj OBJECT ${libimportuibackend_SRCS})

target_compile_definitions(gui_importuibackend_obj
                           PRIVATE
                           digikamgui_EXPORTS
)

# Used by digikamgui
add_library(gui_importui_obj OBJECT ${libimportui_SRCS})

target_compile_definitions(gui_importui_obj
                           PRIVATE
                           digikamgui_EXPORTS
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/main/importui5.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/digikam)
