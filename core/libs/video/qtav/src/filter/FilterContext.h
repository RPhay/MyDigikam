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

#ifndef QTAV_FILTER_CONTEXT_H
#define QTAV_FILTER_CONTEXT_H

// Qt includes

#include <QByteArray>
#include <QRect>
#include <QPainter>
#include <QPainterPath>

// Local includes

#include "QtAV_Global.h"

/*
 * QPainterFilterContext, D2DFilterContext, ...
 */

QT_BEGIN_NAMESPACE
class QPainter;
class QPaintDevice;
class QTextDocument;
QT_END_NAMESPACE

namespace QtAV
{

class VideoFrame;

class QTAV_EXPORT VideoFilterContext
{
public:

    enum Type
    {
        /// audio and video...

        QtPainter,
        OpenGL,
        Direct2D,   ///< Not implemeted
        GdiPlus,    ///< Not implemented
        X11,
        None        ///< user defined filters, no context can be used
    };

    static VideoFilterContext* create(Type t);
    VideoFilterContext();
    virtual ~VideoFilterContext();

    virtual Type type() const = 0;

    // map to Qt types
    // drawSurface?

    virtual void drawImage(const QPointF& pos, const QImage& image, const QRectF& source = QRectF(), Qt::ImageConversionFlags flags = Qt::AutoColor);

    // if target is null, draw image at target.topLeft(). if source is null, draw the whole image

    virtual void drawImage(const QRectF& target, const QImage& image, const QRectF& source = QRectF(), Qt::ImageConversionFlags flags = Qt::AutoColor);
    virtual void drawPlainText(const QPointF& pos, const QString& text);

    // if rect is null, draw single line text at rect.topLeft(), ignoring flags

    virtual void drawPlainText(const QRectF& rect, int flags, const QString& text);
    virtual void drawRichText(const QRectF& rect, const QString& text, bool wordWrap = true);

    /**
     * TODO: x, y, width, height: |?|>=1 is in pixel unit, otherwise is ratio of video context rect
     * filter.x(a).y(b).width(c).height(d)
     */
    QRectF          rect;

    // Fallback to QPainter if no other paint engine implemented

    QPainter*       painter;                    // TODO: shared_ptr?
    qreal           opacity;
    QTransform      transform;
    QPainterPath    clip_path;
    QFont           font;
    QPen            pen;
    QBrush          brush;

    /*
     * for the filters apply on decoded data, paint_device must be initialized once the data changes
     * can we allocate memory on stack?
     */
    QPaintDevice*   paint_device;
    int             video_width, video_height;  // original size

protected:

    bool            own_painter;
    bool            own_paint_device;

protected:

    virtual bool isReady() const = 0;

    // save the state then setup new parameters

    virtual bool prepare() = 0;

    virtual void initializeOnFrame(VideoFrame* frame); // private?

    // share qpainter etc.

    virtual void shareFrom(VideoFilterContext* vctx);
    friend class VideoFilter;
};

class VideoFrameConverter;

// TODO: font, pen, brush etc?

class QTAV_EXPORT QPainterFilterContext final: public VideoFilterContext
{
public:

    QPainterFilterContext();
    virtual ~QPainterFilterContext();

    Type type() const override
    {
        return VideoFilterContext::QtPainter;
    }

    // empty source rect equals to the whole source rect

    void drawImage(const QPointF& pos, const QImage& image, const QRectF& source = QRectF(), Qt::ImageConversionFlags flags = Qt::AutoColor)   override;
    void drawImage(const QRectF& target, const QImage& image, const QRectF& source = QRectF(), Qt::ImageConversionFlags flags = Qt::AutoColor) override;
    void drawPlainText(const QPointF& pos, const QString& text)                                                                                override;

    // if rect is null, draw single line text at rect.topLeft(), ignoring flags

    void drawPlainText(const QRectF& rect, int flags, const QString& text)           override;
    void drawRichText(const QRectF& rect, const QString& text, bool wordWrap = true) override;

protected:

    bool isReady() const                        override;
    bool prepare()                              override;
    void initializeOnFrame(VideoFrame *vframe)  override;

protected:

    QTextDocument*       doc;
    VideoFrameConverter* cvt;
};

#if QTAV_HAVE(X11)

class QTAV_EXPORT X11FilterContext final : public VideoFilterContext
{
public:

    typedef struct  _XDisplay Display;
    typedef struct  _XGC *GC;
    typedef         quintptr Drawable;
    typedef         quintptr Pixmap;
    struct          XImage;

    X11FilterContext();
    virtual ~X11FilterContext();

    Type type() const override
    {
        return VideoFilterContext::X11;
    }

    void resetX11(Display* dpy = 0, GC g = 0, Drawable d = 0);

    // empty source rect equals to the whole source rect

    void drawImage(const QPointF& pos, const QImage& image, const QRectF& source = QRectF(), Qt::ImageConversionFlags flags = Qt::AutoColor) override;
    void drawImage(const QRectF& target, const QImage& image, const QRectF& source = QRectF(), Qt::ImageConversionFlags flags = Qt::AutoColor) override;
    void drawPlainText(const QPointF& pos, const QString& text) override;

    // if rect is null, draw single line text at rect.topLeft(), ignoring flags

    void drawPlainText(const QRectF& rect, int flags, const QString& text) override;
    void drawRichText(const QRectF& rect, const QString& text, bool wordWrap = true) override;

protected:

    bool isReady() const override;
    bool prepare() override;
    void initializeOnFrame(VideoFrame *vframe) override;
    void shareFrom(VideoFilterContext *vctx) override;

    // null image: use the old x11 image/pixmap

    void renderTextImageX11(QImage* img, const QPointF &pos);
    void destroyX11Resources();

protected:

    QTextDocument*          doc;
    VideoFrameConverter*    cvt;

    Display*                display;
    GC                      gc;
    Drawable                drawable;
    XImage*                 text_img;
    XImage*                 mask_img;
    Pixmap                  mask_pix;
    QImage                  text_q;
    QImage                  mask_q;

    bool                    plain;
    QString                 text;
    QImage                  test_img; // for computing bounding rect
};

#endif // QTAV_HAVE(X11)

} // namespace QtAV

#endif // QTAV_FILTER_CONTEXT_H
