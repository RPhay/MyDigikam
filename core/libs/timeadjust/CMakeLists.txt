#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2018      by Maik Qualmann, <metzpinguin at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories($<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

set(libtimeadjust_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/clockphotodialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/detbyclockphotobutton.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/timeadjustcontainer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/timeadjustsettings.cpp
)

# Used by digikamcore
add_library(core_timeadjust_obj OBJECT ${libtimeadjust_SRCS})

target_compile_definitions(core_timeadjust_obj
                           PRIVATE
                           digikamcore_EXPORTS
)
