#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories(
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Test,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Xml,INTERFACE_INCLUDE_DIRECTORIES>
)

if(HAVE_OPENGL)

    include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::OpenGL,INTERFACE_INCLUDE_DIRECTORIES>)

    if(Qt6_FOUND)

        include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::OpenGLWidgets,INTERFACE_INCLUDE_DIRECTORIES>)

    endif()

endif()

# -------------------------------------------------

add_executable(framesencoder_cli ${CMAKE_CURRENT_SOURCE_DIR}/framesencoder_cli.cpp)
ecm_mark_nongui_executable(framesencoder_cli)

target_link_libraries(framesencoder_cli

                      digikamcore

                      ${COMMON_TEST_LINK}
)


# -------------------------------------------------

add_executable(player_gui ${CMAKE_CURRENT_SOURCE_DIR}/player_gui.cpp)

target_link_libraries(player_gui

                      digikamcore

                      ${COMMON_TEST_LINK}
)

# -------------------------------------------------

add_executable(videothumb_cli ${CMAKE_CURRENT_SOURCE_DIR}/videothumb_cli.cpp)
ecm_mark_nongui_executable(videothumb_cli)

target_link_libraries(videothumb_cli
                      digikamcore

                      ${COMMON_TEST_LINK}
)

# -------------------------------------------------

macro(LIBRAW_BUILD_SAMPLES)

    set(_filename ${ARGV0})
    string(REPLACE "." ";" _temp ${_filename})
    list(GET _temp 0 _target)

    set(${_target}_SRCS ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/tests/${_filename})

    add_executable(qtav_${_target}_cli ${${_target}_SRCS})

    target_link_libraries(qtav_${_target}_cli

                          digikamcore

                          ${COMMON_TEST_LINK}
    )

    target_compile_definitions(qtav_${_target}_cli
                               PRIVATE
                               ${MEDIAPLAYER_DEFINITIONS}
    )

endmacro()

macro(QTAV_TESTS_BUILD)

    set(_filename ${ARGV0})
    string(REPLACE "." ";" _temp ${_filename})
    list(GET _temp 0 _target)

    set(_targetname qtav-${_target}_cli)

    add_executable(${_targetname} ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/tests/${_filename})
    ecm_mark_nongui_executable(${_targetname})

    target_link_libraries(${_targetname}

                          digikamcore

                          ${COMMON_TEST_LINK}
    )

    target_compile_definitions(${_targetname}
                               PRIVATE
                               ${MEDIAPLAYER_DEFINITIONS}
    )

endmacro()

QTAV_TESTS_BUILD(codecslist.cpp)
QTAV_TESTS_BUILD(ao.cpp)
QTAV_TESTS_BUILD(decoder.cpp)
QTAV_TESTS_BUILD(extract.cpp)
QTAV_TESTS_BUILD(playerthread.cpp)
QTAV_TESTS_BUILD(qiodevice.cpp)
QTAV_TESTS_BUILD(qrc.cpp)
QTAV_TESTS_BUILD(subtitle.cpp)
QTAV_TESTS_BUILD(transcode.cpp)
