/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
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

#ifndef QTAV_AV_ERROR_H
#define QTAV_AV_ERROR_H

// Qt includes

#include <QString>

// Local includes

#include "QtAV_Global.h"

namespace QtAV
{

class Q_AV_EXPORT AVError
{
public:

    enum ErrorCode
    {
        NoError,

        //open/read/seek network stream error. value must be less then ResourceError because of correct_error_by_ffmpeg
        NetworkError, // all above and before NoError are NetworkError

        OpenTimedout,
        OpenError,
        ParseStreamTimedOut,
        FindStreamInfoTimedout = ParseStreamTimedOut,
        ParseStreamError,
        FindStreamInfoError = ParseStreamError,
        StreamNotFound, //a,v,s?
        ReadTimedout,
        ReadError,
        SeekError,
        ResourceError, // all above and before NetworkError are ResourceError

        OpenCodecError,
        CloseCodecError,
        AudioCodecNotFound,
        VideoCodecNotFound,
        SubtitleCodecNotFound,
        CodecError, // all above and before NoError are CodecError

        FormatError, // all above and before CodecError are FormatError

        // decrypt error. Not implemented
        AccessDenied, // all above and before NetworkError are AccessDenied

        UnknowError
    };

    AVError();
    AVError(ErrorCode code, int ffmpegError = 0);

    /*!
     * \brief AVError
     * string() will be detail. If ffmpeg error not 0, also contains ffmpegErrorString()
     * \param code ErrorCode value
     * \param detail ErrorCode string will be overrided by detail.
     * \param ffmpegError ffmpeg error code. If not 0, string() will contains ffmpeg error string.
     */
    AVError(ErrorCode code, const QString& detail, int ffmpegError = 0);
    AVError(const AVError& other);

    AVError &operator=(const AVError &other);
    bool operator==(const AVError &other) const;
    inline bool operator!=(const AVError &other) const
    {
        return !(*this == other);
    }

    void setError(ErrorCode ec);
    ErrorCode error() const;
    QString string() const;

    int ffmpegErrorCode() const;
    QString ffmpegErrorString() const;

private:

    ErrorCode mError;
    int mFFmpegError;
    QString mDetail;
};

} // namespace QtAV

Q_DECLARE_METATYPE(QtAV::AVError)

#ifndef QT_NO_DEBUG_STREAM

QT_BEGIN_NAMESPACE
class QDebug;
QT_END_NAMESPACE
Q_AV_EXPORT QDebug operator<<(QDebug debug, const QtAV::AVError &error);

#endif

#endif // QTAV_AV_ERROR_H
