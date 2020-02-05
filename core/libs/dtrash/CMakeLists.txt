#
# Copyright (c) 2015 by Mohamed_Anwer, <m_dot_anwer at gmx dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

set(libdtrash_SRCS
    ${CMAKE_SOURCE_DIR}/core/libs/dtrash/dtrash.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dtrash/dtrashiteminfo.cpp
    ${CMAKE_SOURCE_DIR}/core/libs/dtrash/dtrashitemmodel.cpp
)

include_directories(
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
)

# Used by digikamgui
add_library(dtrash_src OBJECT ${libdtrash_SRCS})

target_compile_definitions(dtrash_src
                           PRIVATE
                           digikamcore_EXPORTS
)
