/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef QTAV_GLOBAL_H
#define QTAV_GLOBAL_H

// C++ includes

#include <cstdarg>

// Qt includes

#include <QMetaType>
#include <QByteArray>
#include <qglobal.h>

// Local includes

#include "QtAV_dptr.h"
#include "digikam_export.h"

#define QTAV_EXPORT         DIGIKAM_EXPORT
#define QTAV_PRIVATE_EXPORT DIGIKAM_EXPORT

/**
 * runtime version. used to compare with compile time version.
 */
QTAV_EXPORT unsigned QtAV_Version();
QTAV_EXPORT QString  QtAV_Version_String();
QTAV_EXPORT QString  QtAV_Version_String_Long();

namespace QtAV
{

/**
 * Default handler is qt message logger. Set environment QTAV_FFMPEG_LOG=0
 * or setFFmpegLogHandler(0) to disable.
 */
QTAV_EXPORT void setFFmpegLogHandler(void(*)(void *, int, const char *, va_list));

/*!
 * \brief setFFmpegLogLevel
 * \param level can be: quiet, panic, fatal, error, warn, info, verbose, debug, trace
 */
QTAV_EXPORT void setFFmpegLogLevel(const QByteArray& level);

/**
 * query the common options of avformat/avcodec that can be used by AVPlayer::setOptionsForXXX.
 * Format/codec specified options are also included
 */
QTAV_EXPORT QString avformatOptions();
QTAV_EXPORT QString avcodecOptions();

QTAV_EXPORT QString aboutFFmpeg_PlainText();
QTAV_EXPORT QString aboutFFmpeg_HTML();
QTAV_EXPORT QString aboutQtAV_PlainText();
QTAV_EXPORT QString aboutQtAV_HTML();

////////////Types/////////////

enum MediaStatus
{
    UnknownMediaStatus,
    NoMedia,
    LoadingMedia,   ///< when source is set
    LoadedMedia,    ///< if auto load and source is set. player is stopped state
    StalledMedia,   ///< insufficient buffering or other interruptions (timeout, user interrupt)
    BufferingMedia, ///< NOT IMPLEMENTED
    BufferedMedia,  ///< when playing // NOT IMPLEMENTED
    EndOfMedia,     ///< Playback has reached the end of the current media. The player is in the StoppedState.
    InvalidMedia    ///< what if loop > 0 or stopPosition() is not mediaStopPosition()?
};

enum BufferMode
{
    BufferTime,
    BufferBytes,
    BufferPackets
};

enum MediaEndActionFlag
{
    MediaEndAction_Default,               ///< stop playback (if loop end) and clear video renderer
    MediaEndAction_KeepDisplay = 1,       ///< stop playback but video renderer keeps the last frame
    MediaEndAction_Pause       = 1 << 1   ///< pause playback. Currently AVPlayer repeat mode will not work if this flag is set
};
Q_DECLARE_FLAGS(MediaEndAction, MediaEndActionFlag)

enum SeekUnit
{
    SeekByTime, ///< only this is supported now
    SeekByByte,
    SeekByFrame
};

enum SeekType
{
    AccurateSeek, ///< slow
    KeyFrameSeek, ///< fast
    AnyFrameSeek
};

// http://www.itu.int/dms_pubrec/itu-r/rec/bt/R-REC-BT.709-5-200204-I!!PDF-E.pdf    https://bitbucket.org/libqxt/libqxt/wiki/Home

// TODO: other color spaces (yuv itu.xxxx, XYZ, ...)

enum ColorSpace
{
    ColorSpace_Unknown,
    ColorSpace_RGB,
    ColorSpace_GBR, ///< for planar gbr format(e.g. video from x264) used in glsl
    ColorSpace_BT601,
    ColorSpace_BT709,
    ColorSpace_XYZ
};

/*!
 * \brief The ColorRange enum
 * YUV or RGB color range
 */
enum ColorRange
{
    ColorRange_Unknown,
    ColorRange_Limited, ///< TV, MPEG
    ColorRange_Full     ///< PC, JPEG
};

/*!
 * \brief The SurfaceType enum
 * HostMemorySurface:
 * Map the decoded frame to host memory
 * GLTextureSurface:
 * Map the decoded frame as an OpenGL texture
 * SourceSurface:
 * get the original surface from decoder, for example VASurfaceID for va-api, CUdeviceptr for CUDA and IDirect3DSurface9* for DXVA.
 * Zero copy mode is required.
 * UserSurface:
 * Do your own magic mapping with it
 */
enum SurfaceType
{
    HostMemorySurface,
    GLTextureSurface,
    SourceSurface,
    UserSurface = 0xffff
};

} // namespace QtAV

Q_DECLARE_METATYPE(QtAV::MediaStatus)
Q_DECLARE_METATYPE(QtAV::MediaEndAction)

#define QTAV_HAVE(FEATURE) (QTAV_HAVE_##FEATURE+0)

#endif // QTAV_GLOBAL_H
