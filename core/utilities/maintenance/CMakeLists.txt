#
# SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# SPDX-FileCopyrightText: 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

APPLY_COMMON_POLICIES()

set(libmaintenance_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/maintenancedata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/iteminfojob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/iteminfoalbumsjob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metadatatask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metadatasynchronizer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/duplicatesfinder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/databasetask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dbcleaner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/newitemsfinder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thumbsgenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thumbstask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/facesdetector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fingerprintsgenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fingerprintstask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imagequalitysorter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imagequalitytask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/maintenancedlg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/maintenancemngr.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/maintenancetool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/maintenancesettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/maintenancethread.cpp
)

include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Sql,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::Solid,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
)

# Used by digikamgui
add_library(gui_maintenance_obj OBJECT ${libmaintenance_SRCS})

target_compile_definitions(gui_maintenance_obj
                           PRIVATE
                           digikamgui_EXPORTS
)
