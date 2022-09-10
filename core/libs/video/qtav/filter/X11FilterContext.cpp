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

#include "FilterContext.h"

// Qt includes

#include <QBuffer>
#include <QFontMetrics>
#include <QImage>
#include <QPainter>
#include <QTextDocument>
#include <QMatrix4x4>
#include "VideoFrame.h"

// Local includes

#include "digikam_debug.h"

// X11 includes

#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace QtAV
{

X11FilterContext::X11FilterContext()
    : VideoFilterContext(),
      doc       (nullptr),
      cvt       (nullptr),
      display   (nullptr),
      gc        (nullptr),
      drawable  (0),
      text_img  (nullptr),
      mask_img  (nullptr),
      mask_pix  (0),
      plain     (0)
{
    painter = new QPainter();
}

X11FilterContext::~X11FilterContext()
{
    if (doc)
    {
        delete doc;
        doc = nullptr;
    }

    if (cvt)
    {
        delete cvt;
        cvt = nullptr;
    }

    resetX11();
}

void X11FilterContext::destroyX11Resources()
{
    if (mask_pix)
    {
        XFreePixmap((::Display*)display, (::Pixmap)mask_pix);
        mask_pix = 0;
    }

    if (mask_img)
    {
        ((::XImage*)mask_img)->data = nullptr; // pointed to qimage data
        XDestroyImage((::XImage*)mask_img);
        mask_img                    = nullptr;
    }

    if (text_img)
    {
        ((::XImage*)text_img)->data = nullptr; // pointed to qimage data
        XDestroyImage((::XImage*)text_img);
        text_img                    = nullptr;
    }
}

void X11FilterContext::resetX11(Display* dpy, GC g, Drawable d)
{
    if ((dpy != display) || (g != gc) || (d != drawable))
    {
        destroyX11Resources();
        display  = dpy;
        gc       = g;
        drawable = d;
    }

    qCDebug(DIGIKAM_QTAV_LOG).noquote()
        << QString::asprintf("resetX11 display:%p,gc:%p", display, g)
        << QString::fromLatin1("drawable:%1").arg(d);
}

void X11FilterContext::renderTextImageX11(QImage* img, const QPointF& pos)
{
    if (img)
    {
        destroyX11Resources();
        mask_q = img->createAlphaMask();

        if (mask_q.isNull())
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("mask image is null");

            return;
        }

        XWindowAttributes xwa;
        XGetWindowAttributes((::Display*)display, (::Window)drawable, &xwa);

        // force the stride to ensure we can safely set ximage data ptr to qimage data ptr

        mask_img = (XImage*)XCreateImage((::Display*)display,
                                         xwa.visual, 1, ZPixmap, 0,
                                         nullptr,
                                         mask_q.width(), mask_q.height(),
                                         8, mask_q.bytesPerLine());

        if (!mask_img)
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote() << QString::asprintf("error create mask image");

            return;
        }

        ((::XImage*)mask_img)->data = (char*)mask_q.constBits();

        // force the stride to ensure we can safely set ximage data ptr to qimage data ptr

        text_img     = (XImage*)XCreateImage((::Display*)display,
                                             xwa.visual, xwa.depth, ZPixmap,
                                             0, nullptr,
                                             img->width(), img->height(),
                                             8, img->bytesPerLine());

        ((::XImage*)text_img)->data = (char*)img->constBits();

        mask_pix     = XCreatePixmap((::Display*)display,
                                     drawable,
                                     ((::XImage*)mask_img)->width, ((::XImage*)mask_img)->height,
                                     ((::XImage*)mask_img)->depth);

        ::GC mask_gc = XCreateGC((::Display*)display, (::Pixmap)mask_pix, 0, nullptr);

        XPutImage((::Display*)display,
                  mask_pix, mask_gc,
                  (::XImage*)mask_img,
                  0, 0, 0, 0,
                  ((::XImage*)mask_img)->width, ((::XImage*)mask_img)->height);
    }

    XSetClipMask((::Display*)display, (::GC)gc, (::Pixmap)mask_pix);
    XSetClipOrigin((::Display*)display, (::GC)gc, pos.x(), pos.y());

    XPutImage((::Display*)display,
              drawable, (::GC)gc,
              (::XImage*)text_img, 0, 0, pos.x(), pos.y(),
              ((::XImage*)text_img)->width, ((::XImage*)text_img)->height);

    XSetClipMask((::Display*)display, (::GC)gc, None);
    XSync((::Display*)display, False);
}

bool X11FilterContext::isReady() const
{
    return !!painter;
}

bool X11FilterContext::prepare()
{
    if (!isReady())
        return false;

    //painter->save(); //is it necessary?

    painter->setBrush(brush);
    painter->setPen(pen);
    painter->setFont(font);
    painter->setOpacity(opacity);

#if 1

    if (!clip_path.isEmpty())
    {
        painter->setClipPath(clip_path);
    }

    // transform at last! because clip_path is relative to paint device coord

    painter->setTransform(transform);

#endif

    return true;
}

void X11FilterContext::initializeOnFrame(VideoFrame* vframe)
{
    Q_UNUSED(vframe);
}

void X11FilterContext::shareFrom(VideoFilterContext* vctx)
{
    VideoFilterContext::shareFrom(vctx);
    X11FilterContext* const c = static_cast<X11FilterContext*>(vctx);
    display                   = c->display;
    gc                        = c->gc;
    drawable                  = c->drawable;

    // can not set other filter's context text
}

void X11FilterContext::drawImage(const QPointF& /*pos*/,
                                 const QImage& /*image*/,
                                 const QRectF& /*source*/,
                                 Qt::ImageConversionFlags /*flags*/)
{
    // qpainter transform etc
}

void X11FilterContext::drawImage(const QRectF& /*target*/,
                                 const QImage& /*image*/,
                                 const QRectF& /*source*/,
                                 Qt::ImageConversionFlags /*flags*/)
{
}

void X11FilterContext::drawPlainText(const QPointF& pos, const QString& text)
{
    if ((text == this->text) && plain && mask_pix)
    {
        renderTextImageX11(nullptr, pos);

        return;
    }

    this->text  = text;
    this->plain = true;

    QFontMetrics fm(font);
    text_q      = QImage(fm.horizontalAdvance(text), fm.height(), QImage::Format_ARGB32);
    text_q.fill(0);
    painter->begin(&text_q);
    painter->translate(0, 0);
    prepare();
    painter->drawText(QPoint(0, fm.ascent()), text);
    painter->end();
    renderTextImageX11(&text_q, pos);
}

void X11FilterContext::drawPlainText(const QRectF& rect, int flags, const QString& text)
{
     if (text.isEmpty())
         return;

     if (rect.isEmpty())
     {
         drawPlainText(rect.topLeft(), text);
         return;
     }

     if (test_img.size() != rect.size().toSize())
     {
         test_img = QImage(rect.size().toSize(), QImage::Format_ARGB32); // TODO: create once
     }

     painter->begin(&test_img);
     prepare();
     QRectF br = painter->boundingRect(rect, flags, text);
     painter->end();

     if (br.isEmpty())
         return;

    if ((text == this->text) && plain && mask_pix)
    {
        renderTextImageX11(nullptr, br.topLeft());
        return;
    }

    this->text  = text;
    this->plain = true;

    text_q      = QImage(br.size().toSize(), QImage::Format_ARGB32);
    text_q.fill(0);
    painter->begin(&text_q);
    prepare();
    painter->drawText(0, 0, br.width(), br.height(), Qt::AlignCenter, text);
    painter->end();
    renderTextImageX11(&text_q, br.topLeft());
}

void X11FilterContext::drawRichText(const QRectF& rect, const QString& text, bool wordWrap)
{
    Q_UNUSED(rect);
    Q_UNUSED(text);

    if ((text == this->text) && plain && mask_pix)
    {
        renderTextImageX11(nullptr, rect.topLeft());

        return;
    }

    this->text  = text;
    this->plain = false;

    if (!doc)
        doc = new QTextDocument();

    doc->setHtml(text);
/*
    painter->translate(rect.topLeft()); //drawContent() can not set target rect, so move here
*/
    if (wordWrap)
        doc->setTextWidth(rect.width());

    QMatrix4x4 m(transform);
    const QRectF r   = m.mapRect(QRectF(rect.x(), rect.y(), doc->size().width(), doc->size().height()));
    text_q           = QImage(r.size().toSize(), QImage::Format_ARGB32);
    text_q.fill(0);
    painter->begin(&text_q);
    prepare();
    const QPointF tl = m.map(rect.topLeft());
    m.setColumn(3, QVector4D(0, 0, 0, 1));      // reset O to let painter draw from 0
    const QPointF dp =  tl - r.topLeft();       // painter should start from the mapped top left relative to mapped rect's top left
/*
    qCDebug(DIGIKAM_QTAV_LOG) << dp << r.;
*/
    painter->setTransform(m.toTransform());
    painter->translate(dp);

    doc->drawContents(painter);
    painter->end();
    renderTextImageX11(&text_q, r.topLeft());   // TODO: use boundingRect?
}

} // namespace QtAV
