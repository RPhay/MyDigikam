#
# SPDX-FileCopyrightText: 2010-2022, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

APPLY_COMMON_POLICIES()

# TODO: re-enable warnings step by step to fix code

# For FFMPEG 4 to 5 deprecrated declarations => Fix code to support FFMPEG 5 API.

if(NOT FFMPEG_VER5_FOUND)

    # For FFMPEG 4 to 5 deprecrated declarations => Fix code to support FFMPEG 5 API.
    REMOVE_COMPILER_WARNINGS("-Wdeprecated-declarations")
    DISABLE_GCC_COMPILER_WARNINGS("5.99.99" "-Wno-deprecated-declarations")
    DISABLE_CLANG_COMPILER_WARNINGS("3.99.99" "-Wno-deprecated-declarations")

endif()

# For C++ noisy warnings with Qt5 API (fixed in Qt6).

if(NOT Qt6_FOUND)

    REMOVE_COMPILER_WARNINGS("-Wdeprecated-copy")
    DISABLE_GCC_COMPILER_WARNINGS("5.99.99" "-Wno-deprecated-copy")
    DISABLE_CLANG_COMPILER_WARNINGS("3.99.99" "-Wno-deprecated-copy")

endif()

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

    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/AVCompat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/AVMuxer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/AVDemuxer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/AVError.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/AVPlayerCore.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/AVPlayerCore_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/AudioFormat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/AudioFrame.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/AudioResampler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/AudioResamplerTemplate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/AudioThread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/ImageConverter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/ImageConverterFF.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/VideoCapture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/VideoFrame.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/VideoFrameExtractor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/VideoFormat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/VideoThread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/Packet.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/PacketBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/AVDemuxThread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/AVThread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/AVTranscoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/AVClock.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/Frame.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/FrameReader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/ffmpegconfighelper.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/subtitle/CharsetDetector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/subtitle/PlainText.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/subtitle/PlayerSubtitle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/subtitle/Subtitle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/subtitle/SubtitleProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/subtitle/SubtitleProcessorFFmpeg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/subtitle/SubImage.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/utils/GPUMemCopy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/ColorTransform.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/QtAV_Statistics.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/QtAV_Global.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/QtAV_internal.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/filter/Filter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/FilterContext.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/FilterManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/LibAVFilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/SubtitleFilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/EncodeFilter.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/io/MediaIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/io/QIODeviceIO.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/output/audio/AudioOutput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/output/audio/AudioOutputBackend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/output/audio/AudioOutputNull.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/output/video/VideoRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/output/video/VideoOutput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/output/video/QPainterRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/output/AVOutput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/output/OutputSet.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/codec/audio/AudioDecoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/codec/audio/AudioDecoderFFmpeg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/codec/audio/AudioEncoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/codec/audio/AudioEncoderFFmpeg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/codec/AVDecoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/codec/AVEncoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/VideoDecoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/VideoDecoderFFmpegBase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/VideoDecoderFFmpeg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/VideoDecoderFFmpegHW.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/VideoEncoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/VideoEncoderFFmpeg.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/GraphicsItemRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/VideoPreviewWidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/WidgetRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/QtAVWidgets_Global.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/opengl/OpenGLWidgetRenderer.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/config/VideoEQConfigPage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/config/ClickableMenu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/config/PropertyEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/config/DecoderConfigPage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/config/AVPlayerSlider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/config/AVFormatConfigPage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/config/AVPlayerConfigMngr.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/config/AVFilterConfigPage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/config/ShaderPage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/config/MiscPage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/config/ConfigPageBase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/config/CaptureConfigPage.cpp
)

if(SSE4_1_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/GPUMemCopy_Frame_SSE4.cpp
    )

elseif(SSE2_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/GPUMemCopy_Frame_SSE2.cpp
    )

endif()

if(Qt6_FOUND)

    qt6_add_resources(libqtav_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shaders.qrc)

else()

    qt5_add_resources(libqtav_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shaders.qrc)

endif()

if(DirectX_DSound_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/output/audio/AudioOutputDSound.cpp
    )

endif()

if(DirectX_XAudio2_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/output/audio/AudioOutputXAudio2.cpp
    )

endif()

if(DirectX_D3D11_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/VideoDecoderD3D.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/VideoDecoderD3D11.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/SurfaceInteropD3D11.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/SurfaceInteropD3D9.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/D3D11VP.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/DirectXHelper.cpp
    )

endif()

if(X11_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/filter/X11FilterContext.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/X11/X11Renderer.cpp
    )

endif()

if(X11_Xv_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/X11/XVRenderer.cpp
    )

endif()

if(OPENGL_FOUND)

    include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::OpenGL,INTERFACE_INCLUDE_DIRECTORIES>)

    if(Qt6_FOUND)

        include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::OpenGLWidgets,INTERFACE_INCLUDE_DIRECTORIES>)

    endif()

    set(libqtav_SRCS ${libqtav_SRCS}
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

        set(libqtav_SRCS ${libqtav_SRCS}
            ${CMAKE_CURRENT_SOURCE_DIR}/opengl/GLWidgetRenderer.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/opengl/GLWidgetRenderer2.cpp
        )

    endif()

    if(DirectX_D3D11_FOUND)

        set(libqtav_SRCS ${libqtav_SRCS}
            ${CMAKE_CURRENT_SOURCE_DIR}/windows/SurfaceInteropD3D11GL.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/windows/SurfaceInteropD3D9GL.cpp
        )

        if(AVCODEC_FOUND)

            set(libqtav_SRCS ${libqtav_SRCS}
                ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/VideoDecoderDXVA.cpp
            )

        endif()

    endif()

    if(OpenGL_EGL_FOUND)

        set(libqtav_SRCS ${libqtav_SRCS}
            ${CMAKE_CURRENT_SOURCE_DIR}/opengl/egl_api.cpp
        )

        if (DirectX_D3D11_FOUND)

            set(libqtav_SRCS ${libqtav_SRCS}
                ${CMAKE_CURRENT_SOURCE_DIR}/windows/SurfaceInteropD3D11EGL.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/windows/SurfaceInteropD3D9EGL.cpp
            )

        endif()

    endif()

endif()


if(GDIPLUS_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/GDIRenderer.cpp
    )

endif()

if(Direct2D_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/Direct2DRenderer.cpp
    )

endif()

if(ASS_FOUND)

    include_directories(${ASS_INCLUDE_DIRS})

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/subtitle/ass_api.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/subtitle/SubtitleProcessorLibASS.cpp
    )

endif()

# ---

if(OPENAL_FOUND)

    include_directories(${OPENAL_INCLUDE_DIR})

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/output/audio/openal_api.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/output/audio/AudioOutputOpenAL.cpp
    )

endif()

# ---

if(OPENSLES_FOUND)

    include_directories(${OPENSLES_INCLUDE_DIR})

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/output/audio/AudioOutputOpenSL.cpp
    )

endif()

# ---

if(PORTAUDIO_FOUND)

    include_directories(${PORTAUDIO_INCLUDE_DIRS})

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/output/audio/AudioOutputPortAudio.cpp
    )

endif()

# ---


if(PULSEAUDIO_FOUND)

    include_directories(${PULSEAUDIO_INCLUDE_DIRS})

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/output/audio/AudioOutputPulse.cpp
    )

endif()

# ---

# TODO: FFMPEG4::vaapi struct is removed and code must be ported to FFMPEG5::AVCodecContext.hw_frames_ctx instead.

if(VAAPI_FOUND AND NOT FFMPEG_VER5_FOUND)

    include_directories(${VAAPI_INCLUDE_DIRS})

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/vaapi/vaapi_helper.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/vaapi/SurfaceInteropVAAPI.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/VideoDecoderVAAPI.cpp
    )

endif()

# ---

if(APPLE)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/VideoDecoderVideoToolbox.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/SurfaceInteropCV.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/SurfaceInteropCV_IOSurface.mm

        ${CMAKE_CURRENT_SOURCE_DIR}/output/audio/AudioOutputAudioToolbox.cpp
    )

else()

    # TODO: FFMPEG4::AVBitStreamFilterContext struct is removed (aka FFMPEG4::FF_API_OLD_BSF define). Port to new API.

    if(NOT FFMPEG_VER5_FOUND)

        set(libqtav_SRCS ${libqtav_SRCS}
            ${CMAKE_CURRENT_SOURCE_DIR}/cuda/cuda_api.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/SurfaceInteropCUDA.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/VideoDecoderCUDA.cpp
        )

    endif()

endif()

if(ANDROID)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/codec/video/VideoDecoderMediaCodec.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/io/AndroidIO.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/io/AndroidIO_jni.cpp
    )

endif()

# TODO more check for WinRT Qt modules availability is required here

#if(WIN32)
#
#    set(libqtav_SRCS ${libqtav_SRCS}
#        ${CMAKE_CURRENT_SOURCE_DIR}/io/WinRTIO.cpp
#    )
#
#endif()

if(AVRESAMPLE_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/AudioResamplerLibav.cpp
    )

endif()

if(SWRESAMPLE_FOUND)

    set(libqtav_SRCS ${libqtav_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/AudioResamplerFF.cpp
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
