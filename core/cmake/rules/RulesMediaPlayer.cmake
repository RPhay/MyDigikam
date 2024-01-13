#
# SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

if(ENABLE_MEDIAPLAYER)

    # NOTE: For Qt5, QtMultimedia is used to render video on widget.
    # NOTE: For Qt6, QtMultimedia based on FFMpeg is only supported with version > 6.5.

    find_package(Qt${QT_VERSION_MAJOR}
                 OPTIONAL_COMPONENTS
                 Multimedia
                 MultimediaWidgets
    )

    include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Multimedia,INTERFACE_INCLUDE_DIRECTORIES>
                        $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::MultimediaWidgets,INTERFACE_INCLUDE_DIRECTORIES>)

    if(Qt6_FOUND)

        include_directories(${CMAKE_SOURCE_DIR}/core/libs/video/player/qtmm)


        set(ENABLE_QTMULTIMEDIA ON)
        message(STATUS "MediaPlayer type:     Qt6::Multimedia")

    else()

        set(ENABLE_QTMULTIMEDIA OFF)

        if(NOT ${FFMPEG_FOUND})

            set(ENABLE_MEDIAPLAYER OFF)
            message(STATUS "MediaPlayer type:     None")

        else()

            message(STATUS "--------------------------------------------------")
            message(STATUS "QtAV dependencies checks:")
            message(STATUS "")

            find_package(VAAPI QUIET)

            if(WIN32)

                set(DirectX_FIND_REQUIRED_D3D11 TRUE)
                find_package(DirectX)

            endif()

            MACRO_BOOL_TO_01(VAAPI_FOUND           HAVE_LIBVAAPI)
            MACRO_BOOL_TO_01(DirectX_D3D11_FOUND   HAVE_LIBD3D11)

            # --- Reports and Libraries -----------------------------------------------------------

            if(VAAPI_FOUND)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${VAAPI_LIBRARIES})

                if(VAAPI_X11_FOUND AND X11_FOUND)

                    set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${X11_LIBRARIES})

                    message(STATUS "MediaPlayer will be compiled with VaAPI X11 support  : yes")

                else()

                    set(VAAPI_X11_FOUND FALSE)
                    message(STATUS "MediaPlayer will be compiled with VaAPI X11 support  : no")

                endif()

                if(VAAPI_DRM_FOUND AND OPENSLES_FOUND)

                    set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${OPENSLES_LIBRARIES})

                    message(STATUS "MediaPlayer will be compiled with VaAPI DRM support  : yes")

                else()

                    set(VAAPI_DRM_FOUND FALSE)
                    message(STATUS "MediaPlayer will be compiled with VaAPI DRM support  : no")

                endif()

            endif()

            # --- Windows config ---

            if(DIRECTX_FOUND)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${DIRECT3D_LIBRARIES})

                if(DirectX_D3D11_FOUND)

                    message(STATUS "MediaPlayer will be compiled with Direct3D11 support : yes")

                else()

                    message(STATUS "MediaPlayer will be compiled with Direct3D11 support : no")

                endif()

            else()

                message(STATUS "MediaPlayer will be compiled with Direct3D11 support : no")

            endif()

            # --- MacOS config ---

            if(APPLE)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} "-framework CoreMedia"
                                                                   "-framework CoreVideo"
                                                                   "-framework VideoToolbox"
                                                                   "-framework AudioToolbox"
                                                                   "-framework CoreFoundation"
                                                                   "-framework VideoDecodeAcceleration"
                )

                if(AVDEVICE_FOUND)

                    set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} "-framework Foundation"
                                                                       "-framework QuartzCore"
                                                                       "-framework CoreGraphics"
                                                                       "-framework AVFoundation"
                    )

                endif()

                if(AVFILTER_FOUND)

                    set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} "-framework AppKit")

                endif()

            endif()

            # --- Resume ---------------------------------------------------------------------------------------

            set(MEDIAPLAYER_DEFINITIONS ${MEDIAPLAYER_DEFINITIONS} -DQT_AVPLAYER_MULTIMEDIA)

            message(STATUS "MediaPlayer type       : QtAVPlayer")
            message(STATUS "MediaPlayer libraries  : ${MEDIAPLAYER_LIBRARIES}")
            message(STATUS "MediaPlayer definitions: ${MEDIAPLAYER_DEFINITIONS}")
            message(STATUS "MediaPlayer flags      : ${MEDIAPLAYER_FLAGS}")

            include_directories(${CMAKE_SOURCE_DIR}/core/libs/video/QtAVPlayer)
            include_directories(${CMAKE_SOURCE_DIR}/core/libs/video/player/QtAVPlayer)

        endif()

    endif()

endif()
