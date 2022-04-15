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

#ifndef QTAV_MediaIO_P_H
#define QTAV_MediaIO_P_H

#include "QtAV/QtAV_Global.h"
#include "QtAV/private/AVCompat.h"
#include <QtCore/QString>
#include "QtAV/MediaIO.h"

namespace QtAV {

class MediaIO;
class Q_AV_PRIVATE_EXPORT MediaIOPrivate : public DPtrPrivate<MediaIO>
{
public:
    MediaIOPrivate()
        : ctx(NULL)
        , buffer_size(-1)
        , mode(MediaIO::Read)
    {}
    AVIOContext *ctx;
    int buffer_size;
    MediaIO::AccessMode mode;
    QString url;
};

} //namespace QtAV
#endif // QTAV_MediaIO_P_H
