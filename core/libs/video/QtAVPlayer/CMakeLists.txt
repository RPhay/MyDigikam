#
# SPDX-FileCopyrightText: 2010-2024, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

APPLY_COMMON_POLICIES()

# The QtAVPlayer library does not adhere to the flags we use
remove_definitions(
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_DISABLE_DEPRECATED_BEFORE=0x050E00
    -DQT_NO_KEYWORDS
)

include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>
                    ${DIRECTX_LIBRARY_DIRS}             # Populated under Windows, else empty
)

set(libqtavplayer_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/qavaudiocodec.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavaudiofilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavaudioframe.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavaudioinputfilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavaudiooutput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavaudiooutputfilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavcodec.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavdemuxer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavfilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavfiltergraph.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavfilters.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavframecodec.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavframe.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavinoutfilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qaviodevice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavpacket.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavplayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavstream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavstreamframe.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavsubtitlecodec.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavsubtitleframe.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavvideobuffer_cpu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavvideobuffer_gpu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavvideocodec.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavvideofilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavvideoframe.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavvideoinputfilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qavvideooutputfilter.cpp

#   ANDROID
#
#   ${CMAKE_CURRENT_SOURCE_DIR}/qavandroidsurfacetexture.cpp
#   ${CMAKE_CURRENT_SOURCE_DIR}/qavhwdevice_mediacodec.cpp

#   VDPAU API (https://en.wikipedia.org/wiki/VDPAU)
#
#   ${CMAKE_CURRENT_SOURCE_DIR}/qavhwdevice_vdpau.cpp

)

if(DirectX_D3D11_FOUND)

    set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/qavhwdevice_d3d11.cpp
    )

endif()

if(VAAPI_FOUND)

    include_directories(${VAAPI_INCLUDE_DIRS})

    set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/qavhwdevice_vaapi_drm_egl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/qavhwdevice_vaapi_x11_glx.cpp
    )

endif()

if(APPLE)

    set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/qavhwdevice_videotoolbox.mm
    )

endif()

# -------------------------------------------------------------------------

# Used by digikamcore
add_library(core_qtavplayer_obj OBJECT ${libqtavplayer_SRCS})

target_compile_definitions(core_qtavplayer_obj
                           PRIVATE
                           digikamcore_EXPORTS
)

target_compile_definitions(core_qtavplayer_obj
                           PRIVATE
                           ${MEDIAPLAYER_DEFINITIONS}
)

target_compile_options(core_qtavplayer_obj
                       PRIVATE
                       ${MEDIAPLAYER_FLAGS}
)

# -------------------------------------------------------------------------


#[[
if(SSE4_1_FOUND)

    set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/GPUMemCopy_Frame_SSE4.cpp
    )

endif()

if(SSE2_FOUND)

    set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/GPUMemCopy_Frame_SSE2.cpp
    )

endif()

if(Qt6_FOUND)

    qt6_add_resources(libqtavplayer_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shaders.qrc)

else()

    qt5_add_resources(libqtavplayer_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shaders.qrc)

endif()

if(DirectX_DSound_FOUND)

    set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/output/audio/AudioOutputDSound.cpp
    )

endif()

if(DirectX_XAudio2_FOUND)

    set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/output/audio/AudioOutputXAudio2.cpp
    )

endif()


if(X11_FOUND)

    set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/filter/X11FilterContext.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/X11/X11Renderer.cpp
    )

endif()

if(X11_Xv_FOUND)

    set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/X11/XVRenderer.cpp
    )

endif()

if(OPENGL_FOUND)

    include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::OpenGL,INTERFACE_INCLUDE_DIRECTORIES>)

    if(Qt6_FOUND)

        include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::OpenGLWidgets,INTERFACE_INCLUDE_DIRECTORIES>)

    endif()

    set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/opengl/gl_api.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/opengl/ConvolutionShader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/opengl/OpenGLVideo.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/opengl/SubImagesRenderer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/opengl/Geometry.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/opengl/OpenGLHelper.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/opengl/ShaderManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/opengl/VideoShader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/opengl/GeometryRenderer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/opengl/OpenGLTypes.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/opengl/SubImagesGeometry.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/opengl/VideoShaderObject.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/filter/GLSLFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/output/video/OpenGLRendererBase.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/output/video/OpenGLWindowRenderer.cpp
    )

    if(NOT Qt6_FOUND)

        set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
            ${CMAKE_CURRENT_SOURCE_DIR}/opengl/GLWidgetRenderer.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/opengl/GLWidgetRenderer2.cpp
        )

    endif()

    if(DirectX_D3D11_FOUND)

        set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
            ${CMAKE_CURRENT_SOURCE_DIR}/windows/SurfaceInteropD3D11GL.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/windows/SurfaceInteropD3D9GL.cpp
        )

        if(AVCODEC_FOUND)

            set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
                ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/VideoDecoderDXVA.cpp
            )

        endif()

    endif()

    if(OpenGL_EGL_FOUND)

        set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
            ${CMAKE_CURRENT_SOURCE_DIR}/opengl/egl_api.cpp
        )

        if (DirectX_D3D11_FOUND)

            set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
                ${CMAKE_CURRENT_SOURCE_DIR}/windows/SurfaceInteropD3D11EGL.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/windows/SurfaceInteropD3D9EGL.cpp
            )

        endif()

    endif()

endif()


if(GDIPLUS_FOUND)

    set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/GDIRenderer.cpp
    )

endif()

if(Direct2D_FOUND)

    set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/Direct2DRenderer.cpp
    )

endif()

if(ASS_FOUND)

    include_directories(${ASS_INCLUDE_DIRS})

    set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/subtitle/ass_api.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/subtitle/SubtitleProcessorLibASS.cpp
    )

endif()

# ---

if(OPENAL_FOUND)

    include_directories(${OPENAL_INCLUDE_DIR})

    set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/output/audio/openal_api.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/output/audio/AudioOutputOpenAL.cpp
    )

endif()

# ---

if(OPENSLES_FOUND)

    include_directories(${OPENSLES_INCLUDE_DIR})

    set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/output/audio/AudioOutputOpenSL.cpp
    )

endif()

# ---

if(PORTAUDIO_FOUND)

    include_directories(${PORTAUDIO_INCLUDE_DIRS})

    set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/output/audio/AudioOutputPortAudio.cpp
    )

endif()

# ---


if(PULSEAUDIO_FOUND)

    include_directories(${PULSEAUDIO_INCLUDE_DIRS})

    set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/output/audio/AudioOutputPulse.cpp
    )

endif()

# ---


# ---

if(APPLE)

    set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/VideoDecoderVideoToolbox.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/SurfaceInteropCV.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/SurfaceInteropCV_IOSurface.mm

        ${CMAKE_CURRENT_SOURCE_DIR}/output/audio/AudioOutputAudioToolbox.cpp
    )

else()

    # TODO: FFMPEG4::AVBitStreamFilterContext struct is removed (aka FFMPEG4::FF_API_OLD_BSF define). Port to new API.

    if(NOT FFMPEG_VER5_FOUND)

        set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
            ${CMAKE_CURRENT_SOURCE_DIR}/cuda/cuda_api.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/SurfaceInteropCUDA.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/VideoDecoderCUDA.cpp
        )

    endif()

endif()

if(ANDROID)

    set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/VideoDecoderMediaCodec.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/io/AndroidIO.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/io/AndroidIO_jni.cpp
    )

endif()

if(AVRESAMPLE_FOUND)

    set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/AudioResamplerLibav.cpp
    )

endif()

if(SWRESAMPLE_FOUND)

    set(libqtavplayer_SRCS ${libqtavplayer_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/AudioResamplerFF.cpp
    )

endif()
]]
