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

#ifndef QTAV_VIDEO_RENDERER_P_H
#define QTAV_VIDEO_RENDERER_P_H

#include "VideoRenderer.h"

// Qt includes

#include <QMutex>
#include <QRect>
#include <QColor>

// Local includes

#include "QtAV_Statistics.h"
#include "VideoFrame.h"
#include "AVOutput_p.h"

/* TODO:
 * Region of Interest(ROI)
 * use matrix to compute out rect, mapped point etc
 */

class QObject;
class QWidget;

namespace QtAV
{

class Filter;

class DIGIKAM_EXPORT VideoRendererPrivate : public AVOutputPrivate
{
public:

    VideoRendererPrivate()
      : AVOutputPrivate         (),
        update_background       (true),
        renderer_width          (480),
        renderer_height         (320),
        source_aspect_ratio     (0),
        src_width               (0),
        src_height              (0),
        aspect_ratio_changed    (true),     ///< to set the initial parameters
        out_aspect_ratio_mode   (VideoAspectRatio),
        out_aspect_ratio        (0),
        quality                 (QualityBest),
        preferred_format        (VideoFormat::Format_RGB32),
        force_preferred         (false),
        brightness              (0),
        contrast                (0),
        hue                     (0),
        saturation              (0),
        bg_color                (0, 0, 0),
        orientation             (0)
    {
/*
        conv.setInFormat(PIX_FMT_YUV420P);
        conv.setOutFormat(PIX_FMT_BGR32); // TODO: why not RGB32?
*/
    }

    virtual ~VideoRendererPrivate()
    {
    }

    // return true if video rect changed

    bool computeOutParameters(qreal outAspectRatio)
    {
        qreal rendererAspectRatio = qreal(renderer_width) / qreal(renderer_height);
        const QRect out_rect0(out_rect);

        if (out_aspect_ratio_mode == RendererAspectRatio)
        {
            out_aspect_ratio = rendererAspectRatio;
            out_rect         = QRect(0, 0, renderer_width, renderer_height);

            return (out_rect0 != out_rect);
        }

        // dar: displayed aspect ratio in video renderer orientation

        int rotate = orientation;

        if (statistics)
        {
            rotate += int(statistics->video_only.rotate);
        }

        const qreal dar = (rotate % 180) ? (1.0 / outAspectRatio)
                                         : outAspectRatio;
/*
        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("out rect: %f %dx%d ==>",
                out_aspect_ratio, out_rect.width(), out_rect.height());
*/
        if      (rendererAspectRatio >= dar)
        {
            // equals to original video aspect ratio here, also equals to out ratio
            // renderer is too wide, use renderer's height, horizonal align center

            const int h = renderer_height;
            const int w = qRound(dar * qreal(h));
            out_rect    = QRect((renderer_width - w) / 2, 0, w, h);
        }
        else
        {
            // rendererAspectRatio < dar : renderer is too high, use renderer's width

            const int w = renderer_width;
            const int h = qRound(qreal(w) / dar);
            out_rect    = QRect(0, (renderer_height - h) / 2, w, h);
        }

        out_aspect_ratio = outAspectRatio;
/*
        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("%f %dx%d <<<<<<<<",
            out_aspect_ratio, out_rect.width(), out_rect.height());
*/
        return (out_rect0 != out_rect);
    }

    virtual void setupQuality()
    {
    }

    int rotation() const
    {
        if (!statistics)
            return orientation;

        return (statistics->video_only.rotate + orientation);
    }

public:

    // draw background when necessary, for example, renderer is resized. Then set to false

    bool                                update_background;

    // width, height: the renderer's size. i.e. size of video frame with the value with borders
    // TODO: rename to renderer_width/height

    int                                 renderer_width, renderer_height;
    qreal                               source_aspect_ratio;
    int                                 src_width, src_height; // TODO: in_xxx
    QMutex                              img_mutex;

    // for both source, out aspect ratio. because source change
    // may result in out change if mode is VideoAspectRatio

    bool                                aspect_ratio_changed;
    OutAspectRatioMode                  out_aspect_ratio_mode;
    qreal                               out_aspect_ratio;
    Quality                             quality;

    // out_rect: the displayed video frame out_rect in the renderer

    QRect                               out_rect; // TODO: out_out_rect
    QRectF                              roi;

    VideoFrame                          video_frame;
    VideoFormat::PixelFormat            preferred_format;
    bool                                force_preferred;

    qreal                               brightness, contrast, hue, saturation;
    QColor                              bg_color;

private:

    int                                 orientation;

    friend class VideoRenderer;
};

} // namespace QtAV

#endif // QTAV_VIDEO_RENDERER_P_H
