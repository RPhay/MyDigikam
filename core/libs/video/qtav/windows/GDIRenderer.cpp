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

#include "VideoRenderer_p.h"

// Qt includes

#include <QWidget>
#include <QResizeEvent>

// Local includes

#include "QtAV_factory.h"
#include "digikam_debug.h"

// Windows includes

#ifndef WIN32_LEAN_AND_MEAN     // krazy:exclude=cpp
#   define WIN32_LEAN_AND_MEAN
#endif

#include <algorithm>
#include <windows.h>
#include <winsock2.h>
#include <unknwn.h>

#ifndef __MINGW32__             // krazy:exclude=cpp

#   ifndef min
#       define min(x,y) ((x) < (y) ? (x) : (y))
#   endif

#   ifndef max
#       define max(x,y) ((x) > (y) ? (x) : (y))
#   endif

#endif

#include <gdiplus.h>

#define USE_GRAPHICS 0

// http://msdn.microsoft.com/en-us/library/ms927613.aspx        // krazy:exclude=insecurenet
// #pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

namespace QtAV
{

class GDIRendererPrivate;

class Q_DECL_HIDDEN GDIRenderer : public QWidget,
                                  public VideoRenderer
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(GDIRenderer)

public:

    GDIRenderer(QWidget* const parent = nullptr,
                Qt::WindowFlags f = Qt::WindowFlags(Qt::Widget)); // offscreen?

    VideoRendererId id()                              const override;
    bool isSupported(VideoFormat::PixelFormat pixfmt) const override;

    /*
     * WA_PaintOnScreen: To render outside of Qt's paint system, e.g. If you require
     * native painting primitives, you need to reimplement QWidget::paintEngine() to
     * return 0 and set this flag
     */
    QPaintEngine* paintEngine()                       const override;

    /*
     * http://lists.trolltech.com/qt4-preview-feedback/2005-04/thread00609-0.html       // krazy:exclude=insecurenet
     * true: paintEngine.getDC(), double buffer is enabled by defalut.
     * false: GetDC(winId()), no double buffer, should reimplement paintEngine()
     */
    QWidget* widget()                                       override
    {
        return this;
    }

    DECLARE_VIDEO_RENDERER_SIGNALS

protected:

    bool receiveFrame(const VideoFrame& frame)              override;
    void drawBackground()                                   override;
    void drawFrame()                                        override;

    /*
     * usually you don't need to reimplement paintEvent, just drawXXX() is ok. unless you want do all
     * things yourself totally
     */

    void paintEvent(QPaintEvent*)                           override;
    void resizeEvent(QResizeEvent*)                         override;

    // stay on top will change parent, hide then show(windows). we need GetDC() again

    void showEvent(QShowEvent*)                             override;

    DECLARE_VIDEO_RENDERER_EMIT_METHODS
};

typedef GDIRenderer VideoRendererGDI;

extern VideoRendererId VideoRendererId_GDI;

#if 0

FACTORY_REGISTER_ID_AUTO(VideoRenderer, GDI, "GDI")

#else

bool RegisterVideoRendererGDI_Man()
{
    return VideoRenderer::Register<GDIRenderer>(VideoRendererId_GDI, "GDI");
}

#endif

VideoRendererId GDIRenderer::id() const
{
    return VideoRendererId_GDI;
}

class Q_DECL_HIDDEN GDIRendererPrivate : public VideoRendererPrivate
{
public:

    DPTR_DECLARE_PUBLIC(GDIRenderer)

    GDIRendererPrivate()
      : VideoRendererPrivate(),
        support_bitblt      (true),
        gdiplus_token       (0),
        device_context      (0)

#if USE_GRAPHICS

        graphics            (nullptr)

#endif

    {
        GdiplusStartupInput gdiplusStartupInput;
        GdiplusStartup(&gdiplus_token, &gdiplusStartupInput, nullptr);
    }

    ~GDIRendererPrivate()
    {
        if (device_context)
        {
            DPTR_P(GDIRenderer);
            ReleaseDC((HWND)p.winId(), device_context); // Qt5: must cast WID to HWND

#if !USE_GRAPHICS

            DeleteDC(off_dc);

#endif

            device_context = 0;
        }

        GdiplusShutdown(gdiplus_token);
    }

    void prepare()
    {
        update_background = true;
        DPTR_P(GDIRenderer);
        device_context    = GetDC((HWND)p.winId()); /* Qt5: must cast WID to HWND */

#if USE_GRAPHICS

        if (graphics)
        {
            delete graphics;
        }

        graphics = new Graphics(device_context);

#endif

        // TODO: check bitblt support

        int ret  = GetDeviceCaps(device_context, RC_BITBLT);

        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("bitblt=%d", ret);

        // TODO: wingapi? vlc

#if 0

        BITMAPINFOHEADER bih;
        bih.biSize          = sizeof(BITMAPINFOHEADER);
        bih.biSizeImage     = 0;
        bih.biPlanes        = 1;
        bih.biCompression   = BI_RGB; // vlc: 16bpp=>BI_RGB, 15bpp=>BI_BITFIELDS
        bih.biBitCount      = 32;
        bih.biWidth         = src_width;
        bih.biHeight        = src_height;
        bih.biClrImportant  = 0;
        bih.biClrUsed       = 0;
        bih.biXPelsPerMeter = 0;
        bih.biYPelsPerMeter = 0;

        off_bitmap = CreateDIBSection(device_context,
                                      (BITMAPINFO*)&bih,
                                      DIB_RGB_COLORS,
                                      &p_pic_buffer, nullptr, 0);
#endif

#if !USE_GRAPHICS

        off_dc = CreateCompatibleDC(device_context);

#endif

    }

    void setupQuality() override
    {
        // http://www.codeproject.com/Articles/9184/Custom-AntiAliasing-with-GDI                // krazy:exclude=insecurenet
        // http://msdn.microsoft.com/en-us/library/windows/desktop/ms533836%28v=vs.85%29.aspx   // krazy:exclude=insecurenet

        /*
         * Graphics.DrawImage, Graphics.InterpolationMode
         * bitblit?
         */

#if USE_GRAPHICS

        if (!graphics)
            return;

        switch (quality)
        {
            case QtAV::QualityBest:
            {
                graphics->SetInterpolationMode(InterpolationModeHighQualityBicubic);

                break;
            }

            case QtAV::QualityFastest:
            {
                graphics->SetInterpolationMode(InterpolationModeNearestNeighbor);

                break;
            }

            default:
            {
                graphics->SetInterpolationMode(InterpolationModeDefault);

                break;
            }
        }

#endif

    }

public:

    bool      support_bitblt;
    ULONG_PTR gdiplus_token;

    /*
     * GetDC(winID()): will flick
     * QPainter.paintEngine()->getDC() in paintEvent: doc says it's for internal use
     */
    HDC       device_context;

    /* Our offscreen bitmap and its framebuffer */

#if USE_GRAPHICS

    Graphics* graphics;

#else

    HDC       off_dc;
    HBITMAP   off_bitmap;

#endif

};

GDIRenderer::GDIRenderer(QWidget* const parent, Qt::WindowFlags f)
    : QWidget      (parent, f),
      VideoRenderer(*new GDIRendererPrivate())
{
    DPTR_INIT_PRIVATE(GDIRenderer);

    setAcceptDrops(true);
    setFocusPolicy(Qt::StrongFocus);

    /* To rapidly update custom widgets that constantly paint over their entire areas with
     * opaque content, e.g., video streaming widgets, it is better to set the widget's
     * Qt::WA_OpaquePaintEvent, avoiding any unnecessary overhead associated with repainting the
     * widget's background
     */
    setAttribute(Qt::WA_OpaquePaintEvent);
/*
    setAttribute(Qt::WA_NoSystemBackground);
*/
    setAutoFillBackground(false);
/*
    setDCFromPainter(false); //changeEvent will be called on show()
*/
    setAttribute(Qt::WA_PaintOnScreen, true);
}

bool GDIRenderer::isSupported(VideoFormat::PixelFormat pixfmt) const
{
    return VideoFormat::isRGB(pixfmt);
}

QPaintEngine* GDIRenderer::paintEngine() const
{
    return nullptr;
}

bool GDIRenderer::receiveFrame(const VideoFrame& frame)
{
    DPTR_D(GDIRenderer);

    if (frame.constBits(0))
        d.video_frame = frame;
    else
        d.video_frame = frame.to(frame.pixelFormat());

    updateUi();

    return true;
}

void GDIRenderer::drawBackground()
{
    const QRegion bgRegion(backgroundRegion());

    if (bgRegion.isEmpty())
        return;

    const QColor bc(backgroundColor());

    DPTR_D(GDIRenderer);
/*
    HDC hdc = d.device_context;
*/
    Graphics g(d.device_context);
    SolidBrush brush(Color(bc.alpha(), bc.red(), bc.green(), bc.blue())); // argb

    for (const QRect& r : bgRegion)
    {
        g.FillRectangle(&brush, r.x(), r.y(), r.width(), r.height());
    }
}

void GDIRenderer::drawFrame()
{
    DPTR_D(GDIRenderer);

    /* http://msdn.microsoft.com/en-us/library/windows/desktop/ms533829%28v=vs.85%29.aspx       // krazy:exclude=insecurenet
     * Improving Performance by Avoiding Automatic Scaling
     * TODO: How about QPainter?
     */

    // steps to use BitBlt: http://bbs.csdn.net/topics/60183502                                 // krazy:exclude=insecurenet

    Bitmap bitmap(d.video_frame.width(), d.video_frame.height(), d.video_frame.bytesPerLine(0),
                  PixelFormat32bppRGB, (BYTE*)d.video_frame.constBits(0));

#if USE_GRAPHICS

    if (d.graphics)
        d.graphics->DrawImage(&bitmap, d.out_rect.x(), d.out_rect.y(), d.out_rect.width(), d.out_rect.height());

#else

    if (FAILED(bitmap.GetHBITMAP(Color(), &d.off_bitmap)))
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote() << QString::asprintf("Failed GetHBITMAP");

        return;
    }

    HDC hdc          = d.device_context;
    HBITMAP hbmp_old = (HBITMAP)SelectObject(d.off_dc, d.off_bitmap);
    QRect roi        = realROI();

    // && image.size() != size()
    // assume that the image data is already scaled to out_size(NOT renderer size!)

    StretchBlt(hdc,
               d.out_rect.left(),
               d.out_rect.top(),
               d.out_rect.width(),
               d.out_rect.height(),
               d.off_dc,
               roi.x(),
               roi.y(),
               roi.width(),
               roi.height(),
               SRCCOPY);

    SelectObject(d.off_dc, hbmp_old);
    DeleteObject(d.off_bitmap);         // avoid mem leak

#endif

}

void GDIRenderer::paintEvent(QPaintEvent*)
{
    handlePaintEvent();
}

void GDIRenderer::resizeEvent(QResizeEvent* e)
{
    d_func().update_background = true;
    resizeRenderer(e->size());
    update();
}

void GDIRenderer::showEvent(QShowEvent*)
{
    DPTR_D(GDIRenderer);

    d.update_background = true;
    d_func().prepare();
}

IMPLEMENT_VIDEO_RENDERER_EMIT_METHODS(GDIRenderer)

} // namespace QtAV

#include "GDIRenderer.moc"
