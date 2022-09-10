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

#include "SurfaceInteropD3D9.h"

#define DX_LOG_COMPONENT "D3D9 Interop"

// Local includes

#include "VideoFrame.h"
#include "DirectXHelper.h"
#include "OpenGLHelper.h"
#include "VideoDecoderD3D.h"
#include "digikam_debug.h"

// no need to check qt4 because no ANGLE there

#if QTAV_HAVE(EGL_CAPI) // always use dynamic load
#   if defined(QT_OPENGL_DYNAMIC) || defined(QT_OPENGL_ES_2) || defined(QT_OPENGL_ES_2_ANGLE)
#       define QTAV_HAVE_D3D9_EGL 1
#   endif
#endif

#if defined(QT_OPENGL_DYNAMIC) || !defined(QT_OPENGL_ES_2)
#   define QTAV_HAVE_D3D9_GL 1
#endif

#define MS_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    static const GUID name = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }

namespace QtAV
{

MS_GUID(IID_IDirect3DDevice9Ex, 0xb18b10ce, 0x2649, 0x405a, 0x87, 0xf, 0x95, 0xf7, 0x77, 0xd4, 0x31, 0x3a);

namespace d3d9
{

bool InteropResource::isSupported(InteropType type)
{
    Q_UNUSED(type);

#if QTAV_HAVE(D3D9_EGL)

    if ((type == InteropAuto) || (type == InteropEGL))
    {
        if (OpenGLHelper::isOpenGLES())
            return true;
    }

#endif

#if QTAV_HAVE(D3D9_GL)

    if ((type == InteropAuto) || (type == InteropGL))
    {
        if (!OpenGLHelper::isOpenGLES())
            return true;
    }

#endif

    return false;
}

extern InteropResource* CreateInteropEGL(IDirect3DDevice9* dev);
extern InteropResource* CreateInteropGL(IDirect3DDevice9* dev);

InteropResource* InteropResource::create(IDirect3DDevice9* dev, InteropType type)
{
    Q_UNUSED(dev);

    if ((type == InteropAuto) || (type == InteropEGL))
    {
        IDirect3DDevice9Ex* devEx = nullptr;
        dev->QueryInterface(IID_IDirect3DDevice9Ex, (void**)&devEx);
        qCDebug(DIGIKAM_QTAV_LOG).noquote() << QString::asprintf("using D3D9Ex: %d", !!devEx);

        if (!devEx)
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("IDirect3DDevice9Ex is required to share d3d resource. "
                                     "It's available in vista and later. d3d9 can not CreateTexture with shared handle");
        }

        SafeRelease(&devEx);

#if QTAV_HAVE(D3D9_EGL)

        if (OpenGLHelper::isOpenGLES())
            return CreateInteropEGL(dev);

#endif

    }

    if ((type == InteropAuto) || (type == InteropGL))
    {

#if QTAV_HAVE(D3D9_GL)

        if (!OpenGLHelper::isOpenGLES())
            return CreateInteropGL(dev);

#endif

    }

    return nullptr;
}

InteropResource::InteropResource(IDirect3DDevice9* d3device)
    : d3ddev    (d3device),
      dx_texture(nullptr),
      dx_surface(nullptr),
      width     (0),
      height    (0)
{
    d3ddev->AddRef();
}

InteropResource::~InteropResource()
{
    releaseDX();
    SafeRelease(&d3ddev);
}

void InteropResource::releaseDX()
{
    SafeRelease(&dx_surface);
    SafeRelease(&dx_texture);
}

SurfaceInterop::~SurfaceInterop()
{
    SafeRelease(&m_surface);
}

void SurfaceInterop::setSurface(IDirect3DSurface9* surface, int frame_w, int frame_h)
{
    m_surface    = surface;
    m_surface->AddRef();
    frame_width  = frame_w;
    frame_height = frame_h;
}

void* SurfaceInterop::map(SurfaceType type, const VideoFormat& fmt, void* handle, int plane)
{
    if (!handle)
        return nullptr;

    if (!m_surface)
        return 0;

    if      (type == GLTextureSurface)
    {
        if (!fmt.isRGB())
            return nullptr;

        if (m_resource->map(m_surface, *((GLuint*)handle), frame_width, frame_height, plane))
            return handle;
    }
    else if (type == HostMemorySurface)
    {
        return mapToHost(fmt, handle, plane);
    }

    return nullptr;
}

void SurfaceInterop::unmap(void* handle)
{
    m_resource->unmap(*((GLuint*)handle));
}

void* SurfaceInterop::mapToHost(const VideoFormat& format, void* handle, int plane)
{
    Q_UNUSED(plane);

    class Q_DECL_HIDDEN ScopedD3DLock
    {
        IDirect3DSurface9* mpD3D = nullptr;

    public:

        ScopedD3DLock(IDirect3DSurface9* const d3d, D3DLOCKED_RECT* const rect)
            : mpD3D(d3d)
        {
            if (mpD3D)
            {
                if (FAILED(mpD3D->LockRect(rect, nullptr, D3DLOCK_READONLY)))
                {
                    qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                        << QString::asprintf("Failed to lock surface");

                    mpD3D = nullptr;
                }
            }
        }

        ~ScopedD3DLock()
        {
            if (mpD3D)
                mpD3D->UnlockRect();
        }
    };

    D3DLOCKED_RECT lock;
    ScopedD3DLock(m_surface, &lock);             // cppcheck-suppress unusedScopedObject

    if (lock.Pitch == 0)
        return nullptr;

    // TODO: use the same code as VideoDecoderDXVA::frame() like vaapi.
    // picth >= desc.Width

    D3DSURFACE_DESC desc;
    m_surface->GetDesc(&desc);
    const VideoFormat fmt = VideoFormat(VideoDecoderD3D::pixelFormatFromFourcc(desc.Format));

    if (!fmt.isValid())
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("unsupported D3D9 pixel format: %#x",
                desc.Format);

        return nullptr;
    }

    // YV12 need swap, not imc3?
    // imc3 U V pitch == Y pitch, but half of the U/V plane is space. we convert to yuv420p here
    // nv12 bpp(1)==1
    // 3rd plane is not used for nv12

    int pitch[3]  = { lock.Pitch,          0, 0 };  // compute chroma later
    quint8* src[] = { (quint8*)lock.pBits, 0, 0 };  // compute chroma later

    Q_ASSERT(src[0] && (pitch[0] > 0));

    const bool swap_uv = (desc.Format == MAKEFOURCC('I','M','C','3'));

    // try to use SSE. fallback to normal copy if SSE is not supported

    VideoFrame frame(VideoFrame::fromGPU(fmt, frame_width, frame_height, desc.Height, src, pitch, true, swap_uv));

    // TODO: check rgb32 because d3d can use hw to convert

    if (format != fmt)
        frame = frame.to(format);

    VideoFrame* const f = reinterpret_cast<VideoFrame*>(handle);
    frame.setTimestamp(f->timestamp());
    *f                  = frame;

    return f;
}

} // namespace d3d9

} // namespace QtAV
