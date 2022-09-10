/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#   include <QRegularExpression>
#else
#   include <QRegExp>
#endif

/**
 * runtime version. used to compare with compile time version.
 */
DIGIKAM_EXPORT unsigned QtAV_Version();
DIGIKAM_EXPORT QString  QtAV_Version_String();
DIGIKAM_EXPORT QString  QtAV_Version_String_Long();

namespace QtAV
{

/**
 * Default handler is qt message logger. Set environment QTAV_FFMPEG_LOG=0
 * or setFFmpegLogHandler(0) to disable.
 */
DIGIKAM_EXPORT void setFFmpegLogHandler(void(*)(void*, int, const char*, va_list));

/*!
 * \brief setFFmpegLogLevel
 * \param level can be: quiet, panic, fatal, error, warn, info, verbose, debug, trace
 */
DIGIKAM_EXPORT void setFFmpegLogLevel(const QByteArray& level);

/**
 * query the common options of avformat/avcodec that can be used by AVPlayerCore::setOptionsForXXX.
 * Format/codec specified options are also included
 */
DIGIKAM_EXPORT QString avformatOptions();
DIGIKAM_EXPORT QString avcodecOptions();

DIGIKAM_EXPORT QString aboutFFmpeg_PlainText();
DIGIKAM_EXPORT QString aboutFFmpeg_HTML();
DIGIKAM_EXPORT QString aboutQtAV_PlainText();
DIGIKAM_EXPORT QString aboutQtAV_HTML();

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
    MediaEndAction_Pause       = 1 << 1   ///< pause playback. Currently AVPlayerCore repeat mode will not work if this flag is set
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

// http://www.itu.int/dms_pubrec/itu-r/rec/bt/R-REC-BT.709-5-200204-I!!PDF-E.pdf    // krazy:exclude=insecurenet
// https://bitbucket.org/libqxt/libqxt/wiki/Home

// TODO: other color spaces (yuv itu.xxxx, XYZ, ...)

enum ColorSpace
{
    ColorSpace_Unknown,
    ColorSpace_RGB,
    ColorSpace_GBR, ///< for planar gbr format (e.g. video from x264) used in glsl
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
 * get the original surface from decoder, for example VASurfaceID for va-api,
 * CUdeviceptr for CUDA and IDirect3DSurface9* for DXVA.
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
