#
# Copyright (c) 2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include(MacroDPlugins)

include_directories($<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Network,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Concurrent,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Xml,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
)

set(libmjpegstream_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/mjpegstreamdlg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mjpegstreamdlg_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mjpegframethread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mjpegframeosd.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mjpegserver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mjpegserver_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mjpegservermngr.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mjpegstreamsettings.cpp
)

# For Unit-tests and plugin
add_library(mjpegstreambackend STATIC ${libmjpegstream_SRCS})

if(MSVC)

    target_compile_options(mjpegstreambackend PRIVATE /w)

else()

    target_compile_options(mjpegstreambackend PRIVATE -w)

endif()

target_link_libraries(mjpegstreambackend
                      PRIVATE
                      digikamcore
)

if(WIN32)

    target_link_libraries(mjpegstreambackend
                          PRIVATE
                          wsock32
                          ws2_32
    )

endif()

# ---------------------------------------------------------------------------------------------------

set(mjpegstreamplugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/mjpegstreamplugin.cpp
)

DIGIKAM_ADD_GENERIC_PLUGIN(NAME    MjpegStream
                           SOURCES ${mjpegstreamplugin_SRCS}
                           DEPENDS mjpegstreambackend
)
