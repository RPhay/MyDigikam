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

#include "AVError.h"

#ifndef QT_NO_DEBUG_STREAM
#   include <QDebug>
#endif

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "AVCompat.h"

namespace QtAV
{

namespace
{

class Q_DECL_HIDDEN RegisterMetaTypes
{
public:

    RegisterMetaTypes()
    {
        qRegisterMetaType<QtAV::AVError>("QtAV::AVError");
    }
} _registerMetaTypes;

} // namespace

static AVError::ErrorCode errorFromFFmpeg(int fe)
{
    typedef struct Q_DECL_HIDDEN Err_Entry_
    {
        int                ff = 0;
        AVError::ErrorCode e  = AVError::UnknowError;
    } err_entry;

    static const err_entry err_map[] =
    {
        { AVERROR_BSF_NOT_FOUND,      AVError::FormatError   },

#ifdef AVERROR_BUFFER_TOO_SMALL

        { AVERROR_BUFFER_TOO_SMALL,   AVError::ResourceError },

#endif

        { AVERROR_DECODER_NOT_FOUND,  AVError::CodecError    },
        { AVERROR_ENCODER_NOT_FOUND,  AVError::CodecError    },
        { AVERROR_DEMUXER_NOT_FOUND,  AVError::FormatError   },
        { AVERROR_MUXER_NOT_FOUND,    AVError::FormatError   },
        { AVERROR_PROTOCOL_NOT_FOUND, AVError::ResourceError },
        { AVERROR_STREAM_NOT_FOUND,   AVError::ResourceError },
        { 0,                          AVError::UnknowError   }
    };

    for (int i = 0 ; err_map[i].ff ; ++i)
    {
        if (err_map[i].ff == fe)
            return err_map[i].e;
    }

    return AVError::UnknowError;
}

/**
 *  correct wrong AVError to a right category by ffmpeg error code
 *  does nothing if ffmpeg error code is 0
 */
static void correct_error_by_ffmpeg(AVError::ErrorCode* e, int fe)
{
    if (!fe || !e)
        return;

    const AVError::ErrorCode ec = errorFromFFmpeg(fe);

    if (*e > ec)
        *e = ec;
}

AVError::AVError()
    : mError      (NoError),
      mFFmpegError(0)
{
}

AVError::AVError(ErrorCode code, int ffmpegError)
    : mError      (code),
      mFFmpegError(ffmpegError)
{
    correct_error_by_ffmpeg(&mError, mFFmpegError);
}

AVError::AVError(ErrorCode code, const QString& detail, int ffmpegError)
    : mError      (code),
      mFFmpegError(ffmpegError),
      mDetail     (detail)
{
    correct_error_by_ffmpeg(&mError, mFFmpegError);
}

AVError::AVError(const AVError& other)
    : mError      (other.mError),
      mFFmpegError(other.mFFmpegError),
      mDetail     (other.mDetail)
{
}

AVError& AVError::operator=(const AVError& other)
{
    mError       = other.mError;
    mFFmpegError = other.mFFmpegError;

    return *this;
}

bool AVError::operator==(const AVError& other) const
{
    return ((mError == other.mError) && (mFFmpegError == other.mFFmpegError));
}

void AVError::setError(ErrorCode ec)
{
    mError = ec;
}

AVError::ErrorCode AVError::error() const
{
    return mError;
}

QString AVError::string() const
{
    QString errStr(mDetail);

    if (errStr.isEmpty())
    {
        switch (mError)
        {
            case NoError:
            {
                errStr = i18n("No error");

                break;
            }

            case OpenError:
            {
                errStr = i18n("Open error");

                break;
            }

            case OpenTimedout:
            {
                errStr = i18n("Open timed out");

                break;
            }

            case ParseStreamTimedOut:
            {
                errStr = i18n("Parse stream timed out");

                break;
            }

            case ParseStreamError:
            {
                errStr = i18n("Parse stream error");

                break;
            }

            case StreamNotFound:
            {
                errStr = i18n("Stream not found");

                break;
            }

            case ReadTimedout:
            {
                errStr = i18n("Read packet timed out");

                break;
            }

            case ReadError:
            {
                errStr = i18n("Read error");

                break;
            }

            case SeekError:
            {
                errStr = i18n("Seek error");

                break;
            }

            case ResourceError:
            {
                errStr = i18n("Resource error");

                break;
            }

            case OpenCodecError:
            {
                errStr = i18n("Open codec error");

                break;
            }

            case CloseCodecError:
            {
                errStr = i18n("Close codec error");

                break;
            }

            case VideoCodecNotFound:
            {
                errStr = i18n("Video codec not found");

                break;
            }

            case AudioCodecNotFound:
            {
                errStr = i18n("Audio codec not found");

                break;
            }

            case SubtitleCodecNotFound:
            {
                errStr = i18n("Subtitle codec not found");

                break;
            }

            case CodecError:
            {
                errStr = i18n("Codec error");

                break;
            }

            case FormatError:
            {
                errStr = i18n("Format error");

                break;
            }

            case NetworkError:
            {
                errStr = i18n("Network error");

                break;
            }

            case AccessDenied:
            {
                errStr = i18n("Access denied");

                break;
            }

            default:
            {
                errStr = i18n("Unknown error");

                break;
            }
        }
    }

    if (mFFmpegError != 0)
    {
        errStr += QString::fromUtf8("\n(FFmpeg %1: %2)")
                    .arg(mFFmpegError, 0, 16)
                    .arg(ffmpegErrorString());
    }

    return errStr;
}

int AVError::ffmpegErrorCode() const
{
    return mFFmpegError;
}

QString AVError::ffmpegErrorString() const
{
    if (mFFmpegError == 0)
        return QString();

    return QString::fromUtf8(av_err2str(mFFmpegError));
}

} // namespace QtAV

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<(QDebug debug, const QtAV::AVError& error)
{
    debug << error.string();

    return debug;
}

#endif
