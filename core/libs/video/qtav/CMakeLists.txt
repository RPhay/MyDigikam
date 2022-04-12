#
# Copyright (c) 2010-2022, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

if(ENABLE_MEDIAPLAYER)

    # The QtAV library does not adhere to the flags we use
    remove_definitions(
        -DQT_NO_CAST_FROM_ASCII
        -DQT_NO_CAST_TO_ASCII
        -DQT_NO_URL_CAST_FROM_STRING
        -DQT_NO_CAST_FROM_BYTEARRAY
        -DQT_DISABLE_DEPRECATED_BEFORE=0x050E00
    )

    include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                        $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>
    )

    set(libqtav_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/AVCompat.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/QtAV_Global.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/CharsetDetector.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/PlainText.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/PlayerSubtitle.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/Subtitle.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/SubtitleProcessor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/SubtitleProcessorFFmpeg.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/subtitle/SubImage.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/GPUMemCopy.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/Logger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioThread.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/internal.cpp
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

        ${CMAKE_CURRENT_SOURCE_DIR}/widgets/global.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets/GraphicsItemRenderer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets/OpenGLWidgetRenderer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets/VideoPreviewWidget.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets/WidgetRenderer.cpp
    )

    if(HAVE_OPENGL)

        include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::OpenGL,INTERFACE_INCLUDE_DIRECTORIES>)

        set(QT_OPENGL_LIBRARIES Qt${QT_VERSION_MAJOR}::OpenGL)

        if (Qt6_FOUND)
            include_directories($<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::OpenGLWidgets,INTERFACE_INCLUDE_DIRECTORIES>)
            set(QT_OPENGL_LIBRARIES ${QT_OPENGL_LIBRARIES} Qt${QT_VERSION_MAJOR}::OpenGLWidgets)
        endif()

        set(libqtav_SRCS ${libqtav_SRCS}
            ${CMAKE_CURRENT_SOURCE_DIR}/widgets/GLWidgetRenderer2.cpp
        )

    endif()

    # Used by digikamcore
    add_library(core_qtav_obj OBJECT ${libqtav_SRCS})

    target_compile_definitions(core_qtav_obj
                               PRIVATE
                               -DBUILD_QTAV_LIB
                               -DBUILD_QTAVWIDGETS_LIB
                               -DQTAV_HAVE_GL=1
                               -D__STDC_CONSTANT_MACROS
                               -DQTAV_HAVE_CAPI=1
    )

    find_package(OpenAL)

    if(OPENAL_FOUND)

        if(APPLE)
            target_compile_definitions(core_qtav_obj PRIVATE -DHEADER_OPENAL_PREFIX)
        endif()

        set(libqtav_SRCS ${libqtav_SRCS}
            ${CMAKE_CURRENT_SOURCE_DIR}/src/capi/openal_api.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/output/audio/AudioOutputOpenAL.cpp
        )

        target_compile_definitions(core_qtav_obj PRIVATE -DQTAV_HAVE_OPENAL=1)

    endif()

    if(APPLE)

        target_compile_definitions(core_qtav_obj PRIVATE -DQTAV_HAVE_VIDEOTOOLBOX=1)

        set(libqtav_SRCS ${libqtav_SRCS}
            ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/VideoDecoderVideoToolbox.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/SurfaceInteropCV.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/codec/video/SurfaceInteropIOSurface.cpp
        )

    endif()

    if(${AVCODEC_FOUND})
        target_compile_definitions(core_qtav_obj PRIVATE -DQTAV_HAVE_AVCODEC)
    endif()

    if(${AVDEVICE_FOUND})
        target_compile_definitions(core_qtav_obj PRIVATE -DQTAV_HAVE_AVDEVICE)
    endif()

    if(${AVFILTER_FOUND})
        target_compile_definitions(core_qtav_obj PRIVATE -DQTAV_HAVE_AVFILTER)
    endif()

    if(${AVFORMAT_FOUND})
        target_compile_definitions(core_qtav_obj PRIVATE -DQTAV_HAVE_AVFORMAT)
    endif()

    if(${AVUTIL_FOUND})
        target_compile_definitions(core_qtav_obj PRIVATE -DQTAV_HAVE_AVUTIL)
    endif()

    if(${SWSCALE_FOUND})
        target_compile_definitions(core_qtav_obj PRIVATE -DQTAV_HAVE_SWSCALE)
    endif()

    if(${AVRESAMPLE_FOUND})

        target_compile_definitions(core_qtav_obj PRIVATE -DDQTAV_HAVE_AVRESAMPLE)

        set(libqtav_SRCS ${libqtav_SRCS}
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioResamplerLibav.cpp
        )

    endif()

    if(${SWRESAMPLE_FOUND})

        target_compile_definitions(core_qtav_obj PRIVATE -DQTAV_HAVE_SWRESAMPLE)

        set(libqtav_SRCS ${libqtav_SRCS}
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioResamplerFF.cpp
        )

    endif()

    target_compile_definitions(core_qtav_obj
                               PRIVATE
                               digikamcore_EXPORTS
    )

    if(HAVE_OPENGL)

        target_link_libraries(core_qtav_obj
                              PRIVATE
                              ${QT_OPENGL_LIBRARIES}
        )

    endif()

endif()
