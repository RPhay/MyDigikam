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

# For undefined pre-processor values.
REMOVE_COMPILER_WARNINGS("-Wundef")
DISABLE_GCC_COMPILER_WARNINGS("5.99.99" "-Wno-undef")

# For not portable pre-processor defined operator uses.
REMOVE_COMPILER_WARNINGS("-Wexpansion-to-defined")
DISABLE_GCC_COMPILER_WARNINGS("5.99.99" "-Wno-expansion-to-defined")

include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>
)

set(libqtav_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AVCompat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/CharsetDetector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/PlainText.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/PlayerSubtitle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/Subtitle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/SubtitleProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/SubtitleProcessorFFmpeg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/SubImage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/GPUMemCopy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioThread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/QtAV_Global.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/QtAV_internal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AVThread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioFormat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioFrame.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioResampler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioResamplerTemplate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/audio/AudioDecoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/audio/AudioDecoderFFmpeg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/audio/AudioEncoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/audio/AudioEncoderFFmpeg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/AVDecoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/AVEncoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AVMuxer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AVDemuxer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AVDemuxThread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ColorTransform.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Frame.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FrameReader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/filter/Filter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/filter/FilterContext.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/filter/FilterManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/filter/LibAVFilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/filter/SubtitleFilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/filter/EncodeFilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ImageConverter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ImageConverterFF.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Packet.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PacketBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AVError.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AVPlayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AVPlayerPrivate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AVTranscoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AVClock.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VideoCapture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VideoFormat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VideoFrame.cpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Statistics.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/VideoDecoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/VideoDecoderFFmpegBase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/VideoDecoderFFmpeg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/VideoDecoderFFmpegHW.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/VideoEncoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/VideoEncoderFFmpeg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VideoThread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VideoFrameExtractor.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/QtAVWidgets_Global.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/GraphicsItemRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/OpenGLWidgetRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/VideoPreviewWidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/WidgetRenderer.cpp
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

qt5_add_resources(libqtav_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/shaders.qrc)

file(GLOB QTAV_HEADERS         ${CMAKE_CURRENT_SOURCE_DIR}/src/QtAV/*.h)
file(GLOB QTAV_PRIVATE_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/QtAV/private/*.h)

set(QTAV_WIDGETS_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/QtAVWidgets/QtAVWidgets_Global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/QtAVWidgets/QtAVWidgets_Version.h
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/QtAVWidgets/GraphicsItemRenderer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/QtAVWidgets/OpenGLWidgetRenderer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/QtAVWidgets/QtAVWidgets.h
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/QtAVWidgets/VideoPreviewWidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/QtAVWidgets/WidgetRenderer.h
)

set(libqtav_SRCS ${libqtav_SRCS}
                 ${QTAV_HEADERS}
                 ${QTAV_PRIVATE_HEADERS}
                 ${QTAV_WIDGETS_HEADERS}
)

if(D3D11_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/VideoDecoderD3D11.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/directx/SurfaceInteropD3D11.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/directx/D3D11VP.cpp
    )

endif()

if(XAUDIO2_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/output/audio/AudioOutputXAudio2.cpp
    )

endif()

if(HAVE_OPENGL)

    include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::OpenGL,INTERFACE_INCLUDE_DIRECTORIES>)

    if(Qt6_FOUND)

        include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::OpenGLWidgets,INTERFACE_INCLUDE_DIRECTORIES>)

    endif()

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/ConvolutionShader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/gl_api.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/egl_api.cpp
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

        ${CMAKE_CURRENT_SOURCE_DIR}/widgets/GLWidgetRenderer2.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets/QtAVWidgets/GLWidgetRenderer2.h
    )

    if(D3D11_FOUND)

        set(libqtav_SRCS ${libqtav_SRCS}
            ${CMAKE_CURRENT_SOURCE_DIR}/src/directx/SurfaceInteropD3D11EGL.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/directx/SurfaceInteropD3D11GL.cpp
        )

    endif()

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
        ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/SurfaceInteropIOSurface.cpp
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
        ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioResamplerLibav.cpp
    )

endif()

if(SWRESAMPLE_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioResamplerFF.cpp
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
