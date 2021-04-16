#
# Copyright (c) 2021      by Joerg Lohse <joergmlpts at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include(MacroDPlugins)

include_directories($<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

if(ENABLE_QWEBENGINE)
    include_directories($<TARGET_PROPERTY:Qt5::WebEngineWidgets,INTERFACE_INCLUDE_DIRECTORIES>)
else()
    include_directories($<TARGET_PROPERTY:Qt5::WebKitWidgets,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

# --------------------------------------------------------------------------------

set(inatplugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/inatbrowserdlg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inatplugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inatscore.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inatsuggest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inattalker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inattaxon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inattaxonedit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inatwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inatwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inatutils.cpp
)

DIGIKAM_ADD_GENERIC_PLUGIN(NAME    iNaturalist
                           SOURCES ${inatplugin_SRCS}
                           DEPENDS ${QWEB_BACKEND}

)

add_subdirectory(icons)
