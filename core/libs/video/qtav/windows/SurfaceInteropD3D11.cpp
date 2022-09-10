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

#include "SurfaceInteropD3D11.h"

#define DX_LOG_COMPONENT "D3D11 Interop"

// C++ includes

#include <cstdint> // uint8_t for windows phone

// Local includes

#include "VideoFrame.h"
#include "DirectXHelper.h"
#include "OpenGLHelper.h"
#include "VideoDecoderD3D.h"
#include "digikam_debug.h"

// no need to check qt4 because no ANGLE there

#if QTAV_HAVE(EGL_CAPI)     // always use dynamic load
#   if defined(QT_OPENGL_DYNAMIC) || defined(QT_OPENGL_ES_2) || defined(QT_OPENGL_ES_2_ANGLE)
#       define QTAV_HAVE_D3D11_EGL 1
#   endif
#endif

#if defined(QT_OPENGL_DYNAMIC) || !defined(QT_OPENGL_ES_2)
#   define QTAV_HAVE_D3D11_GL 1
#endif

namespace QtAV
{

int fourccFromDXGI(DXGI_FORMAT fmt); // FIXME: defined in d3d11 decoder

namespace d3d11
{

bool InteropResource::isSupported(InteropType type)
{
    if ((type == InteropAuto) || (type == InteropEGL))
    {

#if QTAV_HAVE(D3D11_EGL)

        if (OpenGLHelper::isOpenGLES())
            return true;

#endif

    }

    if ((type == InteropAuto) || (type == InteropGL))
    {

#if QTAV_HAVE(D3D11_GL)

        if (!OpenGLHelper::isOpenGLES())
            return true;

#endif

    }

    return false;
}

extern InteropResource* CreateInteropEGL();
extern InteropResource* CreateInteropGL();

InteropResource* InteropResource::create(InteropType type)
{
    if ((type == InteropAuto) || (type == InteropEGL))
    {

#if QTAV_HAVE(D3D11_EGL)

        if (OpenGLHelper::isOpenGLES())
            return CreateInteropEGL();

#endif

    }

    if ((type == InteropAuto) || (type == InteropGL))
    {

#if QTAV_HAVE(D3D11_GL)

        if (!OpenGLHelper::isOpenGLES())
            return CreateInteropGL();

#endif

    }

    return nullptr;
}

void InteropResource::setDevice(ComPtr<ID3D11Device> dev)
{
    d3ddev = dev;
}

void SurfaceInterop::setSurface(ComPtr<ID3D11Texture2D> surface, int index, int frame_w, int frame_h)
{
    m_surface    = surface;
    m_index      = index;
    frame_width  = frame_w;
    frame_height = frame_h;
}

void* SurfaceInterop::map(SurfaceType type, const VideoFormat &fmt, void *handle, int plane)
{
    if (!handle)
        return nullptr;

    if (!m_surface)
        return 0;

    if      (type == GLTextureSurface)
    {
        if (m_resource->map(m_surface, m_index, *((GLuint*)handle), frame_width, frame_height, plane))
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
    ComPtr<ID3D11Device> dev;
    m_surface->GetDevice(&dev);
    D3D11_TEXTURE2D_DESC desc;
    m_surface->GetDesc(&desc);
    desc.MipLevels      = 1;
    desc.MiscFlags      = 0;
    desc.ArraySize      = 1;
    desc.Usage          = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.BindFlags      = 0; // ?
    ComPtr<ID3D11Texture2D> tex;
    DX_ENSURE(dev->CreateTexture2D(&desc, nullptr, &tex), nullptr);
    ComPtr<ID3D11DeviceContext> ctx;
    dev->GetImmediateContext(&ctx);

    ctx->CopySubresourceRegion(tex.Get(), 0, 0, 0, 0,
                               m_surface.Get(),
                               m_index,
                               nullptr);

    struct Q_DECL_HIDDEN ScopedMap
    {
        ScopedMap(ComPtr<ID3D11DeviceContext> ctx, ComPtr<ID3D11Texture2D> res, D3D11_MAPPED_SUBRESOURCE* mapped)
            : c(ctx),
              r(res)
        {
            DX_ENSURE(c->Map(r.Get(), 0, D3D11_MAP_READ, 0, mapped)); //TODO: check error
        }

        ~ScopedMap()
        {
            c->Unmap(r.Get(), 0);
        }

        ComPtr<ID3D11DeviceContext> c;
        ComPtr<ID3D11Texture2D>     r;
    };

    D3D11_MAPPED_SUBRESOURCE mapped;

    // mingw error if ComPtr<T> constructs from ComPtr<U> [T=ID3D11Resource, U=ID3D11Texture2D]

    ScopedMap sm(ctx, tex, &mapped);

    Q_UNUSED(sm);

    int pitch[3]          = { (int)mapped.RowPitch,   0, 0 }; // compute chroma later
    uint8_t* src[]        = { (uint8_t*)mapped.pData, 0, 0 }; // compute chroma later

    const VideoFormat fmt = VideoDecoderD3D::pixelFormatFromFourcc(fourccFromDXGI(desc.Format));
    VideoFrame frame      = VideoFrame::fromGPU(fmt, frame_width, frame_height, desc.Height, src, pitch);

    if (fmt != format)
        frame = frame.to(format);

    VideoFrame* f         = reinterpret_cast<VideoFrame*>(handle);
    frame.setTimestamp(f->timestamp());
    *f                    = frame;

    return f;
}

} // namespace d3d11

} // namespace QtAV
