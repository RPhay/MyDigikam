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

#ifndef QTAV_IMAGECONVERTER_P_H
#define QTAV_IMAGECONVERTER_P_H

#include <QtAV/private/AVCompat.h>
#include <QVector>

namespace QtAV
{

class ImageConverter;
class ImageConverterPrivate : public DPtrPrivate<ImageConverter>
{
public:
    ImageConverterPrivate()
        : w_in(0),h_in(0)
        , w_out(0),h_out(0)
        , fmt_in(QTAV_PIX_FMT_C(YUV420P))
        , fmt_out(QTAV_PIX_FMT_C(RGB32))
        , range_in(ColorRange_Unknown)
        , range_out(ColorRange_Unknown)
        , brightness(0)
        , contrast(0)
        , saturation(0)
        , update_data(true)
        , out_offset(0)
    {
        bits.reserve(8);
        pitchs.reserve(8);
    }
    virtual bool setupColorspaceDetails(bool force = true) {
        Q_UNUSED(force);
        return true;
    }

    int w_in, h_in, w_out, h_out;
    AVPixelFormat fmt_in, fmt_out;
    ColorRange range_in, range_out;
    int brightness, contrast, saturation;
    bool update_data;
    int out_offset;
    QByteArray data_out;
    QVector<quint8*> bits;
    QVector<int> pitchs;
};

} // namespace QtAV
#endif // QTAV_IMAGECONVERTER_P_H
