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

#ifndef QTAV_STATISTICS_H
#define QTAV_STATISTICS_H

// Qt includes

#include <QHash>
#include <QTime>
#include <QSharedData>

// Local includes

#include "QtAV_Global.h"

/*!
 * values from functions are dynamically calculated
 */
namespace QtAV
{

class Q_AV_EXPORT Statistics
{
public:

    Statistics();
    ~Statistics();

    void reset();

public:

    QString                 url;
    int                     bit_rate;
    QString                 format;
    QTime                   start_time, duration;
    QHash<QString, QString> metadata;

    class Common
    {
    public:

        Common();

        // TODO: dynamic bit rate compute

        bool available;
        QString codec, codec_long;
        QString decoder;
        QString decoder_detail;
        QTime current_time, total_time, start_time;
        int bit_rate;
        qint64 frames;
        qreal frame_rate; // average fps stored in media stream information

        // union member with ctor, dtor, copy ctor only works in c++11
        /*union {
            audio_only audio;
            video_only video;
        } only;*/

        QHash<QString, QString> metadata;
    } audio, video; // init them

    // from AVCodecContext

    class Q_AV_EXPORT AudioOnly
    {
    public:

        AudioOnly();

        int sample_rate; ///< samples per second
        int channels;    ///< number of audio channels
        QString channel_layout;
        QString sample_fmt;  ///< sample format

        /**
         * Number of samples per channel in an audio frame.
         * decoding: may be set by some decoders to indicate constant frame size
         */
        int frame_size;

        /**
         * number of bytes per packet if constant and known or 0
         * Used by some WAV based audio codecs.
         */
        int block_align;
    } audio_only;

    // from AVCodecContext

    class Q_AV_EXPORT VideoOnly
    {
    public:

        // union member with ctor, dtor, copy ctor only works in c++11

        VideoOnly();
        VideoOnly(const VideoOnly&);
        VideoOnly& operator =(const VideoOnly&);
        ~VideoOnly();

        // compute from pts history

        qreal currentDisplayFPS() const;
        qreal pts() const; // last pts

        int width, height;

        /**
         * Bitstream width / height, may be different from width/height if lowres enabled.
         * - decoding: Set by user before init if known. Codec should override / dynamically change if needed.
         */
        int coded_width, coded_height;

        /**
         * the number of pictures in a group of pictures, or 0 for intra_only
         */
        int     gop_size;
        QString pix_fmt;
        int     rotate;

        /// return current absolute time (seconds since epcho

        qint64 frameDisplayed(qreal pts); // used to compute currentDisplayFPS()

    private:

        class Private;
        QExplicitlySharedDataPointer<Private> d;

    } video_only;
};

} // namespace QtAV

#endif // QTAV_STATISTICS_H
