/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2016-04-21
 * Description : video thumbnails extraction based on ffmpeg
 *
 * SPDX-FileCopyrightText: 2010      by Dirk Vanden Boer <dirk dot vdb at gmail dot com>
 * SPDX-FileCopyrightText: 2016-2018 by Maik Qualmann <metzpinguin at gmail dot com>
 * SPDX-FileCopyrightText: 2016-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_VIDEO_THUMB_DECODER_PRIVATE_H
#define DIGIKAM_VIDEO_THUMB_DECODER_PRIVATE_H

#include "videothumbdecoder.h"

// FFMpeg includes

extern "C"
{
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>
}

namespace Digikam
{

class Q_DECL_HIDDEN VideoThumbDecoder::Private
{
public:

    explicit Private();
    ~Private();

public:

    int                videoStream;
    AVFormatContext*   pFormatContext;
    AVCodecContext*    pVideoCodecContext;
    AVCodecParameters* pVideoCodecParameters;

#ifndef HAVE_FFMPEG_VERSION5

    AVCodec*           pVideoCodec;

#else // ffmpeg >= 5

    const AVCodec*     pVideoCodec;

#endif

    AVStream*          pVideoStream;
    AVFrame*           pFrame;
    quint8*            pFrameBuffer;
    AVPacket*          pPacket;
    bool               allowSeek;
    bool               initialized;
    AVFilterContext*   bufferSinkContext;
    AVFilterContext*   bufferSourceContext;
    AVFilterGraph*     filterGraph;
    AVFrame*           filterFrame;
    int                lastWidth;
    int                lastHeight;
    enum AVPixelFormat lastPixfmt;

public:

    bool initializeVideo();
    bool getVideoPacket();
    bool decodeVideoPacket() const;

    void convertAndScaleFrame(AVPixelFormat format,
                              int scaledSize,
                              bool maintainAspectRatio,
                              int& scaledWidth,
                              int& scaledHeight);

    bool processFilterGraph(AVFrame* const dst,
                            const AVFrame* const src,
                            enum AVPixelFormat pixfmt,
                            int width,
                            int height);

    void deleteFilterGraph();

private:

    bool initFilterGraph(enum AVPixelFormat pixfmt, int width, int height);

    void calculateDimensions(int squareSize,
                             bool maintainAspectRatio,
                             int& destWidth,
                             int& destHeight);

    void createAVFrame(AVFrame** const avFrame,
                       quint8** const frameBuffer,
                       int width,
                       int height,
                       AVPixelFormat format);

    // cppcheck-suppress unusedPrivateFunction
    int  decodeVideoNew(AVCodecContext* const avContext,
                        AVFrame* const avFrame,
                        int* gotFrame,
                        AVPacket* const avPacket) const;
};

} // namespace Digikam

#endif // DIGIKAM_VIDEO_THUMB_DECODER_PRIVATE_H
