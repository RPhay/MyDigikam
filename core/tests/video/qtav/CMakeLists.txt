#
# Copyright (c) 2010-2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

# -------------------------------------------------

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

# -------------------------------------------------

set(qtavplayer_SRCS ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/utils/ClickableMenu.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/utils/EventFilter.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/utils/Slider.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/utils/StatisticsView.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/utils/ScreenSaver.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/utils/qoptions.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/utils/Config.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/utils/common.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/config/VideoEQConfigPage.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/config/PropertyEditor.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/config/DecoderConfigPage.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/config/ConfigDialog.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/config/AVFormatConfigPage.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/config/ShaderPage.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/config/MiscPage.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/config/ConfigPageBase.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/config/CaptureConfigPage.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/config/AVFilterConfigPage.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/filters/OSDFilter.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/filters/OSD.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/filters/AVFilterSubtitle.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/playlist/PlayListModel.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/playlist/PlayListItem.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/playlist/PlayListDelegate.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/playlist/PlayList.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/main.cpp
                    ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/MainWindow.cpp
)

if(Qt6_FOUND)

    qt6_add_resources(qtavplayer_SRCS ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/theme/theme.qrc)

else()

    qt5_add_resources(qtavplayer_SRCS ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/player/theme/theme.qrc)

endif()

add_executable(qtav-player_gui ${qtavplayer_SRCS})

target_link_libraries(qtav-player_gui
                      digikamcore
                      ${COMMON_TEST_LINK}
                      ${MEDIAPLAYER_LIBRARIES}
)

target_compile_definitions(qtav-player_gui
                           PRIVATE
                           ${MEDIAPLAYER_DEFINITIONS}
)

# -------------------------------------------------

add_executable(qtav-audiopipeline_cli ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/audiopipeline/main.cpp)
ecm_mark_nongui_executable(qtav-audiopipeline_cli)
target_link_libraries(qtav-audiopipeline_cli digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-audiopipeline_cli PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

add_executable(qtav-framereader_cli ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/framereader/main.cpp)
ecm_mark_nongui_executable(qtav-framereader_cli)
target_link_libraries(qtav-framereader_cli digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-framereader_cli PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

set(qtavfilters_SRCS ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/filters/main.cpp
                     ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/filters/SimpleFilter.cpp
)

if(Qt6_FOUND)

    qt6_add_resources(qtavfilters_SRCS ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/filters/res.qrc)

else()

    qt5_add_resources(qtavfilters_SRCS ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/filters/res.qrc)

endif()

add_executable(qtav-filters_gui ${qtavfilters_SRCS})
target_link_libraries(qtav-filters_gui digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-filters_gui PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

set(qtavsharedoutput_SRCS ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/sharedoutput/main.cpp
                          ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/sharedoutput/widget.cpp
)

add_executable(qtav-sharedoutput_gui ${qtavsharedoutput_SRCS})
target_link_libraries(qtav-sharedoutput_gui digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-sharedoutput_gui PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

set(qtavsimpleplayer_SRCS ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/simpleplayer/main.cpp
                          ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/simpleplayer/playerwindow.cpp
)

add_executable(qtav-simpleplayer_gui ${qtavsimpleplayer_SRCS})
target_link_libraries(qtav-simpleplayer_gui digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-simpleplayer_gui PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

add_executable(qtav-glslfilter_gui ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/glslfilter/main.cpp)
target_link_libraries(qtav-glslfilter_gui digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-glslfilter_gui PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

add_executable(qtav-shader_gui ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/shader/main.cpp)
target_link_libraries(qtav-shader_gui digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-shader_gui PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

add_executable(qtav-simpletranscode_cli ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/simpletranscode/main.cpp)
ecm_mark_nongui_executable(qtav-simpletranscode_cli)
target_link_libraries(qtav-simpletranscode_cli digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-simpletranscode_cli PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

set(qtavvideocapture_SRCS ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/videocapture/main.cpp
                          ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/videocapture/playerwindow.cpp
)

add_executable(qtav-videocapture_gui ${qtavvideocapture_SRCS})
target_link_libraries(qtav-videocapture_gui digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-videocapture_gui PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

set(qtavvideogroup_SRCS ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/videogroup/main.cpp
                        ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/videogroup/videogroup.cpp
)

add_executable(qtav-videogroup_gui ${qtavvideogroup_SRCS})
target_link_libraries(qtav-videogroup_gui digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-videogroup_gui PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

set(qtavvideographicsitem_SRCS ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/videographicsitem/main.cpp
                               ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/videographicsitem/videoplayer.cpp
)

add_executable(qtav-videographicsitem_gui ${qtavvideographicsitem_SRCS})
target_link_libraries(qtav-videographicsitem_gui digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-videographicsitem_gui PRIVATE ${MEDIAPLAYER_DEFINITIONS})

# -------------------------------------------------

add_executable(qtav-window_cli ${CMAKE_SOURCE_DIR}/core/libs/video/qtav/examples/window/main.cpp)
target_link_libraries(qtav-window_cli digikamcore ${COMMON_TEST_LINK} ${MEDIAPLAYER_LIBRARIES})
target_compile_definitions(qtav-window_cli PRIVATE ${MEDIAPLAYER_DEFINITIONS})
