#
# SPDX-FileCopyrightText: 2010-2023 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

macro(marble_add_plugin _target_name)

add_library(${_target_name} MODULE ${ARGN})

target_link_libraries(${_target_name}

                      PRIVATE

                      digikamcore

                      Qt${QT_VERSION_MAJOR}::Core
                      Qt${QT_VERSION_MAJOR}::Gui
                      Qt${QT_VERSION_MAJOR}::Xml
                      Qt${QT_VERSION_MAJOR}::Svg
                      Qt${QT_VERSION_MAJOR}::SvgWidgets
                      Qt${QT_VERSION_MAJOR}::Widgets
                      Qt${QT_VERSION_MAJOR}::Network

                      KF${QT_VERSION_MAJOR}::I18n
)

install(TARGETS ${_target_name}
        DESTINATION ${KDE_INSTALL_FULL_PLUGINDIR}/digikam/marble
)

endmacro()

add_subdirectory(render)
add_subdirectory(runner)
