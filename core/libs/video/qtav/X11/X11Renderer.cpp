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

// TODO: ROI (xsubimage?), rotation. slow scale because of alignment? no xsync for shm

// Local includes

/**
 * X11 headers define 'Bool' type which is used in qmetatype.h. we must include X11 files at last,
 * i.e. X11Renderer_p.h. otherwise compile error
 */

#include "VideoRenderer_p.h"

// C++ includes

#include <unistd.h> // usleep

// Qt includes

#include <QWidget>
#include <QResizeEvent>
#include <qmath.h>
#include <QTextStream>

// Local includes

#include "FilterContext.h"
#include "digikam_debug.h"

// X11 includes

#include <sys/shm.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>

// scale: http://www.opensource.apple.com/source/X11apps/X11apps-14/xmag/xmag-X11R7.0-1.0.1/Scale.c     // krazy:exclude=insecurenet

#define FFALIGN(x, a) (((x)+(a)-1)&~((a)-1))

static const int kPoolSize = 2;

namespace QtAV
{

class X11RendererPrivate;

class Q_DECL_HIDDEN X11Renderer : public QWidget,
                                  public VideoRenderer
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(X11Renderer)

public:

    X11Renderer(QWidget* const parent = nullptr,
                Qt::WindowFlags f = Qt::WindowFlags(Qt::Widget));

    VideoRendererId id()                              const override;
    bool isSupported(VideoFormat::PixelFormat pixfmt) const override;

    /* WA_PaintOnScreen: To render outside of Qt's paint system, e.g. If you require
     * native painting primitives, you need to reimplement QWidget::paintEngine() to
     * return 0 and set this flag
     * If paintEngine != 0, the window will flicker when drawing without QPainter.
     * Make sure that paintEngine returns 0 to avoid flicking.
     */
    QPaintEngine* paintEngine()                       const override;

    /* http://lists.trolltech.com/qt4-preview-feedback/2005-04/thread00609-0.html       // krazy:exclude=insecurenet
     * true: paintEngine is QPainter. Painting with QPainter support double buffer
     * false: no double buffer, should reimplement paintEngine() to return 0 to avoid flicker
     */
    QWidget* widget()                                       override
    {
        return this;
    }

    DECLARE_VIDEO_RENDERER_SIGNALS

protected:

    bool receiveFrame(const VideoFrame& frame)              override;

    // called in paintEvent before drawFrame() when required

    void drawBackground()                                   override;

    // draw the current frame using the current paint engine. called by paintEvent()

    void drawFrame()                                        override;
    void paintEvent(QPaintEvent*)                           override;
    void resizeEvent(QResizeEvent*)                         override;

    // stay on top will change parent, hide then show(windows)

    void showEvent(QShowEvent*)                             override;

    DECLARE_VIDEO_RENDERER_EMIT_METHODS
};

typedef X11Renderer VideoRendererX11;

extern VideoRendererId VideoRendererId_X11;

#if 0

FACTORY_REGISTER_ID_AUTO(VideoRenderer, X11, "X11")

#else

bool RegisterVideoRendererX11_Man()
{
    return VideoRenderer::Register<X11Renderer>(VideoRendererId_X11, "X11");
}

#endif

VideoRendererId X11Renderer::id() const
{
    return VideoRendererId_X11;
}

#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
#   define BO_NATIVE    LSBFirst
#   define BO_NONNATIVE MSBFirst
#else
#   define BO_NATIVE    MSBFirst
#   define BO_NONNATIVE LSBFirst
#endif

static const struct Q_DECL_HIDDEN fmt2Xfmtentry
{
    VideoFormat::PixelFormat fmt        = VideoFormat::Format_Invalid;
    int                      byte_order = BO_NATIVE;
    unsigned                 red_mask   = 0;
    unsigned                 green_mask = 0;
    unsigned                 blue_mask  = 0;
}
fmt2Xfmt[] =
{
    { VideoFormat::Format_BGR555,  BO_NATIVE,    0x0000001F, 0x000003E0, 0x00007C00 },
    { VideoFormat::Format_BGR555,  BO_NATIVE,    0x00007C00, 0x000003E0, 0x0000001F },
    { VideoFormat::Format_BGR565,  BO_NATIVE,    0x0000001F, 0x000007E0, 0x0000F800 },
    { VideoFormat::Format_RGB565,  BO_NATIVE,    0x0000F800, 0x000007E0, 0x0000001F },
    { VideoFormat::Format_RGB24,   MSBFirst,     0x00FF0000, 0x0000FF00, 0x000000FF },
    { VideoFormat::Format_RGB24,   LSBFirst,     0x000000FF, 0x0000FF00, 0x00FF0000 },
    { VideoFormat::Format_BGR24,   MSBFirst,     0x000000FF, 0x0000FF00, 0x00FF0000 },
    { VideoFormat::Format_BGR24,   LSBFirst,     0x00FF0000, 0x0000FF00, 0x000000FF },
    { VideoFormat::Format_BGR32,   BO_NATIVE,    0x000000FF, 0x0000FF00, 0x00FF0000 },
    { VideoFormat::Format_BGR32,   BO_NONNATIVE, 0xFF000000, 0x00FF0000, 0x0000FF00 }, // abgr rgba
    { VideoFormat::Format_RGB32,   BO_NATIVE,    0x00FF0000, 0x0000FF00, 0x000000FF }, // argb bgra
    { VideoFormat::Format_RGB32,   BO_NONNATIVE, 0x0000FF00, 0x00FF0000, 0xFF000000 },
    { VideoFormat::Format_ARGB32,  MSBFirst,     0x00FF0000, 0x0000FF00, 0x000000FF },
    { VideoFormat::Format_ARGB32,  LSBFirst,     0x0000FF00, 0x00FF0000, 0xFF000000 },
    { VideoFormat::Format_ABGR32,  MSBFirst,     0x000000FF, 0x0000FF00, 0x00FF0000 },
    { VideoFormat::Format_ABGR32,  LSBFirst,     0xFF000000, 0x00FF0000, 0x0000FF00 },
    { VideoFormat::Format_RGBA32,  MSBFirst,     0xFF000000, 0x00FF0000, 0x0000FF00 },
    { VideoFormat::Format_RGBA32,  LSBFirst,     0x000000FF, 0x0000FF00, 0x00FF0000 },
    { VideoFormat::Format_BGRA32,  MSBFirst,     0x0000FF00, 0x00FF0000, 0xFF000000 },
    { VideoFormat::Format_BGRA32,  LSBFirst,     0x00FF0000, 0x0000FF00, 0x000000FF },
    { VideoFormat::Format_Invalid, BO_NATIVE,    0,          0,          0          }
};

VideoFormat::PixelFormat pixelFormat(XImage* xi)
{
    const struct fmt2Xfmtentry* fmte = fmt2Xfmt;

    while (fmte->fmt != VideoFormat::Format_Invalid)
    {
        int depth = VideoFormat(fmte->fmt).bitsPerPixel();

        // 15->16? mpv

        if (
            (depth            == xi->bits_per_pixel) &&
            (fmte->byte_order == xi->byte_order)     &&
            (fmte->red_mask   == xi->red_mask)       &&
            (fmte->green_mask == xi->green_mask)     &&
            (fmte->blue_mask  == xi->blue_mask)
           )
        {
            break;
        }
/*
        qCDebug(DIGIKAM_QTAV_LOG) << fmte->fmt;
*/
        fmte++;
    }

    return fmte->fmt;
}

class Q_DECL_HIDDEN X11RendererPrivate : public VideoRendererPrivate
{
public:

    DPTR_DECLARE_PUBLIC(X11Renderer)

    X11RendererPrivate()
    {
        XInitThreads();

#ifndef _XSHM_H_

        use_shm = false;

#endif
/*
        XvQueryExtension()
*/
        char* const dispName = XDisplayName(nullptr);

        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("X11 open display: %s",
                dispName);

        display        = XOpenDisplay(dispName);

        if (!display)
        {
            available = false;

            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("Open X11 display error");

            return;
        }

        XWindowAttributes attribs;
        XGetWindowAttributes(display, DefaultRootWindow(display), &attribs);
        depth = attribs.depth;

        if (!XMatchVisualInfo(display, DefaultScreen(display), depth, TrueColor, &vinfo))
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("XMatchVisualInfo error");

            available = false;

            return;
        }

        XImage* ximg = nullptr;

        if ((depth != 15) && (depth != 16) && (depth != 24) && (depth != 32))
        {
/*
            Visual* vs;

            //depth = // find_depth_from_visuals
*/
        }
        else
        {
            ximg = XGetImage(display, DefaultRootWindow(display), 0, 0, 1, 1, AllPlanes, ZPixmap);
        }

        int ximage_depth  = depth;
        unsigned int mask = 0;

        if (ximg)
        {
            bpp = ximg->bits_per_pixel;

            if ((ximage_depth + 7) / 8 != (bpp + 7) / 8) // 24bpp use 32 depth
                ximage_depth = bpp;

            mask = (ximg->red_mask | ximg->green_mask | ximg->blue_mask);

            qCDebug(DIGIKAM_QTAV_LOG)
                << QString::fromLatin1("color mask: %1 %2 %3 %4")
                    .arg(mask)
                    .arg(ximg->red_mask)
                    .arg(ximg->green_mask)
                    .arg(ximg->blue_mask);

            XDestroyImage(ximg);
        }

        if (((ximage_depth + 7) / 8) == 2)
        {
            if      (mask == 0x7FFF)
                ximage_depth = 15;
            else if (mask == 0xFFFF)
                ximage_depth = 16;
        }

        Q_UNUSED(ximage_depth);
    }

    ~X11RendererPrivate()
    {
        for (int i = 0 ; i < kPoolSize ; ++i)
            destroyX11Image(i);

        XCloseDisplay(display);
    }

    void destroyX11Image(int index)
    {
        if (use_shm)
        {
            XShmSegmentInfo& shm = shm_pool[index];

            if (shm.shmaddr)
            {
                XShmDetach(display, &shm);
                shmctl(shm.shmid, IPC_RMID, nullptr);
                shmdt(shm.shmaddr);
            }
        }

        XImage* const ximage = ximage_pool[index];

        if (ximage)
        {
            if (!ximage_data[index].isEmpty())
                ximage->data = nullptr; // we point it to our own data if shm is not used

            XDestroyImage(ximage);
        }

        ximage_pool[index] = nullptr;
        ximage_data[index].clear();
    }

    bool prepareDeviceResource()
    {
        if (gc)
        {
            XFreeGC(display, gc);
            gc = nullptr;
        }

        gc = XCreateGC(display, q_func().winId(), 0, nullptr); // DefaultRootWindow

        if (!gc)
        {
            available = false;

            qCCritical(DIGIKAM_QTAV_LOG_CRITICAL)
                << QString::asprintf("Create GC failed!");

            return false;
        }

        XSetBackground(display, gc, BlackPixel(display, DefaultScreen(display)));

        if (filter_context)
            ((X11FilterContext*)filter_context)->resetX11((X11FilterContext::Display*)display,
                                                          (X11FilterContext::GC)gc,
                                                          (X11FilterContext::Drawable)q_func().winId());

        return true;
    }

    bool ensureImage(int w, int h)
    {
        for (int i = 0 ; i < kPoolSize ; ++i)
        {
            if (!ensureImage(i, w, h))
                return false;
        }

        return true;
    }

    bool ensureImage(int index, int w, int h)
    {
        XImage*& ximage = ximage_pool[index];

        if (ximage && (ximage->width == w) && (ximage->height == h))
            return true;

        warn_bad_pitch       = true;
        destroyX11Image(index);
        use_shm              = XShmQueryExtension(display);
        XShmSegmentInfo &shm = shm_pool[index];

        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("use x11 shm: %d",
                use_shm);

        if (!use_shm)
            goto no_shm;

        ShmCompletionEvent = XShmGetEventBase(display) + ShmCompletion;

        // data seems not aligned

        ximage = XShmCreateImage(display, vinfo.visual, depth, ZPixmap, nullptr, &shm, w, h);

        if (!ximage)
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("XShmCreateImage error");

            goto no_shm;
        }

        shm.shmid = shmget(IPC_PRIVATE, ximage->bytes_per_line*ximage->height, IPC_CREAT | 0777);

        if (shm.shmid < 0)
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("shmget error");

            goto no_shm;
        }

        shm.shmaddr = (char *)shmat(shm.shmid, nullptr, 0);

        if (shm.shmaddr == (char*)-1)
        {
            if (!ximage_data[index].isEmpty())
                   ximage->data = nullptr;

            XDestroyImage(ximage);
            ximage = nullptr;
            ximage_data[index].clear();

            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("Shared memory error, disabling (seg id error)");

            goto no_shm;
        }

        ximage->data = shm.shmaddr;
        shm.readOnly = False;

        if (!XShmAttach(display, &shm))
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("Attach to shm failed! try to use none shm");

            goto no_shm;
        }

        XSync(display, False);
        shmctl(shm.shmid, IPC_RMID, nullptr);
        pixfmt       = pixelFormat(ximage);

        return true;

no_shm:

        ShmCompletionEvent = 0;
        ximage             = XCreateImage(display, vinfo.visual, depth,
                                          ZPixmap, 0, nullptr, w, h, 8, 0);

        if (!ximage)
            return false;

        pixfmt       = pixelFormat(ximage);
        ximage->data = nullptr;
        XSync(display, False);

        // TODO: align 16 or?

        ximage_data[index].resize(ximage->bytes_per_line * ximage->height + 32);

        return true;
    }

    int resizeXImage(int index);

public:

    bool                        use_shm                 = true;             // TODO: set by user
    bool                        warn_bad_pitch          = true;
    unsigned int                num_adaptors            = 0;
    int                         bpp                     = 0;
    int                         depth                   = 0;
    int                         ShmCompletionEvent      = 0;
    int                         ShmCompletionWaitCount  = 0;
    XVisualInfo                 vinfo;
    Display*                    display                 = nullptr;
    int                         current_index           = 0;
    int                         next_index              = 0;
    XImage*                     ximage_pool[kPoolSize]  = { nullptr };
    GC                          gc                      = nullptr;
    XShmSegmentInfo             shm_pool[kPoolSize];
    VideoFormat::PixelFormat    pixfmt                  = VideoFormat::Format_Invalid;

    // if the incoming image pitchs are different from ximage ones, use ximage pitchs and copy data in ximage_data

    QByteArray                  ximage_data[kPoolSize];
    VideoFrame                  frame_orig;                                 // if renderer is resized, scale the original frame
    bool                        frame_changed           = false;
};

X11Renderer::X11Renderer(QWidget* const parent, Qt::WindowFlags f)
    : QWidget      (parent, f),
      VideoRenderer(*new X11RendererPrivate())
{
    DPTR_INIT_PRIVATE(X11Renderer);

    setAcceptDrops(true);
    setFocusPolicy(Qt::StrongFocus);

    /*
     * To rapidly update custom widgets that constantly paint over their entire areas with
     * opaque content, e.g., video streaming widgets, it is better to set the widget's
     * Qt::WA_OpaquePaintEvent, avoiding any unnecessary overhead associated with repainting the
     * widget's background
     */
    setAttribute(Qt::WA_OpaquePaintEvent);
/*
    setAttribute(Qt::WA_NoSystemBackground);
    setAutoFillBackground(false);
*/
    setAttribute(Qt::WA_PaintOnScreen, true);
    d_func().filter_context = VideoFilterContext::create(VideoFilterContext::X11);

    if (!d_func().filter_context)
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("No filter context for X11");
    }
    else
    {
        d_func().filter_context->paint_device = this;
    }
}

bool X11Renderer::isSupported(VideoFormat::PixelFormat pixfmt) const
{
    Q_UNUSED(pixfmt);

    // if always return true, then convert to x11 format and scale in
    // receiveFrame() only once. no need to convert format first then scale

    return true; // ((pixfmt == d_func().pixfmt) && (d_func().pixfmt != VideoFormat::Format_Invalid));
}

bool X11Renderer::receiveFrame(const VideoFrame& frame)
{
    DPTR_D(X11Renderer);

    d.frame_changed = true;

    if (!frame.isValid())
    {
        d.video_frame = VideoFrame(); // fill background
        update();

        return true;
    }

    d.frame_orig  = frame;
    d.video_frame = frame; // must be set because it will be check isValid() somewhere else
    updateUi();

    return true;
}

int X11RendererPrivate::resizeXImage(int index)
{
    if (!frame_orig.isValid())
        return false;

    if (!frame_changed)
        return -1;

    // force align to 8(16?) because xcreateimage will not do alignment
    // GAL vmem is 16 aligned

    // we can also call it in onResizeRenderer, onSetOutAspectXXX

    if (!ensureImage(index, FFALIGN(out_rect.width(), 16), FFALIGN(out_rect.height(), 16)))
        return false;

    frame_changed   = false;
    XImage*& ximage = ximage_pool[index];
    video_frame     = frame_orig; // set before map!
    VideoFrame interopFrame;

    if (!frame_orig.constBits(0))
    {
        interopFrame = VideoFrame(ximage->width, ximage->height, pixelFormat(ximage));
        interopFrame.setBits(use_shm ? (quint8*)ximage->data : (quint8*)ximage_data[index].constData());
        interopFrame.setBytesPerLine(ximage->bytes_per_line);
    }

    if (
        frame_orig.constBits(0) ||
        // check pixel format and scale to ximage size&line_size
        !video_frame.map(UserSurface, &interopFrame, VideoFormat(VideoFormat::Format_RGB32))
       )
    {
        if (
            !frame_orig.constBits(0)                     || // always convert hw frames
            (frame_orig.pixelFormat() != pixfmt)         ||
            (frame_orig.width()       != ximage->width)  ||
            (frame_orig.height()      != ximage->height)
           )
        {
            video_frame = frame_orig.to(pixfmt, QSize(ximage->width, ximage->height));
        }
        else
        {
            video_frame = frame_orig;
        }

        if (video_frame.bytesPerLine(0) == ximage->bytes_per_line)
        {
            if (use_shm)
            {
                memcpy(ximage->data, video_frame.constBits(0), ximage->bytes_per_line*ximage->height);
            }
            else
            {
                ximage->data = (char*)video_frame.constBits(0);
            }
        }
        else
        {
            // copy line by line

            if (warn_bad_pitch)
            {
                warn_bad_pitch = false;

                qCDebug(DIGIKAM_QTAV_LOG)
                    << QString::fromLatin1("bad pitch: %1 - %2 image_data[%3].size: %4")
                        .arg(ximage->bytes_per_line)
                        .arg(video_frame.bytesPerLine(0))
                        .arg(index)
                        .arg(ximage_data[index].size());
            }

            quint8* dst = (quint8*)ximage->data;

            if (!use_shm)
            {
                dst          = (quint8*)ximage_data[index].constData();
                ximage->data = (char*)ximage_data[index].constData();
            }

            VideoFrame::copyPlane(dst, ximage->bytes_per_line,
                                  (const quint8*)video_frame.constBits(0),
                                  video_frame.bytesPerLine(0),
                                  ximage->bytes_per_line, ximage->height);
        }
    }
    else
    {
        if (!use_shm)
            ximage->data = (char*)ximage_data[index].constData();
    }

    return true;
}

QPaintEngine* X11Renderer::paintEngine() const
{
    return nullptr; // use native engine
}

void X11Renderer::drawBackground()
{
    const QRegion bgRegion(backgroundRegion());

    if (bgRegion.isEmpty())
        return;

    DPTR_D(X11Renderer);

    // TODO: fill once each resize? mpv
    // TODO: set color
/*
    XSetBackground(d.display, d.gc, BlackPixel(d.display, DefaultScreen(d.display)));
*/
    for (const QRect& r : bgRegion)
    {
        XFillRectangle(d.display, winId(), d.gc, r.x(), r.y(), r.width(), r.height());
    }

    XFlush(d.display); // apply the color
}

void X11Renderer::drawFrame()
{
    // TODO: interop

    DPTR_D(X11Renderer);

    int ret = d.resizeXImage(d.next_index); // -1: image no change

    if (!ret)
        return;

    if (preferredPixelFormat() != d.pixfmt)
    {
        qCDebug(DIGIKAM_QTAV_LOG) << "x11 preferred pixel format:"
                                         << d.pixfmt;

        setPreferredPixelFormat(d.pixfmt);
    }

    if (d.use_shm)
    {
        int wait_count = 0;

        while (d.ShmCompletionWaitCount >= kPoolSize)
        {
            if (wait_count++ > 100)
            {
                qCDebug(DIGIKAM_QTAV_LOG).noquote()
                    << QString::asprintf("reset ShmCompletionWaitCount");

                d.ShmCompletionWaitCount = 0;

                break;
            }

            while (XPending(d.display))
            {
                XEvent ev;
                XNextEvent(d.display, &ev);

                if (ev.type == d.ShmCompletionEvent)
                {
                    if (d.ShmCompletionWaitCount > 0)
                        d.ShmCompletionWaitCount--;
                }
            }

            usleep(1000);
        }
    }
    QRect roi = realROI();
    int idx   = d.current_index; // ret<0, frame/vo no change. if host frame, no filters; >0: filters

    if (ret > 0)
    {
        // next ximage is ready

        idx          = d.next_index;
        d.next_index = (d.next_index + 1) % kPoolSize;
    }

    XImage* const ximage = d.ximage_pool[idx];

    if (d.use_shm)
    {
        XShmPutImage(d.display, winId(), d.gc, ximage,
                     roi.x(), roi.y(),  // , roi.width(), roi.height()
                     d.out_rect.x(), d.out_rect.y(), d.out_rect.width(), d.out_rect.height(),
                     True               // true: send event
                    );

        d.ShmCompletionWaitCount++;
    }
    else
    {
        XPutImage(d.display, winId(), d.gc, ximage,
                  roi.x(), roi.y(), // roi.width(), roi.height(),
                  d.out_rect.x(), d.out_rect.y(), d.out_rect.width(), d.out_rect.height());

        XSync(d.display, False);    // update immediately
    }
}

void X11Renderer::paintEvent(QPaintEvent*)
{
    handlePaintEvent();
}

void X11Renderer::resizeEvent(QResizeEvent* e)
{
    DPTR_D(X11Renderer);

    d.frame_changed = true;
    resizeRenderer(e->size());
    update(); // update background
}

void X11Renderer::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
    DPTR_D(X11Renderer);

    /*
     * Do something that depends on widget below! e.g. recreate render target for direct2d.
     * When Qt::WindowStaysOnTopHint changed, window will hide first then show. If you
     * don't do anything here, the widget content will never be updated.
     */
    d.prepareDeviceResource();
}

IMPLEMENT_VIDEO_RENDERER_EMIT_METHODS(X11Renderer)

} // namespace QtAV

#include "X11Renderer.moc"
