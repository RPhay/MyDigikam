#
# SPDX-FileCopyrightText: 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

APPLY_COMMON_POLICIES()

include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::Service,INTERFACE_INCLUDE_DIRECTORIES>
)

###

set(libvideotoolscommon_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/vidslidesettings.cpp
)

# Used by digikamcore
add_library(core_videotoolscommon_obj OBJECT ${libvideotoolscommon_SRCS})

target_compile_definitions(core_videotoolscommon_obj
                           PRIVATE
                           digikamcore_EXPORTS
)

###

# NOTE: this part depends of QtAv dependency

if(ENABLE_MEDIAPLAYER)

    set(libvideotools_SRCS

        ${CMAKE_CURRENT_SOURCE_DIR}/manager/vidslidethread.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/manager/vidslidetask.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/player/vidplayerdlg.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/player/mediaplayerview.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/player/slidevideo.cpp
    )

    # Used by digikamcore
    add_library(core_videotools_obj OBJECT ${libvideotools_SRCS})

    target_compile_definitions(core_videotools_obj
                               PRIVATE
                               digikamcore_EXPORTS
    )

    target_compile_definitions(core_videotools_obj
                               PRIVATE
                               ${MEDIAPLAYER_DEFINITIONS}
    )

    # ---

    add_subdirectory(qtav)

endif()
