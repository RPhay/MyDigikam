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

            find_package(VAAPI      QUIET)

            if(WIN32)

                set(DirectX_FIND_REQUIRED_D3D11 TRUE)
                find_package(DirectX)
                find_package(GDIPLUS)

            endif()

            MACRO_BOOL_TO_01(VAAPI_FOUND           HAVE_LIBVAAPI)
            MACRO_BOOL_TO_01(DirectX_D3D11_FOUND   HAVE_LIBD3D11)
            MACRO_BOOL_TO_01(DirectX_DSound_FOUND  HAVE_LIBDIRECTSOUND)
            MACRO_BOOL_TO_01(DirectX_XAudio2_FOUND HAVE_LIBXAUDIO2)
            MACRO_BOOL_TO_01(GDIPLUS_FOUND         HAVE_LIBGDIPLUS)

            # --- Reports and Libraries -----------------------------------------------------------

            if(VAAPI_FOUND)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${VAAPI_LIBRARIES})
                message(STATUS "MediaPlayer will be compiled with VaAPI support      : yes")

            else()

                message(STATUS "MediaPlayer will be compiled with VaAPI support      : no")

            endif()

            if(X11_FOUND)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${X11_LIBRARIES})
                message(STATUS "MediaPlayer will be compiled with X11 support        : yes")

            else()

                message(STATUS "MediaPlayer will be compiled with X11 support        : no")

            endif()

            if(X11_Xv_FOUND)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${X11_Xv_LIB})
                message(STATUS "MediaPlayer will be compiled with XV support         : yes")

            else()

                message(STATUS "MediaPlayer will be compiled with XV support         : no")

            endif()

            if(OPENGL_FOUND)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${OPENGL_LIBRARIES} Qt${QT_VERSION_MAJOR}::OpenGL)

                if(Qt6_FOUND)

                    set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} Qt${QT_VERSION_MAJOR}::OpenGLWidgets)

                endif()

                message(STATUS "MediaPlayer will be compiled with OpenGL support     : yes")

                if(OpenGL_EGL_FOUND)

                    set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${OPENGL_egl_LIBRARY})
                    message(STATUS "MediaPlayer will be compiled with OpenGL-EGL support : yes")

                else()

                    message(STATUS "MediaPlayer will be compiled with OpenGL-EGL support : no")

                endif()

            else()

                message(STATUS "MediaPlayer will be compiled with OpenGL support     : no")
                message(STATUS "MediaPlayer will be compiled with OpenGL-EGL support : no")

            endif()

            # --- Windows config ---

            if(GDIPLUS_FOUND)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${GDIPLUS_LIBRARIES})
                message(STATUS "MediaPlayer will be compiled with GDI+ support       : yes")

            else()

                message(STATUS "MediaPlayer will be compiled with GDI+ support       : no")

            endif()

            if(DIRECTX_FOUND)

                set(MEDIAPLAYER_LIBRARIES ${MEDIAPLAYER_LIBRARIES} ${DIRECT3D_LIBRARIES})

                if(DirectX_D3D11_FOUND)

                    message(STATUS "MediaPlayer will be compiled with Direct3D11 support : yes")

                else()

                    message(STATUS "MediaPlayer will be compiled with Direct3D11 support : no")

                endif()


                if(DirectX_DSound_FOUND)

                    message(STATUS "MediaPlayer will be compiled with DirectSound support: yes")

                else()

                    message(STATUS "MediaPlayer will be compiled with DirectSound support: no")

                endif()

                if(DirectX_XAudio2_FOUND)

                    message(STATUS "MediaPlayer will be compiled with XAudio2 support    : yes")

                else()

                    message(STATUS "MediaPlayer will be compiled with XAudio2 support    : no")

                endif()

            else()

                message(STATUS "MediaPlayer will be compiled with Direct3D11 support : no")
                message(STATUS "MediaPlayer will be compiled with DirectSound support: no")
                message(STATUS "MediaPlayer will be compiled with XAudio2 support    : no")

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
