#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories($<TARGET_PROPERTY:Qt5::PrintSupport,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Network,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::Service,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::Solid,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::WindowSystem,INTERFACE_INCLUDE_DIRECTORIES>
)

if(KF5KIO_FOUND)
    include_directories($<TARGET_PROPERTY:KF5::KIOWidgets,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

if(Gphoto2_FOUND)
    include_directories(${GPHOTO2_INCLUDE_DIRS})
endif()

# ------------------------------------------------------------------------------------

set(libeditorwidgets_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/imageguidewidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/imagepreviewitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/previewtoolbar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/previewlist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/imageregionwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/imageregionitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/rubberitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/canvas.cpp
)

set(libeditordlg_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/colorcorrectiondlg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/softproofdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs/versioningpromptusersavedlg.cpp
)

set(libeditorcore_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/core/undocache.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/undoaction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/undomanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/editorcore.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/iccpostloadingmanager.cpp
)

set(libeditoriface_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/editor/editortool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/editor/editortooliface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/editor/editorstackview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/editor/editortoolsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/editor/editorwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/editor/imageiface.cpp
)

# Used by digikamcore
add_library(core_imageeditor_obj OBJECT ${libeditorcore_SRCS}
                                        ${libeditordlg_SRCS}
                                        ${libeditoriface_SRCS}
                                        ${libeditorwidgets_SRCS}
)

target_compile_definitions(core_imageeditor_obj
                           PRIVATE
                           digikamcore_EXPORTS
)

# ------------------------------------------------------------------------------------

set(libeditorgui_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/main/imagewindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main/imagewindow_setup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main/imagewindow_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main/imagewindow_import.cpp
)

# Used by digikamgui
add_library(gui_imageeditorgui_obj OBJECT ${libeditorgui_SRCS})

target_compile_definitions(gui_imageeditorgui_obj
                           PRIVATE
                           digikamgui_EXPORTS
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/main/imageeditorui5.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/digikam)
