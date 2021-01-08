#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(libqueuemanager_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/actionthread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/batchtool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/batchtoolutils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/batchtoolsfactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/queuesettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/workflowmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dplugins/dpluginbqm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dplugins/dpluginconfviewbqm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/workflowdlg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/assignedlist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/queuelist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/queuepool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/queuesettingsview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/queuetooltip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/toolsettingsview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/toolslistview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/toolsview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/views/workflowlist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main/queuemgrwindow.cpp
)

if(Gphoto2_FOUND)
    include_directories(${GPHOTO2_INCLUDE_DIRS})
endif()

include_directories($<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::WidgetsAddons,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::Service,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::Solid,INTERFACE_INCLUDE_DIRECTORIES>
)

add_library(gui_queuemanager_obj OBJECT ${libqueuemanager_SRCS} ${libdlib_SRCS})

# Used by digikamgui
target_compile_definitions(gui_queuemanager_obj
                           PRIVATE
                           digikamgui_EXPORTS
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/main/queuemgrwindowui5.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/digikam)
