#
# SPDX-FileCopyrightText: 2010-2023 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

APPLY_COMMON_POLICIES()

include(CMakeParseArguments)

# -------------------------------------------------------------------------

# This macro implement the rules to compile and link a marble plugin with extra arguments.
#
# Usage: marble_add_plugin(NAME    _plugin_name_
#                          SOURCE  _plugin_sources_
#                          DEPENDS _plugin_dependencies_)
#
# With: _plugin_name_ the literal name of the plugin (mandatory).
#       _plugin_sources_ the list of source codes to compile (mandatory).
#       _plugin_dependencies_ the list of dependencies to link (optional).
#
macro(marble_add_plugin)

    set(_OPTIONS_ARGS)
    set(_ONE_VALUE_ARGS)
    set(_MULTI_VALUE_ARGS NAME SOURCES DEPENDS)

    cmake_parse_arguments(_parse_results "${_OPTIONS_ARGS}"
                                         "${_ONE_VALUE_ARGS}"
                                         "${_MULTI_VALUE_ARGS}"
                                         ${ARGN}
    )

    # Mandatory
    if(NOT _parse_results_NAME)
        message(FATAL_ERROR "Marble plugin name is required.")
    endif()

    if(NOT _parse_results_SOURCES)
        message(FATAL_ERROR "Marble plugin sources is required.")
    endif()

    if(APPLE)
        set(_extra_deps /System/Library/Frameworks/AppKit.framework)
    endif()

    add_library(${_parse_results_NAME}
                MODULE ${_parse_results_SOURCES})

    target_link_libraries(${_parse_results_NAME}

                          PRIVATE

                          digikamcore

                          Qt${QT_VERSION_MAJOR}::Core
                          Qt${QT_VERSION_MAJOR}::Gui
                          Qt${QT_VERSION_MAJOR}::Xml
                          Qt${QT_VERSION_MAJOR}::Widgets
                          Qt${QT_VERSION_MAJOR}::Network
                          Qt${QT_VERSION_MAJOR}::Svg
                          Qt${QT_VERSION_MAJOR}::SvgWidgets

                          KF${QT_VERSION_MAJOR}::I18n

                          PNG::PNG                          # For zlib

                          ${_parse_results_DEPENDS}
                          ${_extra_deps}
    )

    install(TARGETS ${_parse_results_NAME}
            DESTINATION ${KDE_INSTALL_FULL_PLUGINDIR}/digikam/marble
    )

    # Install debug symbols

    if(MSVC)
        install(FILES "$<TARGET_PDB_FILE:${_parse_results_NAME}"
                DESTINATION ${KDE_INSTALL_FULL_PLUGINDIR}/digikam/marble
                CONFIGURATIONS Debug RelWithDebInfo
        )
    endif()

    if(APPLE)
        install(FILES "$<TARGET_FILE:${_parse_results_NAME}>.dSYM"
                DESTINATION ${KDE_INSTALL_FULL_PLUGINDIR}/digikam/marble
                CONFIGURATIONS Debug RelWithDebInfo
        )
    endif()

endmacro()

add_subdirectory(render)
add_subdirectory(runner)
