#
# Copyright (c) 2010-2022, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

# TODO: re-enable warnings step by step to fix code

# For FFMPEG 4 to 5 deprecrated declarations => Fix code to support FFMPEG 5 API.
REMOVE_COMPILER_WARNINGS("-Wdeprecated-declarations")
DISABLE_GCC_COMPILER_WARNINGS("5.99.99" "-Wno-deprecated-declarations")
DISABLE_CLANG_COMPILER_WARNINGS("3.99.99" "-Wno-deprecated-declarations")

# For undefined pre-processor values.
REMOVE_COMPILER_WARNINGS("-Wundef")
DISABLE_GCC_COMPILER_WARNINGS("5.99.99" "-Wno-undef")
DISABLE_CLANG_COMPILER_WARNINGS("3.99.99" "-Wno-undef")

# For not portable pre-processor defined operator uses.
REMOVE_COMPILER_WARNINGS("-Wexpansion-to-defined")
DISABLE_GCC_COMPILER_WARNINGS("5.99.99" "-Wno-expansion-to-defined")
DISABLE_CLANG_COMPILER_WARNINGS("3.99.99" "-Wno-expansion-to-defined")

# For CAPI anonymous non-C-compatible type given name for linkage purposes by typedef declaration.
REMOVE_COMPILER_WARNINGS("-Wnon-c-typedef-for-linkage")
DISABLE_CLANG_COMPILER_WARNINGS("3.99.99" "-Wno-non-c-typedef-for-linkage")


include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>
                    ${DIRECTX_LIBRARY_DIRS}             # Populated under Windows, else empty
)

set(libqtav_SRCS

    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/AVCompat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/AVMuxer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/AVDemuxer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/AVError.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/AVPlayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/AVPlayerPrivate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/AudioFormat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/AudioFrame.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/AudioResampler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/AudioResamplerTemplate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/AudioThread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/ImageConverter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/ImageConverterFF.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/VideoCapture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/VideoFrame.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/VideoFrameExtractor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/VideoFormat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/VideoThread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/Packet.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/PacketBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/AVDemuxThread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/AVThread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/AVTranscoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/AVClock.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/Frame.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/FrameReader.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/CharsetDetector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/PlainText.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/PlayerSubtitle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/Subtitle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/SubtitleProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/SubtitleProcessorFFmpeg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/SubImage.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/GPUMemCopy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/Statistics.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/ColorTransform.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/QtAV_Global.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/QtAV_internal.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/filter/Filter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/filter/FilterContext.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/filter/FilterManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/filter/LibAVFilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/filter/SubtitleFilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/filter/EncodeFilter.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/io/MediaIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/io/QIODeviceIO.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/output/audio/AudioOutput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/output/audio/AudioOutputBackend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/output/audio/AudioOutputNull.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/output/video/VideoRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/output/video/VideoOutput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/output/video/QPainterRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/output/AVOutput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/output/OutputSet.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/audio/AudioDecoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/audio/AudioDecoderFFmpeg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/audio/AudioEncoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/audio/AudioEncoderFFmpeg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/AVDecoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/AVEncoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/VideoDecoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/VideoDecoderFFmpegBase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/VideoDecoderFFmpeg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/VideoDecoderFFmpegHW.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/VideoEncoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/VideoEncoderFFmpeg.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/common/GraphicsItemRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/common/VideoPreviewWidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/common/WidgetRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/utils/QtAVWidgets_Global.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/opengl/OpenGLWidgetRenderer.cpp
)

if(SSE4_1_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/CopyFrame_SSE4.cpp
    )

elseif(SSE2_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/CopyFrame_SSE2.cpp
    )

endif()

if(Qt6_FOUND)

    qt6_add_resources(libqtav_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/shaders.qrc)

else()

    qt5_add_resources(libqtav_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/shaders.qrc)

endif()

if(DirectX_DSound_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/output/audio/AudioOutputDSound.cpp
    )

endif()

if(DirectX_XAudio2_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/output/audio/AudioOutputXAudio2.cpp
    )

endif()

if(DirectX_D3D11_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/VideoDecoderD3D.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/VideoDecoderD3D11.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/directx/SurfaceInteropD3D11.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/directx/SurfaceInteropD3D9.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/directx/D3D11VP.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/directx/DirectXHelper.cpp
    )

endif()

if(X11_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/filter/X11FilterContext.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/widgets/X11/X11Renderer.cpp
    )

endif()

if(X11_Xv_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets/X11/XVRenderer.cpp
    )

endif()

if(HAVE_OPENGL)

    include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::OpenGL,INTERFACE_INCLUDE_DIRECTORIES>)

    if(Qt6_FOUND)

        include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::OpenGLWidgets,INTERFACE_INCLUDE_DIRECTORIES>)

    endif()

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/gl_api.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/ConvolutionShader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/OpenGLVideo.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/SubImagesRenderer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/Geometry.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/OpenGLHelper.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/ShaderManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/VideoShader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/GeometryRenderer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/OpenGLTypes.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/SubImagesGeometry.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/VideoShaderObject.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/src/filter/GLSLFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/output/video/OpenGLRendererBase.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/output/video/OpenGLWindowRenderer.cpp
    )

    if(NOT Qt6_FOUND)

        set(libqtav_SRCS ${libqtav_SRCS}
            ${CMAKE_CURRENT_SOURCE_DIR}/widgets/opengl/GLWidgetRenderer.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/widgets/opengl/GLWidgetRenderer2.cpp
        )

    endif()

    if(DirectX_D3D11_FOUND)

        set(libqtav_SRCS ${libqtav_SRCS}
            ${CMAKE_CURRENT_SOURCE_DIR}/src/directx/SurfaceInteropD3D11GL.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/directx/SurfaceInteropD3D9GL.cpp
        )

        if(AVCODEC_FOUND)

            set(libqtav_SRCS ${libqtav_SRCS}
                ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/VideoDecoderDXVA.cpp
            )

        endif()

    endif()

    if(OpenGL_EGL_FOUND)

        set(libqtav_SRCS ${libqtav_SRCS}
            ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/egl_api.cpp
        )

        if (DirectX_D3D11_FOUND)

            set(libqtav_SRCS ${libqtav_SRCS}
                ${CMAKE_CURRENT_SOURCE_DIR}/src/directx/SurfaceInteropD3D11EGL.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/directx/SurfaceInteropD3D9EGL.cpp
            )

        endif()

    endif()

endif()


if(GDIPLUS_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets/windows/GDIRenderer.cpp
    )

endif()

if(Direct2D_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets/windows/Direct2DRenderer.cpp
    )

endif()

if(ASS_FOUND)

    include_directories(${ASS_INCLUDE_DIRS})

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/ass_api.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/SubtitleProcessorLibASS.cpp
    )

endif()

# ---

if(OPENAL_FOUND)

    include_directories(${OPENAL_INCLUDE_DIR})

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/output/audio/openal_api.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/output/audio/AudioOutputOpenAL.cpp
    )

endif()

# ---

if(OPENSLES_FOUND)

    include_directories(${OPENSLES_INCLUDE_DIR})

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/output/audio/AudioOutputOpenSL.cpp
    )

endif()

# ---

if(PORTAUDIO_FOUND)

    include_directories(${PORTAUDIO_INCLUDE_DIRS})

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/output/audio/AudioOutputPortAudio.cpp
    )

endif()

# ---


if(PULSEAUDIO_FOUND)

    include_directories(${PULSEAUDIO_INCLUDE_DIRS})

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/output/audio/AudioOutputPulse.cpp
    )

endif()

# ---

if(VAAPI_FOUND)

    include_directories(${VAAPI_INCLUDE_DIRS})

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vaapi/vaapi_helper.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vaapi/SurfaceInteropVAAPI.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/VideoDecoderVAAPI.cpp
    )

endif()

# ---

if(APPLE)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/VideoDecoderVideoToolbox.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/SurfaceInteropCV.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/SurfaceInteropIO_Surface.mm

        ${CMAKE_CURRENT_SOURCE_DIR}/src/output/audio/AudioOutputAudioToolbox.cpp
    )

else()

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/cuda/cuda_api.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/SurfaceInteropCUDA.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/VideoDecoderCUDA.cpp
    )

endif()

if(AVRESAMPLE_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/AudioResamplerLibav.cpp
    )

endif()

if(SWRESAMPLE_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg/AudioResamplerFF.cpp
    )

endif()

# -------------------------------------------------------------------------

# Used by digikamcore
add_library(core_qtav_obj OBJECT ${libqtav_SRCS})

target_compile_definitions(core_qtav_obj
                           PRIVATE
                           digikamcore_EXPORTS
)

target_compile_definitions(core_qtav_obj
                           PRIVATE
                           ${MEDIAPLAYER_DEFINITIONS}
)

target_compile_options(core_qtav_obj
                       PRIVATE
                       ${MEDIAPLAYER_FLAGS}
)
