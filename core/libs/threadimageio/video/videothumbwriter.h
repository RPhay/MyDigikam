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

#ifndef DIGIKAM_VIDEO_THUMB_WRITER_H
#define DIGIKAM_VIDEO_THUMB_WRITER_H

// Qt includes

#include <QtGlobal>
#include <QImage>
#include <QVector>

namespace Digikam
{

class VideoFrame
{
public:

    VideoFrame();
    VideoFrame(int width, int height, int lineSize);
    ~VideoFrame();

public:

    quint32         width;
    quint32         height;
    quint32         lineSize;
    QVector<quint8> frameData;
};

// -----------------------------------------------------------------

class VideoThumbWriter
{
public:

    explicit VideoThumbWriter();
    ~VideoThumbWriter();

    void writeFrame(VideoFrame& frame, QImage& image);

private:

    // Disable
    VideoThumbWriter(const VideoThumbWriter&)            = delete;
    VideoThumbWriter& operator=(const VideoThumbWriter&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_VIDEO_THUMB_WRITER_H
