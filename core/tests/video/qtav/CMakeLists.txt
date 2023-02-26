#
# SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

APPLY_COMMON_POLICIES()

ecm_add_tests(${CMAKE_CURRENT_SOURCE_DIR}/tests/testdata_utest.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/tests/checkconfig_utest.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/tests/demux_utest.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/tests/audiodecoder_utest.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/tests/videodecoder_utest.cpp

              NAME_PREFIX "qtav-"

              LINK_LIBRARIES
              digikamcore
              ${COMMON_TEST_LINK}
              ${MEDIAPLAYER_LIBRARIES}
              TARGET_NAMES_VAR qtav_utest_targets
    )

foreach(target ${qtav_utest_targets})
    target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/core/tests)
    target_compile_definitions(${target} PRIVATE ${MEDIAPLAYER_DEFINITIONS})
endforeach()

# -------------------------------------------------
# Macro for CLI-style test application
macro(QTAV_TESTS_BUILD)

    set(_filename ${ARGV0})
    string(REPLACE "." ";" _temp ${_filename})
    list(GET _temp 0 _target)

    set(_targetname qtav-${_target}_cli)

    add_executable(${_targetname} ${CMAKE_CURRENT_SOURCE_DIR}/tests/${_filename})
    ecm_mark_nongui_executable(${_targetname})

    target_link_libraries(${_targetname}

                          digikamcore

                          ${COMMON_TEST_LINK}
                          ${MEDIAPLAYER_LIBRARIES}
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
QTAV_TESTS_BUILD(qfileio.cpp)
QTAV_TESTS_BUILD(qrc.cpp)
QTAV_TESTS_BUILD(subtitle.cpp)
QTAV_TESTS_BUILD(transcode.cpp)

add_custom_target(print-codecs ALL DEPENDS ${CMAKE_BINARY_DIR}/bin/qtav-codecslist_cli COMMAND ${CMAKE_BINARY_DIR}/bin/qtav-codecslist_cli)

# -------------------------------------------------

add_executable(qtav-audiopipeline_cli ${CMAKE_CURRENT_SOURCE_DIR}/examples/audiopipeline/main.cpp)
ecm_mark_nongui_executable(qtav-audiopipeline_cli)
target_link_libraries(qtav-audiopipeline_cli digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-audiopipeline_cli PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

add_executable(qtav-framereader_cli ${CMAKE_CURRENT_SOURCE_DIR}/examples/framereader/main.cpp)
ecm_mark_nongui_executable(qtav-framereader_cli)
target_link_libraries(qtav-framereader_cli digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-framereader_cli PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

set(qtavfilters_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/examples/filters/main.cpp
                     ${CMAKE_CURRENT_SOURCE_DIR}/examples/filters/SimpleFilter.cpp
)

if(Qt6_FOUND)

    qt6_add_resources(qtavfilters_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/examples/filters/res.qrc)

else()

    qt5_add_resources(qtavfilters_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/examples/filters/res.qrc)

endif()

add_executable(qtav-filters_gui ${qtavfilters_SRCS})
target_link_libraries(qtav-filters_gui digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-filters_gui PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

set(qtavsharedoutput_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/examples/sharedoutput/main.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/examples/sharedoutput/widget.cpp
)

add_executable(qtav-sharedoutput_gui ${qtavsharedoutput_SRCS})
target_link_libraries(qtav-sharedoutput_gui digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-sharedoutput_gui PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

set(qtavsimpleplayer_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/examples/simpleplayer/main.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/examples/simpleplayer/playerwindow.cpp
)

add_executable(qtav-simpleplayer_gui ${qtavsimpleplayer_SRCS})
target_link_libraries(qtav-simpleplayer_gui digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-simpleplayer_gui PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

add_executable(qtav-glslfilter_gui ${CMAKE_CURRENT_SOURCE_DIR}/examples/glslfilter/main.cpp)
target_link_libraries(qtav-glslfilter_gui digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-glslfilter_gui PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

add_executable(qtav-shader_gui ${CMAKE_CURRENT_SOURCE_DIR}/examples/shader/main.cpp)
target_link_libraries(qtav-shader_gui digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-shader_gui PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

add_executable(qtav-simpletranscode_cli ${CMAKE_CURRENT_SOURCE_DIR}/examples/simpletranscode/main.cpp)
ecm_mark_nongui_executable(qtav-simpletranscode_cli)
target_link_libraries(qtav-simpletranscode_cli digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-simpletranscode_cli PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

set(qtavvideocapture_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/examples/videocapture/main.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/examples/videocapture/playerwindow.cpp
)

add_executable(qtav-videocapture_gui ${qtavvideocapture_SRCS})
target_link_libraries(qtav-videocapture_gui digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-videocapture_gui PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

set(qtavvideogroup_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/examples/videogroup/main.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/examples/videogroup/videogroup.cpp
)

add_executable(qtav-videogroup_gui ${qtavvideogroup_SRCS})
target_link_libraries(qtav-videogroup_gui digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-videogroup_gui PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

set(qtavvideographicsitem_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/examples/videographicsitem/main.cpp
                               ${CMAKE_CURRENT_SOURCE_DIR}/examples/videographicsitem/videoplayer.cpp
)

add_executable(qtav-videographicsitem_gui ${qtavvideographicsitem_SRCS})
target_link_libraries(qtav-videographicsitem_gui digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-videographicsitem_gui PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

set(qtavvideowall_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/examples/videowall/main.cpp
                       ${CMAKE_CURRENT_SOURCE_DIR}/examples/videowall/VideoWall.cpp
)

add_executable(qtav-videowall_gui ${qtavvideowall_SRCS})
target_link_libraries(qtav-videowall_gui digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-videowall_gui PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

add_executable(qtav-window_cli ${CMAKE_CURRENT_SOURCE_DIR}/examples/window/main.cpp)
target_link_libraries(qtav-window_cli digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-window_cli PRIVATE ${MEDIAPLAYER_DEFINITIONS})

add_subdirectory(avplayer)
