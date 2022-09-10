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

#include "QPainterRenderer_p.h"

// Local includes

#include "FilterContext.h"

namespace QtAV
{

QPainterRenderer::QPainterRenderer()
    : VideoRenderer(*new QPainterRendererPrivate())
{
    DPTR_D(QPainterRenderer);

    d.filter_context = VideoFilterContext::create(VideoFilterContext::QtPainter);
}

QPainterRenderer::QPainterRenderer(QPainterRendererPrivate& d)
    : VideoRenderer(d)
{
    d.filter_context = VideoFilterContext::create(VideoFilterContext::QtPainter);
}

bool QPainterRenderer::isSupported(VideoFormat::PixelFormat pixfmt) const
{
    return (VideoFormat::imageFormatFromPixelFormat(pixfmt) != QImage::Format_Invalid);
}

bool QPainterRenderer::preparePixmap(const VideoFrame& frame)
{
    DPTR_D(QPainterRenderer);

    // already locked in a larger scope of receive()

    QImage::Format imgfmt = frame.imageFormat();

    if (frame.constBits(0))
    {
        d.video_frame = frame;
    }
    else
    {
        if (imgfmt == QImage::Format_Invalid)
        {
            d.video_frame = frame.to(VideoFormat::Format_RGB32);
            imgfmt        = d.video_frame.imageFormat();
        }
        else
        {
            d.video_frame = frame.to(frame.pixelFormat());
        }
    }

    const bool swapRGB = ((int)imgfmt < 0);

    if (swapRGB)
    {
        imgfmt = (QImage::Format)(-imgfmt);
    }

    // DO NOT use frameData().data() because it's temp ptr while
    // d.image does not deep copy the data

    QImage image = QImage((uchar*)d.video_frame.constBits(),
                          d.video_frame.width(),
                          d.video_frame.height(),
                          d.video_frame.bytesPerLine(),
                          imgfmt);

    if (swapRGB)
        image = image.rgbSwapped();

    d.pixmap = QPixmap::fromImage(image);

    // Format_RGB32 is fast. see document

    return true;
}

void QPainterRenderer::drawBackground()
{
    DPTR_D(QPainterRenderer);

    if (!d.painter)
        return;

    const QRegion bgRegion(backgroundRegion());

    if (bgRegion.isEmpty())
        return;

#if 1

    d.painter->save();
    d.painter->setClipRegion(bgRegion);
    d.painter->fillRect(QRect(QPoint(), rendererSize()), backgroundColor());
    d.painter->restore();

#else

    const QVector<QRect> bg(bgRegion.rects());

    Q_FOREACH (const QRect& r, bg)
    {
        d.painter->fillRect(r, backgroundColor());

    }

#endif

}

void QPainterRenderer::drawFrame()
{
    DPTR_D(QPainterRenderer);

    if (!d.painter)
        return;

    if (d.pixmap.isNull())
        return;

    QRect roi = realROI();

    if (d.rotation() == 0)
    {
        // assume that the image data is already scaled to out_size (NOT renderer size!)

        if (roi.size() == d.out_rect.size())
        {
            d.painter->drawPixmap(d.out_rect.topLeft(), d.pixmap, roi);
        }
        else
        {
            d.painter->drawPixmap(d.out_rect, d.pixmap, roi);

            // what's the difference?
/*
            d.painter->drawPixmap(QPoint(), d.pixmap.scaled(d.renderer_width, d.renderer_height));
*/
        }

        return;
    }

    // render to whole renderer rect in painter's transformed coordinate
    // scale ratio is different from gl based renderers. gl always fill the whole rect

    d.painter->save();
    d.painter->translate(rendererWidth() / 2, rendererHeight() / 2);

    // TODO: why rotate then scale gives wrong result?

    if (d.rotation() % 180)
    {
        d.painter->scale((qreal)d.out_rect.width()  / (qreal)rendererHeight(),
                         (qreal)d.out_rect.height() / (qreal)rendererWidth());
    }
    else
    {
        d.painter->scale((qreal)d.out_rect.width()  / (qreal)rendererWidth(),
                         (qreal)d.out_rect.height() / (qreal)rendererHeight());
    }

    d.painter->rotate(d.rotation());
    d.painter->translate(-rendererWidth() / 2, -rendererHeight() / 2);
    d.painter->drawPixmap(QRect(0, 0, rendererWidth(), rendererHeight()), d.pixmap, roi);
    d.painter->restore();
}

} // namespace QtAV
