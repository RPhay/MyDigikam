#
# SPDX-FileCopyrightText: 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

APPLY_COMMON_POLICIES()

kde_enable_exceptions()

# ---------------------------------------------------------------------------

include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Xml,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Sql,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Concurrent,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>

                    ${CMAKE_CURRENT_SOURCE_DIR}/model
)

# ---------------------------------------------------------------------------

set(autotagassignment_LIB_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/model/dnnbasedetectormodel.cpp
                               ${CMAKE_CURRENT_SOURCE_DIR}/model/dnnyolodetector.cpp
                               ${CMAKE_CURRENT_SOURCE_DIR}/autotagsassign.cpp
)

# Used by digikamcore
add_library(core_digikamautotagassignment_obj
            OBJECT
            ${autotagassignment_LIB_SRCS}
)

target_compile_definitions(core_digikamautotagassignment_obj
                           PRIVATE
                           digikamcore_EXPORTS
)

# For unit-tests
add_library(digikamautotagassignment STATIC $<TARGET_OBJECTS:core_digikamautotagassignment_obj>)