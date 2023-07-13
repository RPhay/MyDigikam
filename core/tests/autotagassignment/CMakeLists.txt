#
# SPDX-FileCopyrightText: 2010-2021, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#


APPLY_COMMON_POLICIES()


include_directories(
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Network,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Concurrent,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Xml,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::Service,INTERFACE_INCLUDE_DIRECTORIES>
)

# -----------------------------------------------------------------------------

set(detect_object_cli_SRCS  ${CMAKE_CURRENT_SOURCE_DIR}/detect_object_cli.cpp)

add_executable(detect_object_cli ${detect_object_cli_SRCS})

target_link_libraries(detect_object_cli
                      
                      digikamcore
                      digikamdatabase
                      digikamgui

                      KF5::XmlGui
                      KF5::I18n
                      KF5::ConfigCore
                      KF5::Service
                      ${COMMON_TEST_LINK}
)

# -----------------------------------------------------------------------------

set(detect_object_gui_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/detect_object_gui.cpp)

add_executable(detect_object_gui ${detect_object_gui_SRCS})

target_link_libraries(detect_object_gui

                      digikamcore
                      digikamdatabase
                      digikamgui

                      KF5::XmlGui
                      KF5::I18n
                      KF5::ConfigCore
                      KF5::Service
                      ${COMMON_TEST_LINK}
)
